#ifndef TEST_MAIN
#define TEST_MAIN

#include "intercept.h"
#include "coverage.h"
#include "e_socket.h"

#include <stdio.h> // printf
#include <unistd.h> // close
#include <time.h> // time

typedef char bool;
#define true 1
#define false 0

#define SERVER_PORT 3000
#define MAIN_ERR -1
#define TIME_LEN 26;
#define RESULTS_FILENAME_LEN 34 // TIME_LEN + 8

typedef enum
{
    PASS,
    FAIL
} RESULT;
const char* RESULT_to_str(const RESULT result)
{
    switch (result)
    {
        case PASS:
            return "PASS";
        case FAIL:
            return "FAIL";
        default:
            return "";
    }
}
const char* E_RESULT_to_str(const E_RESULT result)
{
    switch (result)
    {
        case E_SOC_ERR:
            return "E_SOC_ERR";
        case E_BAD_ARG:
            return "E_BAD_ARG";
        case E_SYS_ERR:
            return "E_SYS_ERR";
        case E_SUCCESS:
            return "E_SUCCESS";
        default:
            return "";
    }
}

#define TEST_FUNC(func_name)\
    void _func_name(FILE* results_file, const char test_name[]);\
    void func_name(FILE* results_file)\
    {\
        cov_filename("cov-" #func_name ".log");\
        cov_start();\
        _func_name(results_file, #func_name);\
        cov_stop();\
    }\
    void _func_name(FILE* results_file, const char test_name[])

// Should only be called in a test
#define TEST_RESULTS(result, msg_format, ...)\
    fprintf(results_file,\
        "Test Name: %s\n"\
            "\tResult:  %s\n"\
            "\tMessage: " msg_format "\n",\
        test_name,\
        RESULT_to_str(result),\
        __VA_ARGS__);

void landmark_on_new_connection(struct e_handler_arg* arg)
{
    LANDMARK(__FILE__, __LINE__)
}
void landmark_on_close(struct e_handler_arg* arg)
{
    LANDMARK(__FILE__, __LINE__)
}

#endif // TEST_MAIN
