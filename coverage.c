#include "coverage.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static void copy(char* dst, const char* src)
{
    if (!dst || !src)
        return;

    while (*src)
        *dst++ = *src++;

    *dst = '\0';
}
static int len(const char* str)
{
    if (!str)
        return 0;

    int l = 1; // Accounts for \0 char
    while (*str++)
        l++;

    return l;
}

static char* _cov_filename;
static FILE* cov_file;
static int cov_counter;

void cov_filename(const char filename[])
{
    if (_cov_filename != NULL)
        free(cov_filename);

    _cov_filename = calloc(sizeof(char), len(filename));
    copy(_cov_filename, filename);
}

void cov_start()
{
    if (cov_file)
        return;

    cov_file = fopen(_cov_filename, "a");

    const int TIME_LEN = 26;
    time_t cur_time;
    time(&cur_time);
    char* start_time = ctime(&cur_time);

    fwrite(start_time, sizeof(char), TIME_LEN, cov_file);
}
void cov_hit(const char str[], int str_len)
{
    if (!cov_file)
        return;

    fwrite(str, sizeof(char), str_len, cov_file);
}
void cov_stop()
{
    if (!cov_file)
        return;

    fclose(cov_file);
    cov_file = NULL;
}
