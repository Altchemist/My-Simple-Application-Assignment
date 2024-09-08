#ifndef __server_h
#define __server_h

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#define LISTENQ 1024
#define MAXLINE 4096
#define SockAddr struct sockaddr

/* Define bzero() as a macro if it's not in standard C library. */
#ifndef	HAVE_BZERO
#define	bzero(ptr,n)		memset(ptr, 0, n)
/* $$.If bzero$$ */
/* $$.If memset$$ */
#endif

#endif