#ifndef __client_h
#define __client_h

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define	bzero(ptr,n)		memset(ptr, 0, n)
#define SockAddr struct sockaddr

#define MAXLINE 4096

#endif