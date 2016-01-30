GCC = gcc -g -Wall
THR = gcc -g -Wall -pthread -lpthread
OBJ = gcc -g -c -Wall

.PHONY: clean all

all: servidor cliente

servidor: servidor.o
	$(THR) servidor.o -o servidor

servidor.o: servidor.c
	$(OBJ) servidor.c -o servidor.o


cliente: cliente.o
	$(THR) cliente.o -o cliente

cliente.o: cliente.c
	$(OBJ) cliente.c -o cliente.o


clean:
	rm -f *.o
	rm -f servidor
	rm -f cliente
