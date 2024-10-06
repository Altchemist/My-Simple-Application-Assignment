#include "server.h"

/*Prototyping functions*/
void TIME(int connfd);
void USERS(int connfd);
void SUM(int connfd, int x, int y);
void FILES(int connfd, const char filename[10]);
void EXIT(int connfd);
typedef void Sigfunc(int);
Sigfunc *signal(int signo, Sigfunc *func);
void sig_chld(int signo);
void* process_client_request(void* clientStruct);
int getpeername(int sockfd, struct sockaddr* peeraddr, socklen_t *addrlen);


int main(int argc, char **argv)
{
    int listenfd, connfd;
    struct sockaddr_in servaddr;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buff[MAXLINE];

    pthread_mutex_t file_lock;
    pthread_mutex_init(&file_lock, NULL);

    int i;

    int threadCount = 0;
    int max_clients;
    int max_times;
 

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <max_clients> <max_time>\n", argv[0]);
        exit(1);
    }

    /*Max client and Max time passed as argument*/
    max_clients = atoi(argv[1]);
    max_times = atoi(argv[2]);
    
    if (max_clients < 1 || max_clients > 10 || max_times < 1 || max_times > 120) {
        fprintf(stderr, "Invalid max_clients or max_time\n");
        exit(1);
    }

    pthread_t thread_id[max_clients];
    int client[max_clients];

    threadclient *clientArr;
    clientArr = (threadclient*)calloc(max_clients, sizeof(threadclient));
    if (clientArr == NULL) 
    {
        perror("Failed to allocate memory for clientArr");
        exit(1);
    }

    
    /*Initialize available client index for prethreading*/
    for(i=0; i<max_clients; i++)
    {
        client[i] = 0;
    }

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(52000);


    if(bind(listenfd, (SockAddr *) &servaddr, sizeof(servaddr)) <0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if(listen(listenfd, LISTENQ) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    
    for(;;)
    {
        connfd = accept(listenfd, (SockAddr*) NULL, NULL);

        if(threadCount==max_clients)
        {
            sprintf(buff, "Max amount of client accepted reached : %d", max_clients);
            write(connfd, buff, strlen(buff));
        }
        else
        {
            /*Check available thread*/
            for(i=0; i<max_clients; i++)
            {
                /*search for an available thread*/ 
                if(client[i]==0)
                {
                    threadCount++;

                    client[i] = 1;
                    /*Initializie struct for client containing important details*/
                    clientArr[i].index = i;
                    clientArr[i].serverConn = listenfd;
                    clientArr[i].clientConn = connfd;
                    clientArr[i].threadid = thread_id[i];
                    clientArr[i].ptrIndex = &client[i];


                    if(getpeername(clientArr[i].clientConn, (struct sockaddr*)&client_addr, &addr_len) == 0)
                    {
                        clientArr[i].address = client_addr.sin_addr.s_addr;
                        clientArr[i].port = ntohs(client_addr.sin_port);
                    }

                    pthread_create(&thread_id[i], NULL, process_client_request, (void*)&clientArr[i]);
                    pthread_detach(thread_id[i]);
                    break;
                }
            }
            continue;
        }
    }


    close(listenfd);
    free(clientArr);
    pthread_mutex_destroy(&file_lock);

    return 0;
}

void SUM(int connfd, int x, int y)
{
    int value = x+y;
    char buffer[40];
    sprintf(buffer, "The sum of %d and %d is : %d",x,y,value);
    writen(connfd, buffer, strlen(buffer));
}

void TIME(int connfd)
{
    char buff[MAXLINE];
    time_t ticks;
    
    ticks = time(NULL);
    snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
    writen(connfd, buff, strlen(buff));
}

void USERS(int connfd)
{
    FILE *userfile = fopen("users.txt", "r");
    
    char readBuffer[100];

    if(userfile==NULL)
    {
        sprintf(readBuffer, "File error: The file does not exist\n");
        writen(connfd, readBuffer, strlen(readBuffer));
    }

    while(fgets(readBuffer, 100, userfile))
    {
        writen(connfd, readBuffer, strlen(readBuffer));
    }

    fclose(userfile);
}

void FILES(int connfd, const char filename[10])
{
    FILE *file = fopen(filename, "r");

    char readBuffer[100];

    if(file == NULL)
    {
        sprintf(readBuffer, "File error: The file does not exist\n");
        writen(connfd, readBuffer, strlen(readBuffer));
        printf("error\n");
    }

    while(fgets(readBuffer, sizeof(readBuffer), file) != NULL) {
        readBuffer[strcspn(readBuffer, "\n")] = 0;  // Remove newline
        writen(connfd, readBuffer, strlen(readBuffer));
    }
    fclose(file);
}

void EXIT(int connfd)
{
    char buff[30]= "Terminating your MSA Session !";

    writen(connfd, buff, strlen(buff));
}

void* process_client_request(void* clientStruct)
{
    threadclient* client;
    client = (threadclient*) clientStruct;

    char decision[50];
    close(client->serverConn);

    regex_t sumPattern;
    regex_t filePattern;

    const char space[2] = " ";
    char* token;

    int match_sum = regcomp(&sumPattern, "^SUM\\((\\d+),(\\d+)\\)$",0);
    int match_file = regcomp(&filePattern, "^FILES?\\s\\w+\\.txt$", 0);

    while(1)
    {
        match_file = regexec(&filePattern, decision, 0, NULL, 0);
        match_sum = regexec(&sumPattern, decision, 0, NULL, 0);
        if(readn(client->clientConn, decision, 50)==-1)
        {
            writen(client->clientConn, "Error reading data received from client", 39);
        }
        else if(strcmp(decision, "TIME")==0)
        {
            TIME(client->clientConn);
        }
        else if(strcmp(decision, "USERS")==0)
        {
            USERS(client->clientConn);               
        }
        else if(match_sum==0)
        {
            SUM(client->clientConn, 5,6);
        }
        else if(match_file==0)
        {
            char *command = strtok(decision, space);
            char *filename = strtok(NULL, space);

            FILES(client->clientConn, filename);
        }
        else if(strcmp(decision, "EXIT")==0)
        {
            EXIT(client->clientConn);
            break;
        }
        else
        {
            write(client->clientConn, "Wrong command",13);
        }
    }
    
    close(client->clientConn);
    regfree(&sumPattern);
    regfree(&filePattern);

    client->ptrIndex = 0;
}

void log_user(threadclient* clientPtr)
{

    char client_IP[INET_ADDRSTRLEN];
    FILE *file;
    inet_ntop(AF_INET, &(clientPtr->address), client_IP, INET_ADDRSTRLEN);

    pthread_mutex_lock(clientPtr->file_mutex);

    file = fopen("users.txt", "a");

    if (file ==NULL)
    {
        perror("Error opening users.txt");
        pthread_mutex_unlock(clientPtr->file_mutex);
        return;
    }

    fprintf(file, "%s:%u\n", client_IP, clientPtr->port);
    fclose(file);

    pthread_mutex_unlock(clientPtr->file_mutex);
}

void remove_user(threadclient* clientPtr)
{
    char ipString[INET6_ADDRSTRLEN];
    char line[MAXLINE];
    FILE *file, *temp_file;
    char temp_filename[] = "users_temp.txt";

    inet_ntop(AF_INET, &(clientPtr->address), ipString, INET_ADDRSTRLEN);

    pthread_mutex_lock(clientPtr->file_mutex);

    file = fopen("users.txt", "r");

    if (file==NULL)
    {
        perror("Error opening users.txt");
        pthread_mutex_unlock(clientPtr->file_mutex);
    }
    
    temp_file = fopen("temp_users.txt", "w");

    if (temp_file==NULL)
    {
        perror("Error creating temp_users.txt");
        fclose(temp_file);
        pthread_mutex_unlock(clientPtr->file_mutex);
        return;
    }

    char client_info[MAXLINE];
    snprintf(client_info, sizeof(client_info), "%s:%u", ipString, clientPtr->port);


    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, client_info, strlen(client_info)) !=0)
        {
            fputs(line, temp_file);
        }
    }

    fclose(file);
    fclose(temp_file);

    remove("users.txt");
    rename(temp_filename, "users.txt");

    pthread_mutex_unlock(clientPtr->file_mutex);
}

ssize_t readn(int fd, void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nread;
    char *ptr;

    ptr = vptr;
    nleft = n;
    while(nleft>0)
    {
        if( (nread=read(fd, ptr, nleft)) < 0)
        {
            if(errno = EINTR)
            {
                nread = 0;
            }
            else{
                return (-1);
            }
        } else if (nread==0){break;}\

        nleft -= nread;
        ptr += nread;
    }
    return (n-nleft);
}

ssize_t writen(int fd, const void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char* ptr;

    ptr = vptr;

    nleft = n;

    while(nleft>0)
    {
        if ( (nwritten=write(fd, ptr, nleft)) <= 0 )
        {
            if (nwritten < 0 && errno == EINTR)
            {
                nwritten = 0;
            }
            else
            {
                return (-1);
            }
            nleft -= nwritten;
            ptr += nwritten;
        }
        return (n);
    }
}