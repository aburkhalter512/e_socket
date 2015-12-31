#include "e_socket.h"
#include "test_e_socket.h"
#include "coverage.h"

#include <stdio.h> // perror
#include <string.h> // memset

#include <arpa/inet.h> // htons
#include <unistd.h> // close
#include <sys/ioctl.h> // ioctl
#include <poll.h> // Provides poll() which allows the server to be event-driven
#include <sys/socket.h> // socket, bind, listen, accept
#include <sys/types.h>
#include <netdb.h> // struct addrinfo

#include <stdlib.h> // realloc
#include <assert.h> // assert
#include <errno.h>

#include <pthread.h> // pthread_create

typedef char bool;
#define true 1
#define false 0

static const int SYS_ERR = -1;
static const int SYS_SUC = 0;
static const int TCP_PROTOCOL = 6; // see "protocols(5)"
static const int QUEUE_LIMIT = 128; // see "listen(2)"
static const int INIT_CON_LEN = 16;
static const short CONNECTION_EVENTS = POLLRDBAND | POLLRDNORM | POLLWRBAND | POLLWRNORM;
static const int TIMEOUT = 1000;

#define NAND(a, b) ~(a & b)

#define IF_FLAG(val, flag) if (val & flag)
#define NIF_FLAG(val, flag) if (NAND(val, flag))

// Async listener thread's main
static void* e_server_main(void*);
static void* e_client_main(void*);
static void e_handle_accept(struct e_server*, struct pollfd*, struct e_node*, struct pollfd*, int*, int*);
static void e_handle_connections(struct pollfd*, struct e_node*, int*);

// start_e_server(server: e_server*): E_RESULT
//
// Starts a new tcp, event-based server that listens for and accepts new
// connections on a single thread, which then spawns a single new thread for
// each new connection. Each new connection thread manages and calls the
// necessary events for a specific connection.
E_RESULT start_e_server(struct e_server* server)
{
    LANDMARK();

    if (!server || !server->on_new_connection || !server->on_close)
    {
        LANDMARK();
        return E_BAD_ARG;
    }

    server->e_listen_fd = socket(PF_INET, SOCK_STREAM, TCP_PROTOCOL);
    if (server->e_listen_fd == SYS_ERR)
    {
        LANDMARK();
        perror("Socket could not be created.");
        return E_SOC_ERR;
    }

    struct sockaddr_in listen_addr;
    memset(&listen_addr, 0, sizeof(struct sockaddr_in));

    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(server->e_listen_port);
    listen_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server->e_listen_fd, (struct sockaddr*) &listen_addr, sizeof(struct sockaddr_in)) == SYS_ERR)
    {
        LANDMARK();
        close(server->e_listen_fd);

        perror("Socket could not be bound.");
        return E_SOC_ERR;
    }

    if (listen(server->e_listen_fd, QUEUE_LIMIT) == SYS_ERR)
    {
        LANDMARK();
        close(server->e_listen_fd);

        perror("Listening on socket failed.");
        return E_SOC_ERR;
    }

    pthread_t e_server_thread;
    pthread_attr_t* DEFAULT_ATTR = NULL;

    if (pthread_create(&e_server_thread, DEFAULT_ATTR, e_server_main, (void*) server) != SYS_SUC)
    {
        LANDMARK();
        close(server->e_listen_fd);

        perror("Creating server thread failed.");
        return E_SYS_ERR;
    }

    return E_SUCCESS;
}

