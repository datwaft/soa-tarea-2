#include "logging.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

void log_info(const char *message, ...) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  struct tm *tm = localtime(&tv.tv_sec);
  char timestamp[128 + 15 + sizeof(message) + 1];
  char milliseconds[15];
  strftime(timestamp, sizeof(timestamp),
           "\x1b[2m"
           "[%H:%M:%S",
           tm);
  sprintf(milliseconds,
          ".%03d] "
          "\x1b[22m",
          tv.tv_usec / 1000);
  strcat(timestamp, milliseconds);
  strcat(timestamp, message);

  va_list args;
  va_start(args, message);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
  vprintf(timestamp, args);
#pragma clang diagnostic pop

  va_end(args);
}
