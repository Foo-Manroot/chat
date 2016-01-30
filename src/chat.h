/* chat.h */

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFF_SIZE 2048 /* Tamaño del buffer para leer y escribir */

typedef struct {
	struct sockaddr_in client;	/* Estructura con la información del cliente */

	char *user;			/* Nombre de usuario (para identificar los mensajes) */

	int num_hilos;			/* Número de hilos que se van a crear */

	int sock_lec;			/* Socket para la lectura */

	int sock_es;			/* Socket para la escritura */

	struct pollfd *fds;		/* Array para poll() (para el servidor) */

	int *fds_es;			/* Array para escribir en el resto de sockets */

	int clientes;			/* Contador del número de descriptores en el array "fds" */

//	pthread_mutex mutex;		/* Semáforo para controlar el acceso a los atributos de la estructura */
} datos_hilo;
