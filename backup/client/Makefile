CC = gcc
CFLAGS = -I../lib -g -O2 -D_REENTRANT -Wall
LIBS = ../libunp.a -lpthread
LIBS_XTI = ../libunpxti.a ../libunp.a -lpthread
RANLIB = ranlib
PROGRAMS= client.o client

CLIENT = client 

$(CLIENT): client.o
		$(CC) client.o -o $(CLIENT)

client.o: client.c client.h
		$(CC) -c client.c -o client.o

clean:
	rm -f $(PROGRAMS)


