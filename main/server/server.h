#ifndef server_h
#define server_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/time.h> /* For struct timeval */
#include <sys/select.h> /* For fd_set */
#include <errno.h>

#define MAX_BUFFER 1024
#define MAX_CLIENTS 10
#define MAX_TIME 120

typedef struct client_info {
    int socket;
    struct sockaddr_in address;
} 
client_details;

typedef struct thread_args {
    client_details *client;
    int max_time;
} 
thread_arguments;

#endif