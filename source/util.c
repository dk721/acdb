#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#include "util.h"

void
init_locale(void)
{
    setlocale(LC_ALL, "");
    bindtextdomain(PROGRAM, "/usr/local/share/locale");
    textdomain(PROGRAM);
}

void
loglvl(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char time_buf[20];
    char message[512];
    time_t now = time(NULL);
    
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", localtime(&now));

    int len = snprintf(message, sizeof(message), "[%s]: ", time_buf);
    vsnprintf(message + len, sizeof(message) - len, fmt, args);

    FILE *file = fopen("sqlnet.log", "a");
    if (file != NULL) {
        fprintf(file, "%s\n", message);
        fclose(file);
    }

    va_end(args);
}