#include <pthread.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "semaphore.h"

static void print_with_timestamp(const char *message, ...);

typedef struct data_st {
  int64_t id;
  semaphore_t *semaphore;
  direction_t direction;
} data_t;

void thread_function(data_t *data) {
  semaphore_enter(data->semaphore, data->direction);
  print_with_timestamp(
      "\x1b[1;32m"
      "%02d"
      "\x1b[22;39m"
      "("
      "\x1b[1m"
      "%s"
      "\x1b[22m"
      "): entered bridge"
      "\x1b[0m"
      "\n",
      data->id, data->direction == DIRECTION_left ? "<-" : "->",
      data->semaphore->counter,
      data->semaphore->direction == DIRECTION_left ? "<-" : "->");
  nanosleep(&(struct timespec){.tv_nsec = 100 * 1000},
            &(struct timespec){.tv_nsec = 100 * 1000});
  print_with_timestamp(
      "\x1b[1;31m"
      "%02d"
      "\x1b[22;39m"
      "("
      "\x1b[1m"
      "%s"
      "\x1b[22m"
      "): exited bridge"
      "\x1b[0m"
      "\n",
      data->id, data->direction == DIRECTION_left ? "<-" : "->",
      data->semaphore->counter,
      data->semaphore->direction == DIRECTION_left ? "<-" : "->");
  semaphore_exit(data->semaphore);
}

int main(int argc, char **argv) {
  int64_t left_n = 10;
  int64_t right_n = 10;

  semaphore_t *semaphore = malloc(sizeof(semaphore_t));
  semaphore_init(semaphore);

  int64_t threads_size = left_n + right_n;
  pthread_t threads[threads_size];
  data_t data[threads_size];

  direction_t last = DIRECTION_left;
  for (int64_t i = 0; i < threads_size; ++i) {
    data[i].id = i + 1;
    data[i].semaphore = semaphore;
    data[i].direction = last;
    last = last == DIRECTION_left ? DIRECTION_right : DIRECTION_left;
  }

  for (int64_t i = 0; i < threads_size; ++i) {
    pthread_create(&threads[i], NULL, (void *(*)(void *))thread_function,
                   &data[i]);
  }

  for (int64_t i = 0; i < threads_size; ++i) {
    pthread_join(threads[i], NULL);
  }

  free(semaphore);
  return EXIT_SUCCESS;
}

static void print_with_timestamp(const char *message, ...) {
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
          ".%06d] "
          "\x1b[22m",
          tv.tv_usec);
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
