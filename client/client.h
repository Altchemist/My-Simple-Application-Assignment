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
#include <termios.h>
#include <termio.h>
#include <regex.h>

#define	bzero(ptr,n)		memset(ptr, 0, n)
#define SockAddr struct sockaddr
#define IS_TIME strcmpr(sendBuffer, "TIME")==0
#define IS_USERS strcmpr(sendBuffer, "USERS")==0
#define IS_EXIT strcmpr(sendBuffer, "EXIT")==0
#define IS_FILES match_file==0
#define IS_SUM match_sum==0

#define MAXLINE 4096

#endif