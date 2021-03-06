CC = gcc
CFLAGS = -Wall

HEADERS = cache.h hashmap.h doubly.h socket.h server.h
OBJECTS = cache.o hashmap.o doubly.o socket.o server.o main.o

default: server

%.o: %.c $(HEADERS)
	gcc -c -g $< -o $@

server: $(OBJECTS)
	gcc -g -pthread $(OBJECTS) -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f server