#include "server.h"

client_details clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void update_users_file() {
    FILE *file;
    int i;
    struct sockaddr_in peer_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    char ip_str[INET_ADDRSTRLEN];
    
    file = fopen("users", "w");
    if (file == NULL) {
        perror("Error opening users file");
        return;
    }
    
    for (i = 0; i < client_count; i++) {
        if (getpeername(clients[i].socket, (struct sockaddr*)&peer_addr, &addr_len) == 0) {
            inet_ntop(AF_INET, &(peer_addr.sin_addr), ip_str, INET_ADDRSTRLEN);
            fprintf(file, "%s:%d\n", ip_str, ntohs(peer_addr.sin_port));
        } else {
            perror("Error getting peer name");
            fprintf(file, "Unknown:0\n");
        }
    }
    
    fclose(file);
}

void add_client(client_details *client) {
    pthread_mutex_lock(&clients_mutex);
    
    if (client_count < MAX_CLIENTS) {
        clients[client_count] = *client;
        client_count++;
        update_users_file();
    }
    
    pthread_mutex_unlock(&clients_mutex);
}

void remove_client(client_details *client) {
    int i;
    pthread_mutex_lock(&clients_mutex);
    
    for (i = 0; i < client_count; i++)
     {
        if (clients[i].socket == client->socket) 
        {
            while (i < client_count - 1) 
            {
                clients[i] = clients[i + 1];
                i++;
            }
            client_count--;
            update_users_file();
            break;
        }
    }
    
    pthread_mutex_unlock(&clients_mutex);
}

void get_users(char *buffer) {
    int i;
    char temp[MAX_BUFFER];
    
    pthread_mutex_lock(&clients_mutex);
    
    buffer[0] = '\0';
    for (i = 0; i < client_count; i++) 
    {
        sprintf(temp, "%s:%d\n", inet_ntoa(clients[i].address.sin_addr), ntohs(clients[i].address.sin_port));
        strcat(buffer, temp);
    }
    
    pthread_mutex_unlock(&clients_mutex);
}

void process_command(char *command, char *response) {
    if (strncmp(command, "SUM(", 4) == 0 && command[strlen(command) - 1] == ')') 
    {
        int x, y;
        if (sscanf(command, "SUM(%d,%d)", &x, &y) == 2) 
        {
            sprintf(response, "The result is: %d", x + y);
        } 
        else 
        {
            strcpy(response, "Invalid SUM command");
        }
    } 
    else if (strcmp(command, "TIME") == 0) 
    {
        time_t now = time(NULL);
        strftime(response, MAX_BUFFER, "%Y-%m-%d %H:%M:%S", localtime(&now));
    } 
    else if (strcmp(command, "USERS") == 0) 
    {
        get_users(response);
    } 
    else if (strncmp(command, "FILE ", 5) == 0) 
    {
        FILE *file;
        char filename[MAX_BUFFER];
        
        sscanf(command, "FILE %s", filename);
        file = fopen(filename, "r");
        if (file != NULL) 
        {
            size_t bytes_read = fread(response, 1, MAX_BUFFER - 1, file);
            response[bytes_read] = '\0';
            fclose(file);
        }
        else
        {
            sprintf(response, "Error: File '%s' not found", filename);
        }
    } 
    else if (strcmp(command, "EXIT") == 0) 
    {
        strcpy(response, "Thank you for using MSA. Goodbye!");
    } 
    else {
        strcpy(response, "Invalid command");
    }
}

void *handle_client(void *arg) 
{
    thread_arguments *args = (thread_arguments *)arg;
    client_details *client = args->client;
    int max_time = args->max_time;
    char buffer[MAX_BUFFER], response[MAX_BUFFER];
    int bytes_received;
    time_t last_activity;
    
    free(arg);
    
    add_client(client);
    printf("New connection from %s:%d\n", inet_ntoa(client->address.sin_addr), ntohs(client->address.sin_port));
    
    last_activity = time(NULL);
    
    while (1) 
    {
        fd_set read_fds;
        struct timeval tv;
        int activity;
        
        FD_ZERO(&read_fds);
        FD_SET(client->socket, &read_fds);
        
        tv.tv_sec = max_time - (time(NULL) - last_activity);
        tv.tv_usec = 0;
        
        activity = select(client->socket + 1, &read_fds, NULL, NULL, &tv);
        
        if (activity < 0) 
        {
            perror("select error");
            break;
        } 
        else if (activity == 0) 
        {
            strcpy(response, "Timeout. Closing connection.");
            send(client->socket, response, strlen(response), 0);
            break;
        }
        
        bytes_received = recv(client->socket, buffer, MAX_BUFFER - 1, 0);
        if (bytes_received <= 0) 
        {
            if (bytes_received < 0) 
            {
                perror("recv failed");
            }
            break;
        }
        
        buffer[bytes_received] = '\0';
        last_activity = time(NULL);
        
        process_command(buffer, response);
        send(client->socket, response, strlen(response), 0);
        
        if (strcmp(buffer, "EXIT") == 0)
        {
            break;
        }
    }
    
    remove_client(client);
    close(client->socket);
    printf("Connection from %s:%d closed\n", inet_ntoa(client->address.sin_addr), ntohs(client->address.sin_port));
    
    return NULL;
}

int main(int argc, char *argv[]) 
{
    int server_socket, server_port, max_clients, max_time;
    struct sockaddr_in server_address;
    pthread_t thread_id;
    
    if (argc != 5) 
    {
        fprintf(stderr, "Usage: %s <host> <port> <max_clients> <max_time>\n", argv[0]);
        exit(1);
    }

    server_port = atoi(argv[2]);
    max_clients = atoi(argv[3]);
    max_time = atoi(argv[4]);
    
    if (max_clients < 1 || max_clients > 10 || max_time < 1 || max_time > 120) 
    {
        fprintf(stderr, "Invalid max_clients or max_time\n");
        exit(1);
    }
    
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) 
    {
        perror("Error creating socket");
        exit(1);
    }
    
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(argv[1]);
    server_address.sin_port = htons(server_port);
    
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) 
    {
        perror("Error binding socket");
        exit(1);
    }
    
    if (listen(server_socket, max_clients) < 0) 
    {
        perror("Error listening on socket");
        exit(1);
    }
    
    printf("Server started on %s:%d\n", argv[1], server_port);
    
    while (1) 
    {
        client_details *client = malloc(sizeof(client_details));
        socklen_t client_len = sizeof(client->address);
        struct thread_args *args;
        
        client->socket = accept(server_socket, (struct sockaddr *)&client->address, &client_len);

        // Error creating client 
        if (client->socket < 0) {
            perror("Error accepting connection");
            free(client);
            continue;
        }
        
        // Cannot accept more client, reached limit
        if (client_count >= max_clients) 
        {
            const char *msg = "Server is full. Try again later.";
            send(client->socket, msg, strlen(msg), 0);
            close(client->socket);
            free(client);
            continue;
        }
        
        args = malloc(sizeof(thread_arguments));
        args->client = client;
        args->max_time = max_time;
        
        // Cannot create pthread
        if (pthread_create(&thread_id, NULL, handle_client, (void *)args) < 0) 
        {
            perror("Error creating thread");
            free(client);
            free(args);
            continue;
        }
        

        // Pthread created. Detach the thread so there is no need to have codes for pthread_join. 
        pthread_detach(thread_id);
    }
    
    close(server_socket);
    return 0;
}