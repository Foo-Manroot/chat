/* cliente.c */

#include "chat.h"

int sock_es,
    sock_lec;

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
        output.open ("client.log", ofstream::app | ofstream::out);

        output << "\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-";
        output << "\n" << get_date () << "\n";
        output << text;
        output << "\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-";

        output.close ();

        /* Prints the text on the screen (stdout) */
	cout << text;
}


/* Función que ejecutará el hilo encargado de leer del socket */
void *lectura_socket (void *pv)
{
	ostringstream log_text;
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
			/* Vacía el contenido de las cadenas */
			log_text.str ("");
			log_text.clear ();

			log_text << "Error al leer el mensaje.\n";
			log (log_text.str ());

			close(datos->sock_lec);
			break;
		}
		else
		{
			write(1, buffer, rval);
		}

	}

	/* Vacía el contenido de las cadenas */
	log_text.str ("");
	log_text.clear ();

	close(datos->sock_lec);
	log_text << "\nDesconectado del servidor.\n";
	log (log_text.str ());

	exit(1);
}



/* Función que ejecutará el hilo encargado de escribir en el socket */
void *escritura_socket(void *pv)
{
	datos_hilo *datos = (datos_hilo *) pv;

	char lectura[BUFF_SIZE];
	string aux;
	ostringstream mensaje,
		      log_text;
	int r_val;

	/* Una vez se ha conectado correctamente, se comienza el intercambio de información. */
        while (1)
	{
		/* Vacía el contenido de las cadenas */
		mensaje.str ("");
		mensaje.clear ();

		log_text.str ("");
		log_text.clear ();

                if ((r_val = read(0, lectura, BUFF_SIZE)) < 0)
		{
                        log_text << "Error al leer datos del teclado. \n";
			log (log_text.str ());

                        close(datos->sock_es);
                  	break;
                }

                /* Se copia la información en el mensaje */
		aux.assign (lectura, r_val);
		mensaje << "\t\t" << datos->user << ": " << aux;

                /* Se envía a través del socket */
                if (write(datos->sock_es, mensaje.str().c_str (), mensaje.str().size ()) < 0)
		{
                        log_text << "Error al escribir en el socket. \n";
			log (log_text.str ());

                        close(datos->sock_es);
			break;
                }
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
	ostringstream log_text;
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

	/* Fin de la declaración de variables */


	/* Llamada correcta:
		(nombre programa) ip_servidor puerto_servidor */
	if (argc != 4)
	{
		log_text <<"Error. Llamada correcta: \n"
			 << argv[0] << " ip_servidor puerto_servidor nombre_usuario \n";
		log (log_text.str ());

		return -1;
	}

	/* Para manejar ctr+c */
	signal(SIGINT, sig_handler);

	/* Se crean los sockets */
	if ( ((datos.sock_es = socket(AF_INET, SOCK_STREAM, 0)) < 0) ||
	     ((sock_escucha = socket(AF_INET, SOCK_STREAM, 0)) < 0))
	{
		log_text << "Error al crear alguno de los sockets.\n";
		log (log_text.str ());
		return -2;
	}

	sock_es = datos.sock_es; /* Se incializa la variable global para poder cerrar el socket luego */

	/*Cambia del formato notación de punto a formato binario*/
	if( (addr = inet_addr(argv[1])) == INADDR_NONE)
	{
		log_text << "La dirección IP " << argv[1] << " no es válida. \n"
			 << "Debe estar en notación x.x.x.x \n";

		log (log_text.str ());

		close(datos.sock_es);
		close(sock_escucha);
		return -1;
	}


	/*Obtiene una estructura hostent con la información del servidor dado en binario.*/
	host = gethostbyaddr( (char *)&addr, sizeof(addr), AF_INET);

	/* Vacía el contenido de la cadena */
	log_text.str ("");
	log_text.clear ();

	if(host == 0)
	{
		log_text << "No se pudo encontrar información sobre el equipo. \n";
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
		log_text << "Error al intentar conectarse al socket de lectura del servidor. \n";

		log (log_text.str ());

		close(sock_escucha);
		close(datos.sock_es);
		return -2;
	}


	/* Si ha encontrado información sobre el servidor, la imprime */
	if (host != 0)
		for (p = host->h_addr_list; *p != 0; p++)
		{
			memcpy(&in.s_addr, *p, sizeof(in.s_addr));
			/*Pasa el binario de la tabla a in.s_addr porque esa estructura la necesita
			inet_ntoa, para pasarla a formato notación de punto */
			log_text << "Conectado con " << inet_ntoa (in) << " (" << host->h_name << ")\n ";
		}
	else
		log_text << "Conectado con " << inet_ntoa (in) << " (desconocido)\n";

	log_text << "\n.................\n";

	log (log_text.str ());

	/* Vacía el contenido de la cadena */
	log_text.str ("");
	log_text.clear ();

	/* El cliente ya puede enviar información al servidor, pero ahora el cliente tiene que poner su socket de
	escucha para establecer la comunicación en sentido inverso */

	/* Para reutilizar el puerto inmediatamente */
	if (setsockopt(sock_escucha, SOL_SOCKET, SO_REUSEADDR, &opcion, sizeof(opcion)) < 0)
	{

                log_text << "Error en setsockopt";
		log (log_text.str ());

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
		log_text << "Error al asignar una dirección al socket de escucha. \n";
		log (log_text.str ());

		close(datos.sock_es);
		close(sock_escucha);
		return -2;
	}

	if (getsockname(sock_escucha, (struct sockaddr *)&server, &addrlen) == -1)
	{
		log_text << "Error en getsockame ().\n";
		log (log_text.str ());

		/* Vacía el contenido de la cadena */
		log_text.str ("");
		log_text.clear ();
	}

	puerto = ntohs(server.sin_port);

	/* Se envía al servidor el número de puerto para que pueda conectarse */
	if (write(datos.sock_es, &puerto, sizeof(int)) < 0)
	{
		log_text << "Error al intentar enviar la información de conexión al servidor. \n";

		log (log_text.str ());

		close(datos.sock_es);
		close(sock_escucha);
		return -2;
	}

	/* Se muestra un mensaje para hacer saber que se ha creado el servidor */
	log_text << "Esperando al servidor en el puerto " << ntohs(puerto) << "...\n";
	log (log_text.str ());

	/* Vacía el contenido de la cadena */
	log_text.str ("");
	log_text.clear ();

	/* Se escucha para poder aceptar la conexión entrante del servidor */
	listen(sock_escucha, 5);

	/* Se acepta la conexión del servidor */
	datos.sock_lec = accept(sock_escucha, (struct sockaddr *)&datos.client, &addrlen);

	sock_lec = datos.sock_lec; /* Se incializa la variable global para poder cerrar el socket luego */

	if (datos.sock_lec < 0)
	{
		log_text << "Conexión rechazada. \n";
		log (log_text.str ());
	}
	else
	{
		log_text << "\nConectado completamente al servidor. Ya puede comenzar el intercambio de información";
		log_text << "\n-------------------------------------------\n\n";
		log (log_text.str ());

		/* Vacía el contenido de la cadena */
		log_text.str ("");
		log_text.clear ();

		/* Se inicializa el dato para identificar la procedencia los mensajes */
		datos.user = argv[3];

		/* Al detectar la conexión del servidor, se inician los hilos para leer y escribir */

		/* El hilo nº 0 lee y el hilo nº 1 escribe (cada uno en el socket correspondiente) */
		if (pthread_create(&hilos[0], 0, lectura_socket, &datos) != 0)
		{
			log_text << "Error al crear el hilo nº 0 (encargado de la lectura) \n";
			log (log_text.str ());

			return -4;
		}

		if (pthread_create(&hilos[1], 0, escritura_socket, &datos) != 0)
		{
			log_text << "Error al crear el hilo nº 1 (encargado de la escritura) \n";
			log (log_text.str ());

			return -4;
		}

		/* Se espera a que acaben los hilos */
		for(i = 0; i < datos.num_hilos; i++)
			if(pthread_join(hilos[i], 0) != 0)
			{
				/* Vacía el contenido de la cadena */
				log_text.str ("");
				log_text.clear ();

				log_text << "Error esperando al hilo " << i << ". \n";
				log (log_text.str ());
			}
	}

	close(datos.sock_es);
	close(datos.sock_lec);
	close(sock_escucha);

	return 0;
}

