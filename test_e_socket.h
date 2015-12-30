#ifdef INTERCEPT

#ifndef E_TEST_SOCKET
#define E_TEST_SOCKET

#include "testdefs_e_socket.h"

#include <netdb.h> // struct addrinfo
#include <poll.h> // Provides poll() which allows the server to be event-driven

#include <pthread.h> // pthread_create

int e_test_socket(int, int, int);
int e_test_bind(int, const struct sockaddr*, socklen_t);
int e_test_listen(int, int);
int e_test_connect(int, const struct sockaddr*, socklen_t);
int e_test_accept(int, struct sockaddr*, socklen_t*);
int e_test_getaddrinfo(const char*, const char*, const struct addrinfo*, struct addrinfo**);
int e_test_poll(struct pollfd[], nfds_t, int);
int e_test_pthread_create(pthread_t *restrict, const pthread_attr_t *restrict, void *(*)(void *), void *restrict);
int e_test_pthread_mutex_init(pthread_mutex_t *restrict, const pthread_mutexattr_t *restrict);
void* e_test_calloc(size_t, size_t);
void* e_test_realloc(void*, size_t);

#define socket(domain, type, protocol)\
    e_test_socket(domain, type, protocol)
#define bind(socket, addr, addrlen)\
    e_test_bind(socket, addr, addrlen)
#define listen(socket, backlog)\
    e_test_listen(socket, backlog)
#define connect(socket, addr, addrlen)\
    e_test_connect(socket, addr, addrlen)
#define accept(socket, addr, addrlen)\
    e_test_accept(socket, addr, addrlen)
#define getaddrinfo(host, service, hints, res)\
    e_test_getaddrinfo(host, service, hints, res)
#define pthread_create(thread, attr, start_routine, arg)\
    e_test_pthread_create(thread, attr, start_routine, arg)
#define pthread_mutex_init(mutex, attr)\
    e_test_pthread_mutex_init(mutex, attr)
#define calloc(count, size)\
    e_test_calloc(count, size)
#define realloc(ptr, size)\
    e_test_realloc(ptr, size)
#define poll(fds, nfds, timeout)\
    e_test_poll(fds, nfds, timeout)

#endif // E_TEST_SOCKET

#endif // INTERCEPT
