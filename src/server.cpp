/* servidor.c */

#include "chat.h"
#include "list.h"

/* Recorre todo el array de descriptores escribiendo el mensaje contenido en "buffer"
   en todos los sockets salvo en el que esté en la posición "excluido" */
void escribir(datos_hilo *datos, string buffer, int r_val, int excluido)
{
	int i;
	ostringstream mensaje;

	/* Si el descriptor excluido es el 0 (stdout) significa que el mensaje proviene del servidor (stdin) */
	if (excluido == 0)
	{
		mensaje << datos->user << ": " << buffer.substr (0, r_val);

		/* Se escribe en el resto de descriptores */
		for (i = 1; i < datos->clientes.num_elem (); i++)
		{
			write (datos->sockets_es.get_elem (i), mensaje.str().c_str (), mensaje.str().length ());
		}
	}
	else
	{
		/* Escribe los sockets de escritura */
		for (i = 0; i < datos->clientes.num_elem (); i++)
		{
			if (i != excluido)
			{
				write (datos->sockets_es.get_elem (i), buffer.c_str (), r_val);
			}
		}
	}
}


/* Elimina el cliente nº i de los arrays de descriptores */
void eliminar_cliente(datos_hilo *datos, int i, char* user)
{
	printf("\n----------------------------------------\n");
	printf("%s desconectado.\n", user);

	/* Se cierran los sockets */
	close(datos->clientes.get_elem (i).fd);
	close(datos->sockets_es.get_elem (i));

	/* Se eliminan los sockets */
	if (LIST_ERROR(datos->clientes.del_index (i)) ||
	    LIST_ERROR (datos->sockets_es.del_index (i)) )
		cout << "Error al eliminar el elemento de la lista.\n";

	printf("Conexión cerrada completamente.\n");
	cout << "Quedan " << (datos->clientes.num_elem () - 1) << " clientes conectados.\n";
	printf("----------------------------------------\n\n");
}


/* Manejador para el hilo encargado de sondear los descriptores */
void *polling (void *pv)
{
	datos_hilo *datos = (datos_hilo *)pv;

	int ret_val, read_val, i;
	char buffer[BUFF_SIZE];
	struct pollfd *aux;

	/* Bucle para sondear */
	while (1)
	{
		/* Se reserva memoria para el array auxiliar */
		aux = (struct pollfd *) malloc (sizeof (struct pollfd) * datos->clientes.size ());

		/* Se rellena el array auxiliar */
		for (int j = 0; j < datos->clientes.size (); j++)
			aux[j] = datos->clientes.get_elem (j);


		ret_val = poll (aux, datos->clientes.num_elem (), 0);

		switch (ret_val)
		{
			case 0:
				/* Ningún descriptor disponible */
				break;

			case -1:
				printf("Error al sondear los descriptores. \n");
				break;

			default:
				/* Recorre el array de descriptores de lectura para buscar los descriptores activos */
				for (i = 0; i < datos->clientes.num_elem (); i++)
				{
					if (aux[i].revents & POLLIN)
					{
						/* Se lee la información y se escribe en los sockets de escritura */
						read_val = read (datos->clientes.get_elem (i).fd, buffer, BUFF_SIZE);

						/* Si lo recibido por el cliente es "FIN" (sin "\n"), se cierra ese socket */
						if (strcmp(buffer, "FIN") == 0)
						{
							eliminar_cliente(datos, i, "Cliente");
						}
						else
						{
							escribir(datos, buffer, read_val, i);
						}
					}
				}
		}

		/* Duerme 1 segundo antes de volver a sondear para evitar la sobrecarga */
		sleep (1);
		free (aux);
	}

	return 0;
}



