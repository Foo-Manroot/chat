/* cliente.c */

#include "chat.h"

int sock_es, sock_lec;

/* Función que ejecutará el hilo encargado de leer del socket */
void *lectura_socket(void *pv)
{
	datos_hilo *datos = (datos_hilo *) pv;

	char buffer[BUFF_SIZE];
	int rval;

	memset(buffer, 0, BUFF_SIZE);

	/* Bucle para procesar la información */
	while( (rval = read(datos->sock_lec, buffer, BUFF_SIZE)) > 0)
	{
		/* Se escribe por pantalla lo leido */
		if (rval < 0)
		{
			printf("Error al leer el mensaje.\n");

			close(datos->sock_lec);
			break;
		}
		else
		{
			write(1, buffer, rval);
		}

	}

	close(datos->sock_lec);
	printf("\nDesconectado del servidor.\n");

	exit(1);
}



/* Función que ejecutará el hilo encargado de escribir en el socket */
void *escritura_socket(void *pv)
{
	datos_hilo *datos = (datos_hilo *) pv;

	char lectura[BUFF_SIZE];
	char *mensaje;
	int r_val;

	/* Una vez se ha conectado correctamente, se comienza el intercambio de información. */
        while (1)
	{
                if ((r_val = read(0, lectura, BUFF_SIZE)) < 0)
		{
                        printf("Error al leer datos del teclado. \n");

                        close(datos->sock_es);
                  	break;
                }

                /* Se reserva memoria para poder el mensaje de tal manera que aparezca como
                "nombre_cliente: mensaje" */
                mensaje = (char *)malloc(r_val + strlen(datos->user));

                /* Se copia la información en el mensaje */
	  	strcpy(mensaje, "\t\t");
                strncat(mensaje, datos->user, strlen(datos->user));
                strncat(mensaje, ": ", 2);
                strncat(mensaje, lectura, r_val);

                /* Se envía a través del socket */
                if (write(datos->sock_es, mensaje, strlen(mensaje)) < 0)
		{
                        printf("Error al escribir en el socket. \n");

                        close(datos->sock_es);
			break;
                }

                free(mensaje);
        }

	close(datos->sock_es);
	printf("\nConexión cerrada. ");

	return 0;
}



void sig_handler(int sig)
{
	/* Envía "FIN" al servidor para que cierre su descriptor */
	write(sock_es, "FIN", 4);

	/* Cierra los descriptores abiertos */
	close(sock_es);
	close(sock_lec);

	write(1, "\nConexión cerrada.\n", 20);

	exit(0);
}


