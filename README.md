# My-Simple-Application-Assignment
This is an assignment for Advanced Computer Communication, where a client and server is implemented in C89. It consists of UNIX system calls, socket and network programming. 

# Content
/main: contains two folders; one for server codes and one for client codes.

/main/client: client source codes and builds. The log.txt for each client to store their prompts and results. 

/main/server: server source codes and builds


# How to run
The client and server each has their own makefile to compile the source codes in the respective folders.

## Client
To run the client, change into the directory of the client and run the makefile to compile the program. The client application can be run from any device, and connect to the server using the port and address number. 

### Setting up
1. cd main/client

2. Make 

3. ./client "server's ip address" "port number"

### Cleaning 
make clean


## Server
To run the server, change into directory of the server and run the makefile. 

### Setting up
1. cd main/server

2. Make

3. ./server "hosts ip address" "port number" "maximum number of clients accepted" "maximum timeout"

### Cleaning
make clean

