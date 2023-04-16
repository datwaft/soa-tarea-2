#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "logging.h"
#include "semaphore.h"

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

  usleep(100 * 1000);

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

  {
    int64_t i = 0;
    int64_t ii = 0;
    while (i < threads_size) {
      // Add left direction thread
      data[i].id = i + 1;
      data[i].semaphore = semaphore;
      data[i].direction = DIRECTION_left;
      pthread_create(&threads[i], NULL, (void *(*)(void *))thread_function,
                     &data[i]);
      i += 1;
      // Add right direction thread
      data[i].id = i + 1;
      data[i].semaphore = semaphore;
      data[i].direction = DIRECTION_right;
      pthread_create(&threads[i], NULL, (void *(*)(void *))thread_function,
                     &data[i]);
      i += 1;

      int64_t sleep_us = powl(ii + 1, 2) * 10 * 1000;
      usleep(sleep_us);
      ii += 1;
    }
  }

  for (int64_t i = 0; i < threads_size; ++i) {
    pthread_join(threads[i], NULL);
  }

  free(semaphore);
  return EXIT_SUCCESS;
}