int main(int argc, char *argv[])
{
	int sock_escucha, puerto_cli, i, c,
	    opcion = 1;
	unsigned int addrlen = sizeof(struct sockaddr_in);
	struct sockaddr_in server;
	struct pollfd aux;
	char *user = "\t\tServidor";
	/* Variable para los hilos (uno para leer y otro para escribir) */
	datos_hilo datos;
	datos.num_hilos = 1;
	pthread_t hilos[datos.num_hilos];

	/* Inicializa la lista */
	datos.clientes = List <struct pollfd> ();

	/* El "cliente" nº 1 es stdout para escritura y stdin para lectura */
	aux.fd = 0;			/* Inicializa los datos a una estructura auxiliar y añade la información a la lista */
	aux.events = POLLIN;
	datos.clientes.add (aux);

	datos.sockets_es.add (1);


	/* Llamada correcta:
		(nombre programa) nº_puerto */
	if (argc != 2)
	{
		printf("Error. Llamada correcta: \n"
			"%s nº_puerto \n", argv[0]);
		return -1;
	}

	/* Se crea el socket de escucha */
	if ((sock_escucha = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Error al crear socket.\n");
		return -2;
	}

	/* Para reutilizar el puerto inmediatamente */
	if (setsockopt(sock_escucha, SOL_SOCKET, SO_REUSEADDR, &opcion, sizeof(opcion)) < 0)
	{
                printf("Error en setsockopt");

		close(sock_escucha);
		return -2;
	}

	/* Se inicializa la estructura con la información del servidor */
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(atoi(argv[1]));

	/* Se asigna la dirección al socket de lectura */
	if (bind(sock_escucha, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		printf("Error al asignar una dirección al socket de lectura. \n");

		close(sock_escucha);
		return -2;
	}

	/* Se muestra un mensaje para hacer saber que se ha creado el servidor */
	printf("Servidor creado con éxito en el puerto %i. \n", ntohs(server.sin_port));

	/* Se inicializa el dato para identificar la procedencia los mensajes */
	datos.user = user;

	/* Se escucha en el socket de lectura */
	listen(sock_escucha, 5);

	/* Bucle para esperar a las conexiones */
	while(1)
	{
		/* Se acepta la conexión del cliente */
		datos.sock_lec = accept(sock_escucha, (struct sockaddr *)&datos.client, &addrlen);

		if (datos.sock_lec < 0)
		{
			perror("Conexión rechazada. \n");

			close(sock_escucha);
			close(datos.sock_es);
			close(datos.sock_lec);
			return -2;
		}

		/* Una vez aceptada la conexión, el servidor se conecta al socket de lectura del cliente */
		printf("\n\n++++++++++++++++++++++++++++++++++++++++\n");
		printf("Cliente conectado. \n");
		printf("Hay %i clientes conectados.\n", datos.clientes.num_elem ());

		/* Se crea el socket de escritura */
		if ((datos.sock_es = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			printf("Error al crear socket.\n");
			return -2;
		}

		/* El cliente envía el número de puerto al que se debe conectar el servidor */
		for(i = 0;
		i < sizeof(int) && (c = read(datos.sock_lec, i+((char *)&puerto_cli), sizeof(int)-i)) > 0;
		i += c);

		/* Se inicializa la estructura que contiene la información del cliente
		(que actúa como servidor en este sentido de la comunicación) */
		server.sin_addr.s_addr = datos.client.sin_addr.s_addr;
		server.sin_family = AF_INET;
		server.sin_port = ntohs(puerto_cli);

		/* Se conecta al socket de lectura del servidor */
		if (connect(datos.sock_es, (struct sockaddr *)&server, sizeof(server)) < 0)
		{
			printf("Error al intentar conectarse al socket de lectura del cliente. \n");

			close(datos.sock_lec);
			close(datos.sock_es);
			return -2;
		}
		else
		{
			printf("Conectado al cliente. Ya puede empezar a intercambiar mensajes. \n");
			printf("++++++++++++++++++++++++++++++++++++++++\n\n");
		}

		/* Se añaden los descriptores al array para poll() */
		aux.fd = datos.sock_lec;
		aux.events = POLLIN;

		datos.clientes.add (aux);

		datos.sockets_es.add (datos.sock_es);


		/* Sólo se crea un hilo cuando se conecta el primer cliente (para evitar que
		se cree un hilo por cliente, sobrecargando el sistema) */
		if (datos.clientes.size () == 2) /* 2 "clientes": stdout y el cliente */
			/* se crea el hilo encargado de sondear los descriptores */
			if (pthread_create(&hilos[0], 0, polling, &datos) != 0)
			{
				printf("Error al crear el hilo nº 0");
				return -4;
			}

	}

	close(datos.sock_es);
	close(datos.sock_lec);
	close(sock_escucha);

	return 0;
}