E_RESULT e_connect(const char host[], const char service[], struct e_node* connection)
{
    LANDMARK();

    if (host == NULL || service == NULL || connection == NULL)
    {
        LANDMARK();
        return E_BAD_ARG;
    }

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(host, service, &hints,  &res) != 0)
    {
        LANDMARK();
        return E_SOC_ERR;
    }

    connection->socket = socket(PF_INET, SOCK_STREAM, TCP_PROTOCOL);
    if (connection->socket == SYS_ERR)
    {
        LANDMARK();
        perror("Socket could not be created.");

        return E_SOC_ERR;
    }

    if (connect(connection->socket, res->ai_addr, res->ai_addrlen) == SYS_ERR)
    {
        LANDMARK();
        close(connection->socket);

        return E_SOC_ERR;
    }

    connection->addr = *((struct sockaddr_in*) res->ai_addr);

    static pthread_mutex_t e_client_mutex;
    static int e_client_cons_cap = 0;
    static int e_client_cons_len = 0;
    static struct e_node* e_client_cons = NULL;
    static struct pollfd* e_client_pollers = NULL;

    static bool e_first_client = true;
    if (e_first_client)
    {
        LANDMARK();
        pthread_mutexattr_t* DEFAULT_MUTEX_ATTR = NULL;
        if (pthread_mutex_init(&e_client_mutex, DEFAULT_MUTEX_ATTR) != SYS_SUC)
        {
            LANDMARK();
            close(connection->socket);

            return E_SYS_ERR;
        }

        e_client_cons_cap = INIT_CON_LEN;
        e_client_cons = calloc(sizeof(struct e_node), e_client_cons_cap);
        if (e_client_cons == NULL)
        {
            LANDMARK();
            pthread_mutex_destroy(&e_client_mutex);

            close(connection->socket);

            return E_SYS_ERR;
        }

        void* e_client_args[] = { e_client_cons, e_client_pollers, &e_client_cons_len, &e_client_mutex };
        pthread_t e_client_thread;
        pthread_attr_t* DEFAULT_ATTR = NULL;
        if (pthread_create(&e_client_thread, DEFAULT_ATTR, e_client_main, (void*) e_client_args) != SYS_SUC)
        {
            LANDMARK();
            pthread_mutex_destroy(&e_client_mutex);

            close(connection->socket);

            return E_SYS_ERR;
        }

        e_first_client = false;
    }

    pthread_mutex_lock(&e_client_mutex);
    LANDMARK();

    if (e_client_cons_len == e_client_cons_cap)
    {
        LANDMARK();
        void* new_mem = realloc(&e_client_cons, sizeof(struct e_node) * e_client_cons_cap * 2);
        if (new_mem == NULL)
        {
            LANDMARK();
            close(connection->socket);

            pthread_mutex_unlock(&e_client_mutex);

            return E_SYS_ERR;
        }
        e_client_cons = new_mem;

        new_mem = realloc(&e_client_pollers, sizeof(struct pollfd) * e_client_cons_cap);
        if (new_mem == NULL)
        {
            LANDMARK();
            close(connection->socket);

            pthread_mutex_unlock(&e_client_mutex);

            return E_SYS_ERR;
        }
        e_client_pollers = new_mem;

        e_client_cons_cap *= 2;
    }

    e_client_cons[e_client_cons_len] = *connection;

    e_client_pollers[e_client_cons_len].fd = connection->socket;
    e_client_pollers[e_client_cons_len].events = CONNECTION_EVENTS;

    e_client_cons_len++;

    pthread_mutex_unlock(&e_client_mutex);

    return E_SUCCESS;
}

