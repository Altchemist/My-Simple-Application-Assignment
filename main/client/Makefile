CC = gcc
PROGRAMS= client.o client

CLIENT = client 

$(CLIENT): client.o
		$(CC) client.o -o $(CLIENT)

client.o: client.c client.h
		$(CC) -c client.c -o client.o

clean:
	rm -f $(PROGRAMS)