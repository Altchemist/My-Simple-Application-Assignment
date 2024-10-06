CC = gcc
PROGRAMS= server.o server 

SERVER = server

$(SERVER): server.o
		$(CC) server.o -o $(SERVER)


server.o: server.c server.h
		$(CC) -c server.c -o server.o -lpthread

clean:
	rm -f $(PROGRAMS)