int main(int argc, char *argv[])
{
	u_long addr;
	int i, opcion = 1, puerto, sock_escucha;
	unsigned int addrlen = sizeof(struct sockaddr_in);
	struct sockaddr_in server;
	struct in_addr in;
	struct hostent *host;
	char **p;
	/* Variable para los hilos (uno para leer y otro para escribir) */
	datos_hilo datos;
	datos.num_hilos = 2;
	pthread_t hilos[datos.num_hilos];

	/* Llamada correcta:
		(nombre programa) ip_servidor puerto_servidor */
	if (argc != 4)
	{
		printf("Error. Llamada correcta: \n"
			"%s ip_servidor puerto_servidor nombre_usuario \n", argv[0]);
		return -1;
	}

	/* Para manejar ctr+c */
	signal(SIGINT, sig_handler);

	/* Se crean los sockets */
	if ( ((datos.sock_es = socket(AF_INET, SOCK_STREAM, 0)) < 0) ||
	     ((sock_escucha = socket(AF_INET, SOCK_STREAM, 0)) < 0))
	{
		printf("Error al crear socket.\n");
		return -2;
	}

	sock_es = datos.sock_es; /* Se incializa la variable global para poder cerrar el socket luego */

	/*Cambia del formato notación de punto a formato binario*/
	if( (addr = inet_addr(argv[1])) == -1)
	{
		printf("La dirección IP debe estar en notación x.x.x.x \n");

		close(datos.sock_es);
		close(sock_escucha);
		return -1;
	}


	/*Obtiene una estructura hostent con la información del servidor dado en binario.*/
	host = gethostbyaddr( (char *)&addr, sizeof(addr), AF_INET);

	if(host == 0)
	{
		printf("No se pudo encontrar información sobre el equipo. \n");
		server.sin_addr.s_addr = inet_addr(argv[1]);
	}
	else
	{
		memcpy(&server.sin_addr.s_addr, host->h_addr, host->h_length);
	}

	/* Se inicializa la estructura que contiene la información del servidor */
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[2]));

	/* Se conecta al socket de lectura del servidor */
	if (connect(datos.sock_es, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		printf("Error al intentar conectarse al socket de lectura del servidor. \n");
		close(sock_escucha);
		close(datos.sock_es);
		return CAL_ERR_CON;
	}


	/* Si ha encontrado información sobre el servidor, la imprime */
	if (host != 0)
		for (p = host->h_addr_list; *p != 0; p++)
		{
			memcpy(&in.s_addr, *p, sizeof(in.s_addr));
			/*Pasa el binario de la tabla a in.s_addr porque esa estructura la necesita
			inet_ntoa, para pasarla a formato notación de punto */
			printf("Conectado con %s (%s)\n", inet_ntoa(in), host->h_name);
		}
	else
		printf("Conectado con %s (desconocido)\n", inet_ntoa(in));

	printf("\n..............\n");

	/* El cliente ya puede enviar información al servidor, pero ahora el cliente tiene que poner su socket de
	escucha para establecer la comunicación en sentido inverso */

	/* Para reutilizar el puerto inmediatamente */
	if (setsockopt(sock_escucha, SOL_SOCKET, SO_REUSEADDR, &opcion, sizeof(opcion)) < 0)
	{

                printf("Error en setsockopt");
		close(datos.sock_es);
		close(sock_escucha);
		return -2;
	}

	/* Se inicializa la estructura con la información del socket con el puerto de esucha */
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = 0; /* Se le asigna cualquier puerto libre */

	/* Se asigna la dirección al socket de escucha */
	if (bind(sock_escucha, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		printf("Error al asignar una dirección al socket de escucha. \n");

		close(datos.sock_es);
		close(sock_escucha);
		return -2;
	}

	if (getsockname(sock_escucha, (struct sockaddr *)&server, &addrlen) == -1)
	    perror("getsockame");

	puerto = ntohs(server.sin_port);

	/* Se envía al servidor el número de puerto para que pueda conectarse */
	if (write(datos.sock_es, &puerto, sizeof(int)) < 0)
	{
		printf("Error al intentar enviar la información de conexión al servidor. \n");

		close(datos.sock_es);
		close(sock_escucha);
		return -2;
	}

	/* Se muestra un mensaje para hacer saber que se ha creado el servidor */
	printf("Esperando al servidor en el puerto %i... \n", ntohs(puerto));

	/* Se escucha para poder aceptar la conexión entrante del servidor */
	listen(sock_escucha, 5);

	/* Se acepta la conexión del servidor */
	if ( (datos.sock_lec = accept(sock_escucha, (struct sockaddr *)&datos.client, &addrlen)) < 0)
	{
		printf("Error al intentar cerrar la conexión\n.");

		close(sock_escucha);
		close(sock_es);
		close(sock_lec);

		return -3;
	}

	/* ATENCIÓN:
		Hay que añadir un sondeo del socket para que, si no se ha conseguido
		conectar en menos de 'x' segundos, se desista y se informe.
	 */

	sock_lec = datos.sock_lec; /* Se incializa la variable global para poder cerrar el socket luego */

	if (datos.sock_lec < 0) {
		printf("Conexión rechazada. \n");

	} else {
		printf("\nConectado completamente al servidor. Ya puede comenzar el intercambio de información");
		printf("\n-------------------------------------------\n\n");

		/* Se inicializa el dato para identificar la procedencia los mensajes */
		datos.user = argv[3];

		/* Al detectar la conexión del servidor, se inician los hilos para leer y escribir */

		/* El hilo nº 0 lee y el hilo nº 1 escribe (cada uno en el socket correspondiente) */
		if (pthread_create(&hilos[0], 0, lectura_socket, &datos) != 0)
		{
			printf("Error al crear el hilo nº 0");
			return -4;
		}

		if (pthread_create(&hilos[1], 0, escritura_socket, &datos) != 0)
		{
			printf("Error al crear el hilo nº 1");
			return -4;
		}

		/* Se espera a que acaben los hilos */
		for(i = 0; i < datos.num_hilos; i++)
			if(pthread_join(hilos[i], 0) != 0)
			{
				printf("Error esperando al hilo %i. \n", i);
			}
	}

	close(datos.sock_es);
	close(datos.sock_lec);
	close(sock_escucha);

	return 0;
}

