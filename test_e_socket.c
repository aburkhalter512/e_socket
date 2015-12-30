#ifdef INTERCEPT

#include "testdefs_e_socket.h"

#include <sys/socket.h> // socket, bind, listen, accept
#include <sys/types.h> // struct addrinfo
#include <netdb.h>
#include <poll.h> // Provides poll() which allows the server to be event-driven

#include <stdlib.h> // realloc
#include <assert.h> // assert
#include <errno.h>

#include <pthread.h> // pthread_create

static enum E_TEST_TYPE e_test_socket_type;
static int e_test_socket_retVal;
static int e_test_socket_errno;
void e_test_set_socket(enum E_TEST_TYPE type, int retVal, int _errno)
{
    e_test_socket_type = type;
    e_test_socket_retVal = retVal;
    e_test_socket_errno = _errno;
}
int e_test_socket(int domain, int type, int protocol)
{
    switch (e_test_socket_type)
    {
        case E_TEST_NORM:
            return socket(domain, type, protocol);
        case E_TEST_TEST:
            errno = e_test_socket_errno;
            return e_test_socket_retVal;
        default:
            return -1;
    }
}

static enum E_TEST_TYPE e_test_bind_type;
static int e_test_bind_retVal;
static int e_test_bind_errno;
void e_test_set_bind(enum E_TEST_TYPE type, int retVal, int _errno)
{
    e_test_bind_type = type;
    e_test_bind_retVal = retVal;
    e_test_bind_errno = _errno;
}
int e_test_bind(int socket, const struct sockaddr* addr, socklen_t addrlen)
{
    switch (e_test_bind_type)
    {
        case E_TEST_NORM:
            return bind(socket, addr, addrlen);
        case E_TEST_TEST:
            errno = e_test_bind_errno;
            return e_test_bind_retVal;
        default:
            return -1;
    }
}

static enum E_TEST_TYPE e_test_listen_type;
static int e_test_listen_retVal;
static int e_test_listen_errno;
void e_test_set_listen(enum E_TEST_TYPE type, int retVal, int _errno)
{
    e_test_listen_type = type;
    e_test_listen_retVal = retVal;
    e_test_listen_errno = _errno;
}
int e_test_listen(int socket, int backlog)
{
    switch (e_test_listen_type)
    {
        case E_TEST_NORM:
            return listen(socket, backlog);
        case E_TEST_TEST:
            errno = e_test_listen_errno;
            return e_test_listen_retVal;
        default:
            return -1;
    }
}

static enum E_TEST_TYPE e_test_connect_type;
static int e_test_connect_retVal;
static int e_test_connect_errno;
void e_test_set_connect(enum E_TEST_TYPE type, int retVal, int _errno)
{
    e_test_connect_type = type;
    e_test_connect_retVal = retVal;
    e_test_connect_errno = _errno;
}
int e_test_connect(int socket, const struct sockaddr* addr, socklen_t addrlen)
{
    switch (e_test_connect_type)
    {
        case E_TEST_NORM:
            return connect(socket, addr, addrlen);
        case E_TEST_TEST:
            errno = e_test_connect_errno;
            return e_test_connect_retVal;
        default:
            return -1;
    }
}

static enum E_TEST_TYPE e_test_accept_type;
static int e_test_accept_retVal;
static int e_test_accept_errno;
void e_test_set_accept(enum E_TEST_TYPE type, int retVal, int _errno)
{
    e_test_accept_type = type;
    e_test_accept_retVal = retVal;
    e_test_accept_errno = _errno;
}
int e_test_accept(int socket, struct sockaddr* addr, socklen_t* addrlen)
{
    switch (e_test_accept_type)
    {
        case E_TEST_NORM:
            return accept(socket, addr, addrlen);
        case E_TEST_TEST:
            errno = e_test_accept_errno;
            return e_test_accept_retVal;
        default:
            return -1;
    }
}

