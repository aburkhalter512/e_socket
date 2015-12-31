#ifdef INTERCEPT

#ifndef _INTERCEPT
#define _INTERCEPT

#include "intercept_defs.h"

#define socket(domain, type, protocol)\
    intercept_socket(domain, type, protocol)
#define bind(socket, addr, addrlen)\
    intercept_bind(socket, addr, addrlen)
#define listen(socket, backlog)\
    intercept_listen(socket, backlog)
#define connect(socket, addr, addrlen)\
    intercept_connect(socket, addr, addrlen)
#define accept(socket, addr, addrlen)\
    intercept_accept(socket, addr, addrlen)
#define getaddrinfo(host, service, hints, res)\
    intercept_getaddrinfo(host, service, hints, res)
#define pthread_create(thread, attr, start_routine, arg)\
    intercept_pthread_create(thread, attr, start_routine, arg)
#define pthread_mutex_init(mutex, attr)\
    intercept_pthread_mutex_init(mutex, attr)
#define calloc(count, size)\
    intercept_calloc(count, size)
#define realloc(ptr, size)\
    intercept_realloc(ptr, size)
#define poll(fds, nfds, timeout)\
    intercept_poll(fds, nfds, timeout)

#endif // _INTERCEPT

#endif // INTERCEPT
