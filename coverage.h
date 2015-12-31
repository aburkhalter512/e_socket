#ifndef COV_E_SOCKET
#define COV_E_SOCKET

void cov_filename(const char filename[]);

void cov_start();
void cov_hit(const char str[]);
void cov_stop();

#define MARK(file, line) "\tMARK\n\t\tFile: " #file "\n\t\tLine: " #line "\n"
#define LANDMARK(file, line)\
    cov_hit(MARK(file, line));

#endif // COV_E_SOCKET
