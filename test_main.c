#include "intercept.h"
#include "coverage.h"
#include "e_socket.h"

#include <stdio.h> // printf
#include <unistd.h> // close

const char* E_RESULT_to_str(E_RESULT result)
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

void landmark_on_new_connection(struct e_handler_arg* arg)
{
    LANDMARK(__FILE__, __LINE__)
}
void landmark_on_close(struct e_handler_arg* arg)
{
    LANDMARK(__FILE__, __LINE__)
}

int main(int argc, const char* argv[])
{
    cov_filename("test_cov.txt");
    cov_start();

    LANDMARK(__FILE__, __LINE__)

    struct e_server server;
    server.e_listen_port = 3000;
    server.e_command = WORK;
    server.on_new_connection = landmark_on_new_connection;
    server.on_close = landmark_on_close;

    E_RESULT result = start_e_server(&server);
    printf("%s\n", E_RESULT_to_str(result));

    close(server.e_listen_fd);

    void* exit_val;
    int join_res = pthread_join(server.e_thread, &exit_val);
    if (join_res == -1)
        perror("Wait failed.\n");
    else
        printf("Server thread was closed.\n");


    LANDMARK(__FILE__, __LINE__)

    cov_stop();

    return 0;
}
