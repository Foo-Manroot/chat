/* servidor.c */

#include "chat.h"
#include "list.h"

/* Returns current date/time, with format dd-MM-YYYY.HH:mm:ss */
const string get_date ()
{
	time_t now = time(0);
	struct tm  tstruct;
	char buf[80];
	tstruct = *localtime (&now);

	/* Gives the apropiate format to the string */
	strftime (buf, sizeof (buf), "%d-%m-%Y.%X", &tstruct);

	return buf;
}


/* Prints a string on the screen and adds it to a text file */
void log (string text)
{
	ofstream output;

	/* Opens the log file and saves the text */
	output.open ("server.log", ofstream::app | ofstream::out);

	output << "\n--------------------------------------";
	output << "\n" << get_date () << "\n";
	output << text;
	output << "\n";

	output.close ();

	/* Prints the text on the screen (stdout) */
	cout << text;
}

/* Recorre todo el array de descriptores escribiendo el mensaje contenido en "buffer"
   en todos los sockets salvo en el que esté en la posición "excluido" */
void escribir (datos_hilo *datos, string buffer, int r_val, int excluido)
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
void eliminar_cliente (datos_hilo *datos, int i, string user)
{
	ostringstream text;

	text << "\n----------------------------------------\n";
	text << user << " desconectado.\n";

	/* Se cierran los sockets */
	close(datos->clientes.get_elem (i).fd);
	close(datos->sockets_es.get_elem (i));

	/* Se eliminan los sockets */
	if (LIST_ERROR(datos->clientes.del_index (i)) ||
	    LIST_ERROR (datos->sockets_es.del_index (i)) )
		text << "Error al eliminar el elemento de la lista.\n";

	text << "Conexión cerrada completamente.\n";
	text << "Quedan " << (datos->clientes.num_elem () - 1) << " clientes conectados.\n";
	text << "----------------------------------------\n\n";

	log (text.str ());
}


/* Manejador para el hilo encargado de sondear los descriptores */
void *polling (void *pv)
{
	ostringstream log_text;
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
				/* Vacía el contenido de la cadena */
				log_text.str ("");
				log_text.clear ();

				log_text << "Error al sondear los descriptores. \n";
				log (log_text.str ());
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
	ostringstream log_text;
	int sock_escucha,
	    puerto_cli,
	    c,
	    opcion = 1;
	unsigned int addrlen = sizeof(struct sockaddr_in),
		     i;

	struct sockaddr_in server;
	struct pollfd aux;
	string user = "\t\tServidor";

	/* Variable para los hilos (uno para leer y otro para escribir) */
	datos_hilo datos;
	datos.num_hilos = 1;
	pthread_t hilos[datos.num_hilos];
	/* Fin de la declaración de variables */

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
		log_text << "Error. Llamada correcta: \n"
			<< argv[0] << " nº puerto \n";
		log (log_text.str ());

		return -1;
	}

	/* Se crea el socket de escucha */
	if ((sock_escucha = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		log_text << "Error al crear el socket. \n";
		log (log_text.str ());

		return -2;
	}

	/* Para reutilizar el puerto inmediatamente */
	if (setsockopt(sock_escucha, SOL_SOCKET, SO_REUSEADDR, &opcion, sizeof(opcion)) < 0)
	{
		log_text << "Error en setsockopt (). \n";
		log (log_text.str ());

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
		log_text << "Error al asignar una dirección al socket de lectura. \n";
		log (log_text.str ());

		close(sock_escucha);
		return -2;
	}

	/* Vacía el contenido de la cadena */
	log_text.str ("");
	log_text.clear ();

	/* Se muestra un mensaje para hacer saber que se ha creado el servidor */
	log_text << "Servidor creado con éxito en el puerto " << ntohs (server.sin_port)  <<"\n";
	log (log_text.str ());

	/* Se inicializa el dato para identificar la procedencia los mensajes */
	datos.user = user;

	/* Se escucha en el socket de lectura */
	listen(sock_escucha, 5);

	/* Bucle para esperar a las conexiones */
	while(1)
	{
		/* Vacía el contenido de la cadena */
		log_text.str ("");
		log_text.clear ();

		/* Se acepta la conexión del cliente */
		datos.sock_lec = accept(sock_escucha, (struct sockaddr *)&datos.client, &addrlen);

		if (datos.sock_lec < 0)
		{
			log_text << "Conexión rechazada. \n";
			log (log_text.str ());

			close(datos.sock_lec);
			continue;
		}

		/* Una vez aceptada la conexión, el servidor se conecta al socket de lectura del cliente */
		log_text << "\n\n++++++++++++++++++++++++++++++++++++++++\n";
		log_text << "Cliente conectado. \n";
		log_text << "Hay " << datos.clientes.size () << " clientes conectados.\n";

		log (log_text.str ());

		/* Se crea el socket de escritura */
		if ((datos.sock_es = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			/* Vacía el contenido de la cadena */
			log_text.str ("");
			log_text.clear ();

			log_text << "Error al crear socket de escritura. \n";

			log (log_text.str ());

			close (datos.sock_es);
			close (datos.sock_lec);
			continue;
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
			/* Vacía el contenido de la cadena */
			log_text.str ("");
			log_text.clear ();

			log_text << "Error al intentar conectarse al socket de lectura del cliente. \n";

			log (log_text.str ());

			close(datos.sock_lec);
			close(datos.sock_es);
			continue;
		}
		else
		{
			/* Vacía el contenido de la cadena */
			log_text.str ("");
			log_text.clear ();

			log_text << "Conectado al cliente. Ya se puede empezar a intercambiar mensajes. \n";
			log_text << "++++++++++++++++++++++++++++++++++++++++\n\n";

			log (log_text.str ());
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
				/* Vacía el contenido de la cadena */
				log_text.str ("");
				log_text.clear ();

				log_text << "Error al crear el hilo para sondear los descriptores de los clientes.\n";

				log (log_text.str ());
				return -4;
			}

	}

	close(datos.sock_es);
	close(datos.sock_lec);
	close(sock_escucha);

	return 0;
}
