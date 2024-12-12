#include "util.h"

#define PRINT_BUF_SIZE 4096

void Debug_Print(char *tag, const char *stage, const char *fmt, ...)
{
    va_list ap;
    char fmt_buf[PRINT_BUF_SIZE];
    char *ptr_tmp = NULL;

    memset(fmt_buf, '\0', PRINT_BUF_SIZE);
    sprintf(fmt_buf, "[ %s %s ]\t", tag, stage);
    ptr_tmp = fmt_buf + strlen(fmt_buf);

    va_start(ap, fmt);
    vsprintf(ptr_tmp, fmt, ap);
    strcat(ptr_tmp, "\r\n");
    va_end(ap);

    printf("%s", fmt_buf);
}
