#ifndef COV_E_SOCKET
#define COV_E_SOCKET

void cov_filename(const char filename[]);

void cov_start();
void cov_hit(const char str[], int str_len);
void cov_stop();

#define MARK(file, line) "\tMark\n\t\tFile: " #file "\n\t\tLine: " #line
#define LANDMARK()\
    cov_hit(MARK(__FILE__, __LINE__), sizeof(MARK(__FILE__, __LINE__)));

#endif // COV_E_SOCKET