void e_handle_accept(struct e_server* server, struct pollfd* accept_poller, struct e_node* connections, struct pollfd* pollers, int* p_connection_len, int* p_connection_cap)
{
    LANDMARK();
    // Verify new connections are available
    NIF_FLAG (accept_poller->revents, POLLRDNORM)
    {
        LANDMARK();

        return;
    }

    int connection_len = *p_connection_len;
    int connection_cap = *p_connection_cap;

    struct sockaddr_in connection_addr;
    socklen_t addr_len;

    int connection = accept(server->e_listen_fd, (struct sockaddr*) &connection_addr, &addr_len);

    // Error Check
    if (connection == SYS_ERR)
    {
        LANDMARK();
        switch (errno)
        {
            // Non-recoverable cases; see "accept(2)""
            case EBADF: // Bad file descriptor
            case EINVAL: // Socket unwilling to accept connections
            case ENOTSOCK: // Not a socket
            case EOPNOTSUPP: // Not a SOCK_STREAM socket
            case EFAULT: // Address not writable
                LANDMARK();
                return;
        }
    }

    // Expand memory, if necessary
    bool bad_mem = false;
    if (connection_len == connection_cap)
    {
        LANDMARK();
        void* new_con_mem = realloc(connections, sizeof(struct e_node) * connection_len * 2 + 1);
        if (new_con_mem != NULL) // NULL means mem allocation failed
        {
            LANDMARK();
            void* new_pol_mem = realloc(pollers, sizeof(struct pollfd) * connection_len * 2 + 2);
            if (new_pol_mem != NULL)
            {
                LANDMARK();
                connections = (struct e_node*) new_con_mem;
                pollers = (struct pollfd*) new_pol_mem;
                *p_connection_cap = connection_cap * 2 + 1;
            }
            else
                bad_mem = true;
        }
        else
            bad_mem = false;

    }

    // Verify memory expansion succeeded.
    if (bad_mem)
    {
        LANDMARK();
        close(connection);
        return;
    }

    LANDMARK();

    struct e_node* c = &connections[connection_len]; // Get the connection
    memset(c, 0, sizeof(struct e_node));

    c->socket = connection;
    c->addr = connection_addr;

    connection_len++; // ++ before pollers since pollers.len == connections.len + 1

    pollers[connection_len].fd = c->socket;
    pollers[connection_len].events = CONNECTION_EVENTS;

    *p_connection_len = connection_len;

    struct e_handler_arg e_arg;
    memset(&e_arg, 0, sizeof(struct e_handler_arg));

    e_arg.c = c;
    e_arg.arg = (void*) server;

    server->on_new_connection(&e_arg);
}

void e_handle_connections(struct pollfd* con_pollers, struct e_node* connections, int* connection_len)
{
    LANDMARK();

    int old_con_len = *connection_len;
    *connection_len = 0;

    // TODO: Describe the function of the loop and how connections get
    // readded to the list;
    for (int i = 0; i < old_con_len; i++)
    {
        LANDMARK();

        short e = con_pollers[i].revents;
        if (e == 0) // If no events occurred on this socket, continue
        {
            LANDMARK();
            continue;
        }

        struct e_node* c = &connections[i];

        struct e_handler_arg e_arg;
        e_arg.c = c;
        e_arg.arg = NULL;

        bool isOpen = true;

        // If the descriptor was closed, silently remove it from the
        // list of connections and continue to the next.
        IF_FLAG(e, POLLNVAL)
        {
            LANDMARK();
            continue;
        }
        IF_FLAG(e, POLLERR)
        {
            LANDMARK();
            // TODO: Figure out if we should close the socket on an err
            // or let the programmer decide that. If the latter, make
            // sure to add it back into the connections array.

            close(c->socket);
            isOpen = false;

            if (c->on_error)
            {
                LANDMARK();
                c->on_error(&e_arg);
            }
        }
        IF_FLAG(e, POLLHUP)
        {
            LANDMARK();
            // Close connection before e handler is called
            close(c->socket);
            isOpen = false;

            if (c->on_close)
            {
                LANDMARK();
                c->on_close(&e_arg);
            }
        }

        // If there isn't an open fd, then read/write won't work.
        if (!isOpen)
        {
            LANDMARK();
            continue;
        }

        IF_FLAG(e, POLLRDBAND)
        {
            LANDMARK();
            if (c->on_oob_read_ready)
            {
                LANDMARK();
                int bytes;
                ioctl(c->socket, FIONREAD, &bytes);

                e_arg.arg = &bytes;

                c->on_oob_read_ready(&e_arg);
            }
        }
        IF_FLAG(e, POLLRDNORM)
        {
            LANDMARK();
            if (c->on_read_ready)
            {
                LANDMARK();
                int bytes;
                ioctl(c->socket, FIONREAD, &bytes);

                e_arg.arg = &bytes;

                c->on_read_ready(&e_arg);
            }
        }
        IF_FLAG(e, POLLWRBAND)
        {
            LANDMARK();
            if (c->on_oob_send_ready)
            {
                LANDMARK();
                c->on_oob_send_ready(&e_arg);
            }
        }
        IF_FLAG(e, POLLWRNORM)
        {
            LANDMARK();
            if (c->on_send_ready)
            {
                LANDMARK();
                c->on_send_ready(&e_arg);
            }
        }

        // Add connections and pollers back into the list, which could
        // be at a prior point in the list.
        connections[*connection_len] = *c;
        con_pollers[*connection_len] = con_pollers[i];

        (*connection_len)++;
    }
}

