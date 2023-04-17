#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <pthread.h>
#include <stdint.h>

typedef enum DIRECTION { DIRECTION_right, DIRECTION_left } direction_t;

typedef struct semaphore_st {
  pthread_mutex_t lock;
  pthread_cond_t wait;
  int64_t counter;
  direction_t direction;
} semaphore_t;

void semaphore_init(semaphore_t *semaphore);

void semaphore_enter(semaphore_t *semaphore, direction_t direction);

void semaphore_exit(semaphore_t *semaphore);

#endif // !SEMAPHORE_H
