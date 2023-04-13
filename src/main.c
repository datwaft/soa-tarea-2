#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

sem_t mutex;

struct args {
  char *direction;
};

void *thread(void *arg) {
  // wait
  sem_wait(&mutex);
  printf("\nEntered..%s\n", ((struct args *)arg)->direction);

  // critical section
  sleep(2);

  // signal
  printf("\nJust Exiting...\n");
  sem_post(&mutex);
}

void *east_west_create(void *arg) {
  char direction[] = "east to west";
  struct args *thread_args = (struct args *)malloc(sizeof(struct args));
  thread_args->direction = direction;

  printf("Creating east -> west\n");
  int num_threads = 2;
  pthread_t thread_id[num_threads];
  for (int i = 0; i < num_threads; i++) {
    pthread_create(&thread_id[i], NULL, &thread, (void *)thread_args);
    // delay exponencial
  }

  for (int i = 0; i < num_threads; i++) {
    pthread_join(thread_id[i], NULL);
  }
}

void *west_east_create(void *arg) {
  char direction[] = "west to east";
  struct args *thread_args = (struct args *)malloc(sizeof(struct args));
  thread_args->direction = direction;
  printf("Creating west -> east\n");
  int num_threads = 5;
  pthread_t thread_id[num_threads];
  for (int i = 0; i < num_threads; i++) {
    pthread_create(&thread_id[i], NULL, &thread, (void *)thread_args);
    // delay exponencial
  }

  for (int i = 0; i < num_threads; i++) {
    pthread_join(thread_id[i], NULL);
  }
}

int main(int argc, char *argv[]) {
  sem_init(&mutex, 0, 1);

  // num_thread_west_to_east = 0; num_thread_east_to_west = 0;

  int num_threads = 5;
  // pthread_t thread_id[num_threads];

  pthread_t west_east, east_west;

  pthread_create(&west_east, NULL, &west_east_create, NULL);
  pthread_create(&east_west, NULL, &east_west_create, NULL);

  // for (int i = 0; i < num_threads; i++) {
  //   pthread_create(&thread_id[i], NULL, &thread, NULL);
  //   // delay exponencial
  // }

  // for (int i = 0; i < num_threads; i++) {
  //   pthread_join(thread_id[i], NULL);
  // }

  sem_destroy(&mutex);

  pthread_join(east_west, NULL);
  pthread_join(west_east, NULL);
  // printf(argv[1]);
  return 0;
}
