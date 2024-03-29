#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "exponential-distribution.h"
#include "logging.h"
#include "semaphore.h"

typedef struct data_st {
  int64_t id;
  semaphore_t *semaphore;
  direction_t direction;
} data_t;

void thread_function(data_t *data);

typedef struct tc_data_st {
  int64_t thread_n;
  semaphore_t *semaphore;
  direction_t direction;
  double lambda;
} tc_data_t;

pthread_t *thread_creation_function(tc_data_t *data);

int main(int argc, char **argv) {
  if (argc < 3) {
    fprintf(stderr, "\x1b[31m" // red foreground color
                    "You must specify at least "
                    "\x1b[1m" // bold intensity
                    "2"
                    "\x1b[22m" // reset intensity
                    " arguments."
                    "\x1b[0m" // reset
                    "\n");
    return EXIT_FAILURE;
  }

  int64_t left_n = atoi(argv[1]);
  int64_t right_n = atoi(argv[2]);

  log_info("\x1b[33m" // yellow foreground color
           "\x1b[1m"  // bold intensity
           "%d"
           "\x1b[22m" // reset intensity
           " cars are coming from the "
           "\x1b[1m" // bold intensity
           "East"
           "\x1b[22m" // reset intensity
           " ("
           "\x1b[1m" // bold intensity
           "<-"
           "\x1b[22m" // reset intensity
           "). Using "
           "\x1b[1m" // bold intensity
           "λ"
           "\x1b[22m" // reset intensity
           " of "
           "\x1b[1m" // bold intensity
           "0.005"
           "\x1b[22m" // reset intensity
           "."
           "\x1b[0m" // reset
           "\n",
           left_n);
  log_info("\x1b[33m" // yellow foreground color
           "\x1b[1m"  // bold intensity
           "%d"
           "\x1b[22m" // reset intensity
           " cars are coming from the "
           "\x1b[1m" // bold intensity
           "West"
           "\x1b[22m" // reset intensity
           " ("
           "\x1b[1m" // bold intensity
           "->"
           "\x1b[22m" // reset intensity
           "). Using "
           "\x1b[1m" // bold intensity
           "λ"
           "\x1b[22m" // reset intensity
           " of "
           "\x1b[1m" // bold intensity
           "0.005"
           "\x1b[22m" // reset intensity
           "."
           "\x1b[0m" // reset
           "\n",
           right_n);

  semaphore_t *semaphore = malloc(sizeof(semaphore_t));
  semaphore_init(semaphore);

  pthread_t left_creation_thread;
  pthread_create(&left_creation_thread, NULL,
                 (void *(*)(void *))thread_creation_function,
                 &(tc_data_t){.semaphore = semaphore,
                              .direction = DIRECTION_left,
                              .thread_n = left_n,
                              .lambda = 0.005});

  pthread_t right_creation_thread;
  pthread_create(&right_creation_thread, NULL,
                 (void *(*)(void *))thread_creation_function,
                 &(tc_data_t){.semaphore = semaphore,
                              .direction = DIRECTION_right,
                              .thread_n = right_n,
                              .lambda = 0.005});

  pthread_t *left_threads = NULL;
  pthread_join(left_creation_thread, (void **)&left_threads);

  pthread_t *right_threads = NULL;
  pthread_join(right_creation_thread, (void **)&right_threads);

  for (int64_t i = 0; i < left_n; ++i) {
    pthread_join(left_threads[i], NULL);
  }

  for (int64_t i = 0; i < right_n; ++i) {
    pthread_join(right_threads[i], NULL);
  }

  free(left_threads);
  free(right_threads);
  free(semaphore);
  return EXIT_SUCCESS;
}

void thread_function(data_t *data) {
  semaphore_enter(data->semaphore, data->direction);

  log_info("\x1b[1;32m" // bold intensity + green foreground color
           "%02d"
           "\x1b[22;39m" // reset intensity + reset foreground color
           "("
           "\x1b[1m" // bold intensity
           "%s"
           "\x1b[22m" // reset intensity
           "): entered bridge, will stay in the bridge for "
           "\x1b[1m" // bold intensity
           "%d"
           "\x1b[22m" // reset intensity
           "ms"
           "\x1b[0m" // reset
           "\n",
           data->id, data->direction == DIRECTION_left ? "<-" : "->", 100);

  usleep(100 * 1000);

  log_info("\x1b[1;31m" // bold intensity + red foreground color
           "%02d"
           "\x1b[22;39m" // reset intensity + reset foreground color
           "("
           "\x1b[1m" // bold intensity
           "%s"
           "\x1b[22m" // reset intensity
           "): exited bridge"
           "\x1b[0m" // reset
           "\n",
           data->id, data->direction == DIRECTION_left ? "<-" : "->");

  semaphore_exit(data->semaphore);

  // NOTE: it is responsibility of the thread to free the memory for it's data
  free(data);
}

pthread_t *thread_creation_function(tc_data_t *data) {
  // NOTE: it is responsibility of the caller thread to free this memory.
  pthread_t *threads = malloc(sizeof(pthread_t) * data->thread_n);

  for (int64_t i = 0; i < data->thread_n; ++i) {
    // NOTE: it is responsibility of the thread to free the memory for it's data
    data_t *thread_data = malloc(sizeof(data_t));
    thread_data->id = i + 1;
    thread_data->semaphore = data->semaphore;
    thread_data->direction = data->direction;

    pthread_create(&threads[i], NULL, (void *(*)(void *))thread_function,
                   thread_data);

    int64_t sleep_us = exp_rand(data->lambda) * 1000;

    if (i < data->thread_n - 1) {
      log_info("\x1b[33m" // yellow foreground color
               "A new thread has been created with direction "
               "\x1b[1m" // bold intensity
               "%s"
               "\x1b[22m" // reset intensity
               ", will wait "
               "\x1b[1m" // bold intensity
               "%d"
               "\x1b[22m" // reset intensity
               "ms "
               "before creating a new thread."
               "\x1b[0m" // reset
               "\n",
               data->direction == DIRECTION_left ? "East to West (<-)"
                                                 : "West to East (->)",
               sleep_us / 1000);
      usleep(sleep_us);
    } else {
      log_info("\x1b[33m" // yellow foreground color
               "A new thread has been created with direction "
               "\x1b[1m" // bold intensity
               "%s"
               "\x1b[0m" // reset
               "\n",
               data->direction == DIRECTION_left ? "East to West (<-)"
                                                 : "West to East (->)");
    }
  }

  pthread_exit(threads);
}
