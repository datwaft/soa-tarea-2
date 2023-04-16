#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>   // Para utilizar funciones de generación de números aleatorios
#include <unistd.h>   // Para utilizar funciones de espera y tiempo
#include <math.h>     // Para utilizar la función log()
#include <string.h>

#define RESET "\x1b[0m"
#define BOLD "\x1b[1m"
#define DIM "\x1b[2m"
#define GREEN "\x1b[32m"
#define BLUE "\x1b[34m"
#define BUFFER_SIZE 256


sem_t mutex;

struct thread_args_t {
  char *direction;
  int id;
  int time_in_crit_section;
};

struct direction_args_t {
  int num_threads;
};

/**
 * La expresión return -log(1 - x) / lambda; se utiliza para aplicar la transformación necesaria en la generación de tiempos de espera con distribución exponencial.
 * En la distribución exponencial, la función de densidad de probabilidad está dada por f(x) = lambda * exp(-lambda * x), donde lambda es el valor medio (tasa media) de la 
 * distribución. Para generar números aleatorios con distribución exponencial, se puede utilizar la técnica de inversión de la función acumulativa.
 * La función acumulativa de la distribución exponencial es F(x) = 1 - exp(-lambda * x). Para invertir esta función, se puede resolver la ecuación F(x) = 1 - exp(-lambda * x) = y, 
 * donde y es un número aleatorio en el rango [0,1]. Al despejar x en esta ecuación, se obtiene x = -log(1 - y) / lambda.
 * En el código proporcionado, se utiliza drand48() para generar un número aleatorio y se le resta a 1 para obtener (1 - x), donde x es el número aleatorio. Luego, se aplica 
 * la transformación log(1 - x) / lambda para escalar el resultado y obtener el tiempo de espera con distribución exponencial.
*/
double tiempo_espera(double lambda) {
    double x = drand48(); // Generar un número aleatorio en el rango [0,1]
    return -log(1 - x) / lambda; // Aplicar la transformación para obtener el tiempo de espera exponencial
}

/**
 * En la estructura thread_args_t, se cambiaron los nombres de las variables para que fueran más descriptivos. 
 * En la función logger(), se hizo un reordenamiento de los argumentos de la función fprintf() para que estuvieran 
 * más organizados. 
 * La llamada a strcat() para crear la marca de tiempo se cambió por una llamada a sprintf() para hacer lo mismo.
*/

/* Imprime en la consola un mensaje con la información del tag y el mensaje 
 * recibidos como parámetros. Agrega un timestamp al mensaje. */
void logger(const char *tag, const char *message) {
  // Obtain timestamp
  struct timeval tv;
  gettimeofday(&tv, NULL);
  struct tm *tm = localtime(&tv.tv_sec);
  char timestamp[64];
  sprintf(timestamp, "[%02d:%02d:%02d.%06ld]", tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec);

  fprintf(stderr,
          "%s"
          " " DIM "[%s]" RESET
          " %s"
          "\n",
          timestamp, tag, message);
}

/* Procedimiento que implementa la sección crítica en un thread utilizando un 
 * semáforo para la sincronización. */
void *crit_section_thread(void *arg) {
    // Espera a que se libere el semáforo
    sem_wait(&mutex);

    // Registra en el log el ingreso del coche a la sección crítica
    char log_message[BUFFER_SIZE];
    snprintf(log_message, BUFFER_SIZE, "%s%d%s",
              "car id ", ((struct thread_args_t *)arg)->id, " entered bridge.");
    logger(((struct thread_args_t *)arg)->direction, log_message);

    // Sección crítica
    int time_in_crit_section = ((struct thread_args_t *)arg)->time_in_crit_section;
    snprintf(log_message, BUFFER_SIZE, "%s%d%s%d%s",
              "car id ",((struct thread_args_t *)arg)->id, " using bridge for ",
              time_in_crit_section, " seconds.");
    logger(((struct thread_args_t *)arg)->direction, log_message);
    sleep(time_in_crit_section);

    // Registra en el log la salida del coche de la sección crítica
    snprintf(log_message, BUFFER_SIZE, "%s%d%s",
              "car id ", ((struct thread_args_t *)arg)->id, " exited bridge.");
    logger(((struct thread_args_t *)arg)->direction, log_message);

    // Libera el semáforo
    sem_post(&mutex);

    // Libera la memoria en donde estaba la estructura de argumentos
    free(arg);
}

void *east_west_create(void *arg) {
  char direction[] = GREEN "east to west" RESET;

  int num_threads = ((struct direction_args_t *)arg)->num_threads;
  pthread_t thread_id[num_threads];

  for (int i = 0; i < num_threads; i++) {

    struct thread_args_t *thread_args =
        (struct thread_args_t *)malloc(sizeof(struct thread_args_t));

    thread_args->direction = direction;
    thread_args->id = i;
    thread_args->time_in_crit_section = 1; // (seconds) if want to have different wait time
    
    double lambda = 0.1; // Valor medio (lambda) de la distribución exponencial
    srand48(time(0)); // Inicializar la semilla para la generación de números aleatorios
    
    pthread_create(&thread_id[i], NULL, &crit_section_thread,
                   (void *)thread_args);

    usleep(tiempo_espera(lambda) * 1000000); // Esperar el tiempo de espera en microsegundos
  }

  for (int i = 0; i < num_threads; i++) {
    pthread_join(thread_id[i], NULL);
  }
}

void *west_east_create(void *arg) {
  char direction[] = BLUE "west to east" RESET;

  int num_threads = ((struct direction_args_t *)arg)->num_threads;
  pthread_t thread_id[num_threads];

  for (int i = 0; i < num_threads; i++) {

    struct thread_args_t *thread_args =
        (struct thread_args_t *)malloc(sizeof(struct thread_args_t));

    thread_args->direction = direction;
    thread_args->id = i;
    thread_args->time_in_crit_section = 2; // (seconds) if want to have different wait time
   
    double lambda = 0.1; // Valor medio (lambda) de la distribución exponencial
    srand48(time(0)); // Inicializar la semilla para la generación de números aleatorios

    pthread_create(&thread_id[i], NULL, &crit_section_thread,
                   (void *)thread_args);

    usleep(tiempo_espera(lambda) * 1000000); // Esperar el tiempo de espera en microsegundos
  }

  for (int i = 0; i < num_threads; i++) {
    pthread_join(thread_id[i], NULL);
  }
}


int main(int argc, char *argv[]) {
  pthread_t west_east, east_west;
  struct direction_args_t *west_east_args, *east_west_args;

  if (argc < 3) {
      printf("No se proporcionó un argumento válido.\n");
      // Manejar el caso cuando no se proporciona un argumento válido
  } else {
      west_east_args = (struct direction_args_t *)malloc(sizeof(struct direction_args_t));
      east_west_args = (struct direction_args_t *)malloc(sizeof(struct direction_args_t));
    
      if (west_east_args == NULL && east_west_args == NULL) {
          printf("Error en la asignación de memoria.\n");
          // Manejar el error de asignación de memoria
      } else {
          sem_init(&mutex, 0, 1);

          west_east_args->num_threads = atoi(argv[1]);
          east_west_args->num_threads = atoi(argv[2]);

          pthread_create(&west_east, NULL, &west_east_create, (void *)west_east_args);
          pthread_create(&east_west, NULL, &east_west_create, (void *)east_west_args);

          sem_destroy(&mutex);

          pthread_join(east_west, NULL);
          pthread_join(west_east, NULL);
      }
  }
  return 0;
}
