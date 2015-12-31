#include "intercept_defs.h"

#include <sys/socket.h> // socket, bind, listen, accept
#include <sys/types.h>

#include <stdlib.h> // realloc
#include <assert.h> // assert
#include <errno.h>

static enum INTERCEPT_TYPE intercept_socket_type;
static int intercept_socket_retVal;
static int intercept_socket_errno;
void intercept_set_socket(enum INTERCEPT_TYPE type, int retVal, int _errno)
{
    intercept_socket_type = type;
    intercept_socket_retVal = retVal;
    intercept_socket_errno = _errno;
}
int intercept_socket(int domain, int type, int protocol)
{
    switch (intercept_socket_type)
    {
        case INTERCEPT_NORM:
            return socket(domain, type, protocol);
        case INTERCEPT_INTR:
            errno = intercept_socket_errno;
            return intercept_socket_retVal;
        default:
            return -1;
    }
}

static enum INTERCEPT_TYPE intercept_bind_type;
static int intercept_bind_retVal;
static int intercept_bind_errno;
void intercept_set_bind(enum INTERCEPT_TYPE type, int retVal, int _errno)
{
    intercept_bind_type = type;
    intercept_bind_retVal = retVal;
    intercept_bind_errno = _errno;
}
int intercept_bind(int socket, const struct sockaddr* addr, socklen_t addrlen)
{
    switch (intercept_bind_type)
    {
        case INTERCEPT_NORM:
            return bind(socket, addr, addrlen);
        case INTERCEPT_INTR:
            errno = intercept_bind_errno;
            return intercept_bind_retVal;
        default:
            return -1;
    }
}

static enum INTERCEPT_TYPE intercept_listen_type;
static int intercept_listen_retVal;
static int intercept_listen_errno;
void intercept_set_listen(enum INTERCEPT_TYPE type, int retVal, int _errno)
{
    intercept_listen_type = type;
    intercept_listen_retVal = retVal;
    intercept_listen_errno = _errno;
}
int intercept_listen(int socket, int backlog)
{
    switch (intercept_listen_type)
    {
        case INTERCEPT_NORM:
            return listen(socket, backlog);
        case INTERCEPT_INTR:
            errno = intercept_listen_errno;
            return intercept_listen_retVal;
        default:
            return -1;
    }
}

static enum INTERCEPT_TYPE intercept_connect_type;
static int intercept_connect_retVal;
static int intercept_connect_errno;
void intercept_set_connect(enum INTERCEPT_TYPE type, int retVal, int _errno)
{
    intercept_connect_type = type;
    intercept_connect_retVal = retVal;
    intercept_connect_errno = _errno;
}
int intercept_connect(int socket, const struct sockaddr* addr, socklen_t addrlen)
{
    switch (intercept_connect_type)
    {
        case INTERCEPT_NORM:
            return connect(socket, addr, addrlen);
        case INTERCEPT_INTR:
            errno = intercept_connect_errno;
            return intercept_connect_retVal;
        default:
            return -1;
    }
}

static enum INTERCEPT_TYPE intercept_accept_type;
static int intercept_accept_retVal;
static int intercept_accept_errno;
void intercept_set_accept(enum INTERCEPT_TYPE type, int retVal, int _errno)
{
    intercept_accept_type = type;
    intercept_accept_retVal = retVal;
    intercept_accept_errno = _errno;
}
int intercept_accept(int socket, struct sockaddr* addr, socklen_t* addrlen)
{
    switch (intercept_accept_type)
    {
        case INTERCEPT_NORM:
            return accept(socket, addr, addrlen);
        case INTERCEPT_INTR:
            errno = intercept_accept_errno;
            return intercept_accept_retVal;
        default:
            return -1;
    }
}

