#ifndef INTERCEPT_DEFS
#define INTERCEPT_DEFS

#include <netdb.h> // struct addrinfo
#include <poll.h> // Provides poll() which allows the server to be event-driven

#include <pthread.h> // pthread_create

// Internal
int intercept_socket(int, int, int);
int intercept_bind(int, const struct sockaddr*, socklen_t);
int intercept_listen(int, int);
int intercept_connect(int, const struct sockaddr*, socklen_t);
int intercept_accept(int, struct sockaddr*, socklen_t*);
int intercept_getaddrinfo(const char*, const char*, const struct addrinfo*, struct addrinfo**);
int intercept_poll(struct pollfd[], nfds_t, int);
int intercept_pthread_create(pthread_t *restrict, const pthread_attr_t *restrict, void *(*)(void *), void *restrict);
int intercept_pthread_mutex_init(pthread_mutex_t *restrict, const pthread_mutexattr_t *restrict);
void* intercept_calloc(size_t, size_t);
void* intercept_realloc(void*, size_t);

// External
enum INTERCEPT_TYPE
{
    INTERCEPT_NORM,
    INTERCEPT_INTR
};

void intercept_set_socket(enum INTERCEPT_TYPE, int, int);
void intercept_set_bind(enum INTERCEPT_TYPE, int, int);
void intercept_set_listen(enum INTERCEPT_TYPE, int, int);
void intercept_set_connect(enum INTERCEPT_TYPE, int, int);
void intercept_set_accept(enum INTERCEPT_TYPE, int, int);
void intercept_set_getaddrinfo(enum INTERCEPT_TYPE, int, int);
void intercept_set_poll(enum INTERCEPT_TYPE, int, int);
void intercept_set_pthread_create(enum INTERCEPT_TYPE, int, int);
void intercept_set_pthread_mutex_init(enum INTERCEPT_TYPE, int, int);
void intercept_set_calloc(enum INTERCEPT_TYPE, void*);
void intercept_set_realloc(enum INTERCEPT_TYPE, void*);

#endif // INTERCEPT_DEFS
