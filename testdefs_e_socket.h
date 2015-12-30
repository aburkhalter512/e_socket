#ifdef INTERCEPT

#ifndef E_TEST_SOCKET_DEFS
#define E_TEST_SOCKET_DEFS

enum E_TEST_TYPE
{
    E_TEST_NORM,
    E_TEST_TEST
};

void e_test_set_socket(enum E_TEST_TYPE, int, int);
void e_test_set_bind(enum E_TEST_TYPE, int, int);
void e_test_set_listen(enum E_TEST_TYPE, int, int);
void e_test_set_connect(enum E_TEST_TYPE, int, int);
void e_test_set_accept(enum E_TEST_TYPE, int, int);
void e_test_set_getaddrinfo(enum E_TEST_TYPE, int, int);
void e_test_set_poll(enum E_TEST_TYPE, int, int);
void e_test_set_pthread_create(enum E_TEST_TYPE, int, int);
void e_test_set_pthread_mutex_init(enum E_TEST_TYPE, int, int);
void e_test_set_calloc(enum E_TEST_TYPE, void*);
void e_test_set_realloc(enum E_TEST_TYPE, void*);

#endif // E_TEST_SOCKET_DEFS
#endif // INTERCEPT