static enum INTERCEPT_TYPE intercept_getaddrinfo_type;
static int intercept_getaddrinfo_retVal;
static int intercept_getaddrinfo_errno;
void intercept_set_getaddrinfo(enum INTERCEPT_TYPE type, int retVal, int _errno)
{
    intercept_getaddrinfo_type = type;
    intercept_getaddrinfo_retVal = retVal;
    intercept_getaddrinfo_errno = _errno;
}
int intercept_getaddrinfo(
    const char* host,
    const char* service,
    const struct addrinfo* hints,
    struct addrinfo** res)
{
    switch (intercept_getaddrinfo_type)
    {
        case INTERCEPT_NORM:
            return getaddrinfo(host, service, hints, res);
        case INTERCEPT_INTR:
            errno = intercept_getaddrinfo_errno;
            return intercept_getaddrinfo_retVal;
        default:
            return -1;
    }
}

static enum INTERCEPT_TYPE intercept_poll_type;
static int intercept_poll_retVal;
static int intercept_poll_errno;
void intercept_set_poll(enum INTERCEPT_TYPE type, int retVal, int _errno)
{
    intercept_poll_type = type;
    intercept_poll_retVal = retVal;
    intercept_poll_errno = _errno;
}
int intercept_poll(struct pollfd fds[], nfds_t nfds, int timeout)
{
    switch (intercept_poll_type)
    {
        case INTERCEPT_NORM:
            return poll(fds, nfds, timeout);
        case INTERCEPT_INTR:
            errno = intercept_poll_errno;
            return intercept_poll_retVal;
        default:
            return -1;
    }
}

static enum INTERCEPT_TYPE intercept_pthread_create_type;
static int intercept_pthread_create_retVal;
static int intercept_pthread_create_errno;
void intercept_set_pthread_create(enum INTERCEPT_TYPE type, int retVal, int _errno)
{
    intercept_pthread_create_type = type;
    intercept_pthread_create_retVal = retVal;
    intercept_pthread_create_errno = _errno;
}
int intercept_pthread_create(
    pthread_t *restrict thread,
    const pthread_attr_t *restrict attr,
    void *(*start_routine)(void *),
    void *restrict arg)
{
    switch (intercept_pthread_create_type)
    {
        case INTERCEPT_NORM:
            return pthread_create(thread, attr, start_routine, arg);
        case INTERCEPT_INTR:
            errno = intercept_pthread_create_errno;
            return intercept_pthread_create_retVal;
        default:
            return -1;
    }
}

static enum INTERCEPT_TYPE intercept_pthread_mutex_init_type;
static int intercept_pthread_mutex_init_retVal;
static int intercept_pthread_mutex_init_errno;
void intercept_set_pthread_mutex_init(enum INTERCEPT_TYPE type, int retVal, int _errno)
{
    intercept_pthread_mutex_init_type = type;
    intercept_pthread_mutex_init_retVal = retVal;
    intercept_pthread_mutex_init_errno = _errno;
}
int intercept_pthread_mutex_init(
    pthread_mutex_t *restrict mutex,
    const pthread_mutexattr_t *restrict attr)
{
    switch (intercept_pthread_mutex_init_type)
    {
        case INTERCEPT_NORM:
            return pthread_mutex_init(mutex, attr);
        case INTERCEPT_INTR:
            errno = intercept_pthread_mutex_init_errno;
            return intercept_pthread_mutex_init_retVal;
        default:
            return -1;
    }
}

static enum INTERCEPT_TYPE intercept_calloc_type;
static void* intercept_calloc_retVal;
void intercept_set_calloc(enum INTERCEPT_TYPE type, void* retVal)
{
    intercept_calloc_type = type;
    intercept_calloc_retVal = retVal;
}
void* intercept_calloc(size_t count, size_t size)
{
    switch (intercept_calloc_type)
    {
        case INTERCEPT_NORM:
            return calloc(count, size);
        case INTERCEPT_INTR:
            return intercept_calloc_retVal;
        default:
            return NULL;
    }
}

static enum INTERCEPT_TYPE intercept_realloc_type;
static void* intercept_realloc_retVal;
void intercept_set_realloc(enum INTERCEPT_TYPE type, void* retVal)
{
    intercept_realloc_type = type;
    intercept_realloc_retVal = retVal;
}
void* intercept_realloc(void* ptr, size_t size)
{
    switch (intercept_realloc_type)
    {
        case INTERCEPT_NORM:
            return realloc(ptr, size);
        case INTERCEPT_INTR:
            return intercept_realloc_retVal;
        default:
            return NULL;
    }
}
