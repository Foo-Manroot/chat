CXX = g++
CXXFLAGS = -Wall -pthread -lpthread

# Default rule: builds just server and client
chat: client server

# Builds everything
<<<<<<< HEAD
all: client server

# Rules for individual files:
# ---------------------------------------------------------------

# Client executable
client: client.o
	$(CXX) $(CXXFLAGS) -o client client.o

# Server executable
server: server.o
	$(CXX) $(CXXFLAGS) -o server server.o

# Object files
client.o: chat.h list.h
main.o: chat.h list.h
server.o: chat.h list.h

# ---------------------------------------------------------------
=======
all: main client server

# Rules for individual files:
# ---------------------------------------------------------------
main: main.o
	$(CXX) $(CXXFLAGS) -o main main.o


# Client executable
client: client.o
	$(CXX) $(CXXFLAGS) -o client client.o

# Server executable
server: server.o
	$(CXX) $(CXXFLAGS) -o server server.o

# Object files
client.o: chat.h list.h
main.o: chat.h list.h
server.o: chat.h list.h
>>>>>>> d8444c6fcd9b958d023cd0af362f0a93642c462d

# ---------------------------------------------------------------

<<<<<<< HEAD
=======

>>>>>>> d8444c6fcd9b958d023cd0af362f0a93642c462d
# This rule forces the project to being cleaned
# and builds everything again
rebuild: clean all

# Removes all .o files and the executables generated
clean:
	rm -fv *.o
<<<<<<< HEAD
=======
	rm -fv main
>>>>>>> d8444c6fcd9b958d023cd0af362f0a93642c462d
	rm -fv client
	rm -fv server
