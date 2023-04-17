#include "semaphore.h"

void semaphore_init(semaphore_t *semaphore) {
  semaphore->counter = 0;
  pthread_cond_init(&semaphore->wait, NULL);
  pthread_mutex_init(&semaphore->lock, NULL);
}

void semaphore_enter(semaphore_t *semaphore, direction_t direction) {
  pthread_mutex_lock(&semaphore->lock);
  while (semaphore->counter != 0 && semaphore->direction != direction) {
    pthread_cond_wait(&semaphore->wait, &semaphore->lock);
  }
  semaphore->counter += 1;
  semaphore->direction = direction;
  pthread_mutex_unlock(&semaphore->lock);
}

void semaphore_exit(semaphore_t *semaphore) {
  pthread_mutex_lock(&semaphore->lock);
  semaphore->counter -= 1;
  if (semaphore->counter == 0) {
    pthread_cond_broadcast(&semaphore->wait);
  }
  pthread_mutex_unlock(&semaphore->lock);
}
