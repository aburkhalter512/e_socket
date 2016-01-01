#include "test_main.h"

// Tests ===========================================================
TEST_FUNC(bad_server_args)
{
    LANDMARK(__FILE__, __LINE__)

    bool success = true;

    // NULL Arg test
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
            "Server Arguments Verified.")
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

    fclose(results_file);

    return 0;
}
