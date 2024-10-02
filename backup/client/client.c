#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_BUFFER 1024

void log_message(FILE *log_file, const char *type, const char *message) {
    fprintf(log_file, "%s: %s\n", type, message);
    fflush(log_file);
}

int main(int argc, char *argv[]) {
    int client_socket, port;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFFER];
    FILE *log_file;
    
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
        exit(1);
    }
    
    port = atoi(argv[2]);
    
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket");
        exit(1);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(port);
    
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server");
        exit(1);
    }
    
    printf("Connected to %s:%d\n", argv[1], port);
    
    log_file = fopen("log.txt", "a");
    if (log_file == NULL) {
        perror("Error opening log file");
        exit(1);
    }
    
    while (1) {
        printf("Client> ");
        if (fgets(buffer, MAX_BUFFER, stdin) == NULL) {
            break;
        }
        
        /* Remove newline character */
        buffer[strcspn(buffer, "\n")] = '\0';
        
        if (strlen(buffer) == 0) {
            continue;
        }
        
        log_message(log_file, "Request", buffer);
        
        if (send(client_socket, buffer, strlen(buffer), 0) < 0) {
            perror("Error sending data");
            break;
        }
        
        int bytes_received = recv(client_socket, buffer, MAX_BUFFER - 1, 0);
        if (bytes_received < 0) {
            perror("Error receiving data");
            break;
        } else if (bytes_received == 0) {
            printf("Server closed the connection\n");
            break;
        }
        
        buffer[bytes_received] = '\0';
        printf("%s\n", buffer);
        
        log_message(log_file, "Response", buffer);
        
        if (strcmp(buffer, "Thank you for using MSA. Goodbye!") == 0) {
            break;
        }
    }
    
    fclose(log_file);
    close(client_socket);
    return 0;
}