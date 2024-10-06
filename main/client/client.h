#ifndef client_h
#define client_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_BUFFER 1024

#define RESPONSE_IS_EXIT strcmp(buffer, "Thank you for using MSA. Goodbye!") == 0
#define RESPONSE_IS_TIMEOUT strcmp(buffer, "Timeout. Closing connection.") == 0
#endif