CXX = g++
CXXFLAGS = -Wall -pthread -lpthread

# Default rule: builds just server and client
chat: client server

# Builds everything
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
server.o: chat.h list.h

# ---------------------------------------------------------------


# This rule forces the project to being cleaned
# and builds everything again
rebuild: clean all

# Removes all .o files and the executables generated
clean:
	rm -fv *.o
	rm -fv client
	rm -fv server
