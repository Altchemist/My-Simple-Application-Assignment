CC = gcc
CFLAGS = -I../lib -g -O2 -D_REENTRANT -Wall
LIBS = ../libunp.a -lpthread
LIBS_XTI = ../libunpxti.a ../libunp.a -lpthread
RANLIB = ranlib
PROGRAMS= server.o server 

SERVER = server

$(SERVER): server.o
		$(CC) server.o -o $(SERVER)


server.o: serv.c server.h
		$(CC) -c serv.c -o server.o

clean:
	rm -f $(PROGRAMS)