static enum E_TEST_TYPE e_test_getaddrinfo_type;
static int e_test_getaddrinfo_retVal;
static int e_test_getaddrinfo_errno;
void e_test_set_getaddrinfo(enum E_TEST_TYPE type, int retVal, int _errno)
{
    e_test_getaddrinfo_type = type;
    e_test_getaddrinfo_retVal = retVal;
    e_test_getaddrinfo_errno = _errno;
}
int e_test_getaddrinfo(
    const char* host,
    const char* service,
    const struct addrinfo* hints,
    struct addrinfo** res)
{
    switch (e_test_getaddrinfo_type)
    {
        case E_TEST_NORM:
            return getaddrinfo(host, service, hints, res);
        case E_TEST_TEST:
            errno = e_test_getaddrinfo_errno;
            return e_test_getaddrinfo_retVal;
        default:
            return -1;
    }
}

static enum E_TEST_TYPE e_test_poll_type;
static int e_test_poll_retVal;
static int e_test_poll_errno;
void e_test_set_poll(enum E_TEST_TYPE type, int retVal, int _errno)
{
    e_test_poll_type = type;
    e_test_poll_retVal = retVal;
    e_test_poll_errno = _errno;
}
int e_test_poll(struct pollfd fds[], nfds_t nfds, int timeout)
{
    switch (e_test_poll_type)
    {
        case E_TEST_NORM:
            return poll(fds, nfds, timeout);
        case E_TEST_TEST:
            errno = e_test_poll_errno;
            return e_test_poll_retVal;
        default:
            return -1;
    }
}

static enum E_TEST_TYPE e_test_pthread_create_type;
static int e_test_pthread_create_retVal;
static int e_test_pthread_create_errno;
void e_test_set_pthread_create(enum E_TEST_TYPE type, int retVal, int _errno)
{
    e_test_pthread_create_type = type;
    e_test_pthread_create_retVal = retVal;
    e_test_pthread_create_errno = _errno;
}
int e_test_pthread_create(
    pthread_t *restrict thread,
    const pthread_attr_t *restrict attr,
    void *(*start_routine)(void *),
    void *restrict arg)
{
    switch (e_test_pthread_create_type)
    {
        case E_TEST_NORM:
            return pthread_create(thread, attr, start_routine, arg);
        case E_TEST_TEST:
            errno = e_test_pthread_create_errno;
            return e_test_pthread_create_retVal;
        default:
            return -1;
    }
}

static enum E_TEST_TYPE e_test_pthread_mutex_init_type;
static int e_test_pthread_mutex_init_retVal;
static int e_test_pthread_mutex_init_errno;
void e_test_set_pthread_mutex_init(enum E_TEST_TYPE type, int retVal, int _errno)
{
    e_test_pthread_mutex_init_type = type;
    e_test_pthread_mutex_init_retVal = retVal;
    e_test_pthread_mutex_init_errno = _errno;
}
int e_test_pthread_mutex_init(
    pthread_mutex_t *restrict mutex,
    const pthread_mutexattr_t *restrict attr)
{
    switch (e_test_pthread_mutex_init_type)
    {
        case E_TEST_NORM:
            return pthread_mutex_init(mutex, attr);
        case E_TEST_TEST:
            errno = e_test_pthread_mutex_init_errno;
            return e_test_pthread_mutex_init_retVal;
        default:
            return -1;
    }
}

static enum E_TEST_TYPE e_test_calloc_type;
static void* e_test_calloc_retVal;
void e_test_set_calloc(enum E_TEST_TYPE type, void* retVal)
{
    e_test_calloc_type = type;
    e_test_calloc_retVal = retVal;
}
void* e_test_calloc(size_t count, size_t size)
{
    switch (e_test_calloc_type)
    {
        case E_TEST_NORM:
            return calloc(count, size);
        case E_TEST_TEST:
            return e_test_calloc_retVal;
        default:
            return NULL;
    }
}

static enum E_TEST_TYPE e_test_realloc_type;
static void* e_test_realloc_retVal;
void e_test_set_realloc(enum E_TEST_TYPE type, void* retVal)
{
    e_test_realloc_type = type;
    e_test_realloc_retVal = retVal;
}
void* e_test_realloc(void* ptr, size_t size)
{
    switch (e_test_realloc_type)
    {
        case E_TEST_NORM:
            return realloc(ptr, size);
        case E_TEST_TEST:
            return e_test_realloc_retVal;
        default:
            return NULL;
    }
}

#endif // INTERCEPT
