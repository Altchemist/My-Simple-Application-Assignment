CC = gcc
CFLAGS = -I../lib -g -O2 -D_REENTRANT -Wall
LIBS = ../libunp.a -lpthread
LIBS_XTI = ../libunpxti.a ../libunp.a -lpthread
RANLIB = ranlib
PROGRAMS= learn.o learn

CLIENT = learn 

$(CLIENT): learn.o
		$(CC) learn.o -o $(CLIENT)

learn.o: learning.c
		$(CC) -c learning.c -o learn.o

clean:
	rm -f $(PROGRAMS)


