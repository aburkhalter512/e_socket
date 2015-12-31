#ifndef E_SOCKET
#define E_SOCKET

#include <netinet/in.h> // struct sockaddr_in
#include <pthread.h>

struct e_server;
struct e_node;

// Defines the argument passed into the e_handler function
struct e_handler_arg
{
    struct e_node* c; // connection
    void* arg;
};

// Event handler definition
typedef void (*e_handler) (struct e_handler_arg*);

// Contains all event handlers for a given socket. Event handlers can be
// changed at any time with no adverse affects when the event is triggered.
struct e_node
{
    int socket;

    struct sockaddr_in addr;

    e_handler on_error; // arg: null
    e_handler on_close; // arg: null
    e_handler on_oob_read_ready; // arg: int; number of oob bytes ready to be read
    e_handler on_read_ready; // arg: int; number of norm bytes ready to be read
    e_handler on_oob_send_ready; // arg: null
    e_handler on_send_ready; // arg: null
};

enum E_SERVER_CMD
{
    WORK,   // Process server as normal, accepting new connections and processing their event handlers
    KILL,   // Kill the server and all spawned connections
    PEND    // Stop the server from accepting new connections, but continue to process event handlers
};

// Contains the all needed information for a given event server, including the
// listening socket (for new connections), the necessary event handlers and all
// connections created by the server.
struct e_server
{
    int e_listen_fd;
    unsigned short e_listen_port;
    pthread_t e_thread;

    enum E_SERVER_CMD e_command;

    e_handler on_new_connection; // arg: e_server*
    e_handler on_close;
};

// Defines the various return values the library functions can return.
typedef enum
{
    E_SUCCESS,
    E_BAD_ARG,  // Indicates at least one argument does not meet preconditions
    E_SOC_ERR,  // Indicates that some type of socket error occurred
    E_SYS_ERR,  // Indicates a system error has occurred (usually a thread wasn't created)
} E_RESULT;

// start_e_server(server: e_server*): E_RESULT
//
// Starts a event driven server on a new thread.
//
// To kill the server thread, either close the server file descriptor or set
// server->e_command = E_SERVER_CMD.KILL. Killing the server will also close any
// connections created by this server, so only when you want all connections
// to be closed should you kill the server.
E_RESULT start_e_server(struct e_server* server);

E_RESULT e_connect(const char host[], const char service[], struct e_node* connection);

#endif // E_SOCKET
