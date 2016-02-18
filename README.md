# CAL

Para crear los ejecutables, ir a la carpeta src/ y ejecutar:

		make

Modo de uso (si se introducen mal los argumentos, se avisa):

		Cliente:
			./cliente ip_servidor puerto_servidor nombre_usuario
		
		Servidor:
			./servidor puerto_escucha
			
			
Errores conocidos:

		A veces el cliente se queda esperando a que el servidor termine la conexión, pero este último 
	falla y termina de ejecutarse, obligando a matar el proceso del cliente.
	
		Puede que la desconexión de los clientes no se maneje de manera adecuada (no se borran adecuadamente
	los descriptores de los clientes que se desconectan).
