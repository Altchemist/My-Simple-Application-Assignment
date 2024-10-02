#ifndef __server_h
#define __server_h
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <termio.h>
#include <termios.h>
#include <signal.h>
#include <pthread.h>
#include <regex.h>
#include <error.h>
#include <err.h>

struct threadclient{
    pthread_t threadid;
    int index;
    int serverConn;
    int clientConn;
    uint32_t address;
    u_int16_t port;
    pthread_mutex_t *file_mutex;
}threadclient;

typedef struct threadclient threadC;

ssize_t readn(int filedes, void *buff, size_t nbytes);
ssize_t writen(int filedes, const void *buff, size_t nbytes);

#define LISTENQ 1024
#define MAXLINE 4096
#define SockAddr struct sockaddr

/* Define bzero() as a macro if it's not in standard C library. */
#ifndef	HAVE_BZERO
#define	bzero(ptr,n)		memset(ptr, 0, n)
/* $$.If bzero$$ */
/* $$.If memset$$ */
#endif