/* chat.h */
#ifndef CHAT_H
#define CHAT_H
	#include <iostream>
	#include <sstream>
	#include <stdlib.h>
	#include <string>

<<<<<<< HEAD
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>

#include "list.h"
=======
	#include "list.h"
>>>>>>> d8444c6fcd9b958d023cd0af362f0a93642c462d

/* Taken from CAL-v3's chat.h */
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

typedef struct
{
	struct sockaddr_in client;	/* Estructura con la información del cliente */

	string user;			/* Nombre de usuario (para identificar los mensajes) */

	int num_hilos;			/* Número de hilos que se van a crear */

	int sock_lec;			/* Socket para la lectura */

	int sock_es;			/* Socket para la escritura */

        List <struct pollfd> clientes;	/* Lista con los clientes conectados para poll() (para el servidor) */
<<<<<<< HEAD

        List < int > sockets_es ;	/* Lista con los sockets de escritura */
} datos_hilo;



	using namespace std;


	/*******************************/
	/* Definition of custom errors */
	/*******************************/

=======

        List < int > sockets_es ;	/* Lista con los sockets de escritura */
} datos_hilo;



	using namespace std;


	/*******************************/
	/* Definition of custom errors */
	/*******************************/

>>>>>>> d8444c6fcd9b958d023cd0af362f0a93642c462d
	/* List related errors */
//	#define LIST_ERROR(x)			((x) < 0)	/* "true" if it's a list error  */
//	#define LIST_NOERROR			0		/* No errors */

	/* Errors specifiers */
//	#define LIST_ERR_INDEX		-1	/* The specified index doesn't exists on this list */
//	#define LIST_ERR_NOT_FOUND	-2	/* The searched item is not in the list */

	/* Generic errors */
//	#define UNKNOWN_ERR		-10	/* An error which cause is unknown */

#endif
