#include "test_main.h"

#include <errno.h>

#define SYS_ERR -1
#define NO_ARG 0

// Tests ===========================================================
TEST_FUNC(bad_server_args)
{
    LANDMARK(__FILE__, __LINE__)

    bool success = true;

    // NULL Arg test
    LANDMARK(__FILE__, __LINE__)
    E_RESULT e_result = start_e_server(NULL);
    if (e_result != E_BAD_ARG)
    {
        TEST_RESULTS(
            FAIL,
            "Bad E_RESULT: Expected: %s, Actual: %s",
                E_RESULT_to_str(E_BAD_ARG),
                E_RESULT_to_str(e_result))

        success = false;
    }

    // Both required function pointers are NULL
    LANDMARK(__FILE__, __LINE__)
    struct e_server bad_server;
    bad_server.e_listen_port = SERVER_PORT; // Not testing bad ports
    bad_server.on_close = NULL;
    bad_server.on_new_connection = NULL;
    e_result = start_e_server(&bad_server);
    if (e_result != E_BAD_ARG)
    {
        TEST_RESULTS(
            FAIL,
            "Bad E_RESULT: Expected: %s, Actual: %s",
                E_RESULT_to_str(E_BAD_ARG),
                E_RESULT_to_str(e_result))

        success = false;
    }


    // Only on_new_connection func pointer is NULL
    LANDMARK(__FILE__, __LINE__)
    bad_server.on_close = landmark_on_close;
    e_result = start_e_server(&bad_server);
    if (e_result != E_BAD_ARG)
    {
        TEST_RESULTS(
            FAIL,
            "Bad E_RESULT: Expected: %s, Actual: %s",
                E_RESULT_to_str(E_BAD_ARG),
                E_RESULT_to_str(e_result))

        success = false;
    }


    // Only on_close func pointer is NULL
    LANDMARK(__FILE__, __LINE__)
    bad_server.on_close = NULL;
    bad_server.on_new_connection = landmark_on_new_connection;
    e_result = start_e_server(&bad_server);
    if (e_result != E_BAD_ARG)
    {
        TEST_RESULTS(
            FAIL,
            "Bad E_RESULT: Expected: %s, Actual: %s",
                E_RESULT_to_str(E_BAD_ARG),
                E_RESULT_to_str(e_result))

        success = false;
    }


    // All good arguments
    LANDMARK(__FILE__, __LINE__)
    bad_server.on_close = landmark_on_close;
    bad_server.on_new_connection = landmark_on_new_connection;
    e_result = start_e_server(&bad_server);
    if (e_result != E_SUCCESS)
    {
        TEST_RESULTS(
            FAIL,
            "Bad E_RESULT: Expected: %s, Actual: %s",
                E_RESULT_to_str(E_SUCCESS),
                E_RESULT_to_str(e_result))

        success = false;
    }
    else
    {
        bad_server.e_command = KILL;

        void* join_val;
        pthread_join(bad_server.e_thread, &join_val);
    }

    if (success)
    {
        TEST_RESULTS(
            PASS,
            "%s",
            "Server arguments verified.")
    }

    LANDMARK(__FILE__, __LINE__)
}

TEST_FUNC(bad_server_sys_calls)
{
    LANDMARK(__FILE__, __LINE__)

    bool success = true;

    struct e_server server;
    server.e_listen_port = SERVER_PORT; // Not testing bad ports
    server.on_close = landmark_on_close;
    server.on_new_connection = landmark_on_new_connection;

    LANDMARK(__FILE__, __LINE__)
    intercept_set_socket(INTERCEPT_INTR, SYS_ERR, EACCES);
    E_RESULT e_result = start_e_server(&server);
    if (e_result != E_SOC_ERR)
    {
        TEST_RESULTS(
            FAIL,
            "Socket Test: Bad E_RESULT: Expected: %s, Actual: %s",
                E_RESULT_to_str(E_SOC_ERR),
                E_RESULT_to_str(e_result))

        success = false;
    }

    LANDMARK(__FILE__, __LINE__)
    intercept_set_socket(INTERCEPT_NORM, NO_ARG, NO_ARG);
    intercept_set_bind(INTERCEPT_INTR, SYS_ERR, EACCES);
    e_result = start_e_server(&server);
    if (e_result != E_SOC_ERR)
    {
        TEST_RESULTS(
            FAIL,
            "Bind Test: Bad E_RESULT: Expected: %s, Actual: %s",
                E_RESULT_to_str(E_SOC_ERR),
                E_RESULT_to_str(e_result))

        success = false;
    }

    LANDMARK(__FILE__, __LINE__)
    intercept_set_bind(INTERCEPT_NORM, NO_ARG, NO_ARG);
    intercept_set_listen(INTERCEPT_INTR, SYS_ERR, EACCES);
    e_result = start_e_server(&server);
    if (e_result != E_SOC_ERR)
    {
        TEST_RESULTS(
            FAIL,
            "Listen Test: Bad E_RESULT: Expected: %s, Actual: %s",
                E_RESULT_to_str(E_SOC_ERR),
                E_RESULT_to_str(e_result))

        success = false;
    }

    LANDMARK(__FILE__, __LINE__)
    intercept_set_listen(INTERCEPT_NORM, NO_ARG, NO_ARG);
    intercept_set_pthread_create(INTERCEPT_INTR, SYS_ERR, EAGAIN);
    e_result = start_e_server(&server);
    if (e_result != E_SYS_ERR)
    {
        TEST_RESULTS(
            FAIL,
            "Pthread Create Test: Bad E_RESULT: Expected: %s, Actual: %s",
                E_RESULT_to_str(E_SYS_ERR),
                E_RESULT_to_str(e_result))

        success = false;
    }

    LANDMARK(__FILE__, __LINE__)
    intercept_set_pthread_create(INTERCEPT_NORM, NO_ARG, NO_ARG);
    e_result = start_e_server(&server);
    if (e_result != E_SUCCESS)
    {
        TEST_RESULTS(
            FAIL,
            "Success Test: Bad E_RESULT: Expected: %s, Actual: %s",
                E_RESULT_to_str(E_SUCCESS),
                E_RESULT_to_str(e_result))

        success = false;
    }
    else
    {
        server.e_command = KILL;

        void* join_val;
        pthread_join(server.e_thread, &join_val);
    }

    if (success)
    {
        TEST_RESULTS(
            PASS,
            "%s",
            "Server socket functions verified.")
    }

    LANDMARK(__FILE__, __LINE__)
}

// Main =========================================================
int main(int argc, const char* argv[])
{
    char results_filename[RESULTS_FILENAME_LEN];

    time_t cur_time;
    time(&cur_time);
    char* start_time = ctime(&cur_time);
    sprintf(results_filename, "results-%s", start_time);

    FILE* results_file = fopen(results_filename, "w");

    bad_server_args(results_file);
    bad_server_sys_calls(results_file);

    fclose(results_file);

    return 0;
}
