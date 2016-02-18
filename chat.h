/* chat.h */
#ifndef CHAT_H
#define CHAT_H

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


/*
 * error return values
 * all the queue manipulation functions return these;
 * you can interpret them yourself, or print the error
 * message in qe_errbuf, which describes these codes
 */
#define CAL_ERROR(x)			((x) < 0)	/* Es 1 si se ha producido algún error y 0 si no. */
#define CAL_NOERR		 	0		/* Sin errores */

/* Definiciones de errores específicos de esta aplicación */
#define CAL_ERR_CON	-1	/* Error al conectar */
#define CAL_ERR_INI	-2	/* Error al crear e inicializar estructuras */

#endif
