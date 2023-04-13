#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#define RESET "\x1b[0m"
#define BOLD "\x1b[1m"
#define DIM "\x1b[2m"
#define GREEN "\x1b[32m"
#define BLUE "\x1b[34m"

sem_t mutex;

struct args {
  char *direction;
  int id;
};

void logger(const char *tag, const char *message) {
  // Obtain timestamp
  struct timeval tv;
  gettimeofday(&tv, NULL);
  struct tm *tm = localtime(&tv.tv_sec);
  char timestamp[64];
  char milliseconds[8];
  strftime(timestamp, sizeof(timestamp), "%H:%M:%S", tm);
  sprintf(milliseconds, ".%06ld", tv.tv_usec);
  strcat(timestamp, milliseconds);

  fprintf(stderr,
          DIM "[%s]"
              " " RESET "%s"
              " "
              "%s"
              "\n",
          timestamp, tag, message);
}

void *thread(void *arg) {
  // wait
  sem_wait(&mutex);

  char log_message[256];
  snprintf(log_message, sizeof(log_message), "%s%d%s", "car id ",
           ((struct args *)arg)->id, " entered bridge.");
  logger(((struct args *)arg)->direction, log_message);

  // critical section
  sleep(2);

  snprintf(log_message, sizeof(log_message), "%s%d%s", "car id ",
           ((struct args *)arg)->id, " exited bridge.");
  logger(((struct args *)arg)->direction, log_message);
  sem_post(&mutex);

  // not sure
  free(arg);
}

void *east_west_create(void *arg) {
  char direction[] = GREEN "east to west" RESET;

  int num_threads = 2;
  pthread_t thread_id[num_threads];

  for (int i = 0; i < num_threads; i++) {
    struct args *thread_args = (struct args *)malloc(sizeof(struct args));
    thread_args->direction = direction;
    thread_args->id = i;
    pthread_create(&thread_id[i], NULL, &thread, (void *)thread_args);
    // delay exponencial de cada sentido
  }

  for (int i = 0; i < num_threads; i++) {
    pthread_join(thread_id[i], NULL);
  }
}

void *west_east_create(void *arg) {
  char direction[] = BLUE "west to east" RESET;

  int num_threads = 5;
  pthread_t thread_id[num_threads];

  for (int i = 0; i < num_threads; i++) {
    struct args *thread_args = (struct args *)malloc(sizeof(struct args));
    thread_args->direction = direction;
    thread_args->id = i;
    pthread_create(&thread_id[i], NULL, &thread, (void *)thread_args);
    // delay exponencial
  }

  for (int i = 0; i < num_threads; i++) {
    pthread_join(thread_id[i], NULL);
  }
}

int main(int argc, char *argv[]) {
  sem_init(&mutex, 0, 1);

  pthread_t west_east, east_west;

  pthread_create(&west_east, NULL, &west_east_create, NULL);
  pthread_create(&east_west, NULL, &east_west_create, NULL);

  sem_destroy(&mutex);

  pthread_join(east_west, NULL);
  pthread_join(west_east, NULL);
  // printf(argv[1]);

  return 0;
}