void* e_server_main(void* v_server)
{
    LANDMARK();
    struct e_server* server = (struct e_server*) v_server;

    // Constants
    const int LISTEN_POLLER = 0;
    const short ACCEPT_EVENTS_WORK = POLLRDNORM | POLLHUP;
    const short ACCEPT_EVENTS_PEND = POLLHUP;

    // Initialization
    int connection_cap = INIT_CON_LEN - 1; // -1 to account for listen fd
    int connection_len = 0;
    struct e_node* connections = calloc(sizeof(struct e_node), connection_cap);
    if (connections == NULL)
    {
        LANDMARK();
        close(server->e_listen_fd);

        struct e_handler_arg e_arg;
        e_arg.c = NULL;
        e_arg.arg = server;
        server->on_close(&e_arg);

        return NULL;
    }
    struct pollfd* pollers = calloc(sizeof(struct pollfd), connection_cap + 1); // +1 to account for server fd
    if (pollers == NULL)
    {
        LANDMARK();
        close(server->e_listen_fd);

        free(connections);

        struct e_handler_arg e_arg;
        e_arg.c = NULL;
        e_arg.arg = server;
        server->on_close(&e_arg);

        return NULL;
    }

    pollers[0].fd = server->e_listen_fd;
    pollers[0].events = ACCEPT_EVENTS_WORK;

    bool accept_con = true;

    // Poll loop
    while (true)
    {
        LANDMARK();
        // A timeout is set so that commands can get through the poll on a
        // regular basis.
        int result = poll(pollers, connection_len + 1, TIMEOUT);

        assert(result != SYS_ERR || (errno != EFAULT && errno != EINVAL));

        // This is changed by the programmer to communicate with the thread
        // running this function.
        switch (server->e_command)
        {
            case WORK: // Do nothing
                LANDMARK();
                if (result > 0)
                {
                    LANDMARK();
                    pollers[LISTEN_POLLER].events = ACCEPT_EVENTS_WORK;

                    e_handle_accept(server, pollers, connections, pollers, &connection_len, &connection_cap);
                    e_handle_connections(pollers + 1, connections, &connection_len);
                }
                break;
            case PEND:
                LANDMARK();
                if (result > 0)
                {
                    LANDMARK();
                    pollers[LISTEN_POLLER].events = ACCEPT_EVENTS_PEND;

                    e_handle_connections(pollers + 1, connections, &connection_len);
                }
            case KILL:
                LANDMARK();
                close(server->e_listen_fd);

                for (int i = 0; i < connection_len; i++)
                {
                    LANDMARK();
                    close(connections[i].socket);
                }

                free(connections);
                free(pollers);

                return NULL;
        }
    }
}

void* e_client_main(void* _arg)
{
    LANDMARK();
    void** arg = (void**) _arg;
    struct e_node* e_client_cons = arg[0];
    struct pollfd* e_client_pollers = arg[1];
    int* e_client_cons_len = arg[2];
    pthread_mutex_t* e_client_mutex = arg[3];

    while(true)
    {
        LANDMARK();
        pthread_mutex_lock(e_client_mutex);
        LANDMARK();

        int result = poll(e_client_pollers, *e_client_cons_len, TIMEOUT);

        assert(result != SYS_ERR || (errno != EFAULT && errno != EINVAL));

        if (result > 0)
        {
            LANDMARK();
            e_handle_connections(e_client_pollers, e_client_cons, e_client_cons_len);
        }

        pthread_mutex_unlock(e_client_mutex);
    }

    return NULL;
}
