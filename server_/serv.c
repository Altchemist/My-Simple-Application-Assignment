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
    ssize_t n;
    struct sockaddr_in servaddr;
    char buff[MAXLINE];

    pthread_mutex_t file_lock;
    pthread_mutex_init(&file_lock, NULL);

    pid_t pid;
    char decision[] = "TIME";
    int i;

    int threadCount = 0;
    int max_client = 1;
    int max_time = 120;
    int client[max_client];
    int sockfd[max_client];
    pthread_t thread_id[max_client];
    struct threadclient *clientArr;

    if(argc>1)
    {
        /*Max client and Max time passed as argument*/
        max_client = atoi(argv[1]);
        max_time = atoi(argv[2]);
    }
    else
    {
        pthread_t thread_id[max_client];
        int client[max_client];
        clientArr = (struct threadclient*)calloc(max_client, sizeof(struct threadclient));
    }

    
    /*Initialize available client index for prethreading*/
    for(i=0; i<max_client; i++)
    {
        client[i] = 0;
    }

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(52000);


    bind(listenfd, (SockAddr *) &servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);
    
    for(;;)
    {
        connfd = accept(listenfd, (SockAddr*) NULL, NULL);

        if(threadCount==max_client)
        {
            sprintf(buff, "Max amount of client accepted reached : %d", max_client);
            write(connfd, buff, strlen(buff));
        }
        else
        {
            /*Check available thread*/
            for(i=0; i<max_client; i++)
            {
                /*search for an available thread*/ 
                if(client[i]==0)
                {
                    threadCount++;

                    client[i] == 1;

                    /*Initializie struct for client containing important details*/
                    clientArr[i].index = i;
                    clientArr[i].serverConn = listenfd;
                    clientArr[i].clientConn = connfd;
                    clientArr[i].threadid = thread_id[i];

                    pthread_create(&thread_id[i], NULL, process_client_request, (void*)&clientArr[i]);
                    pthread_join(thread_id[i], NULL);
                    break;
                }
            }
            exit(0);
        }
    }

    free(clientArr);
    pthread_mutex_destroy(&file_lock);

    return 0;
}

void SUM(int connfd, int x, int y)
{
    int value = x+y;
    char buffer[40];
    sprintf(buffer, "The sum of %d and %d is : %d",x,y,value);
    write(connfd, buffer, strlen(buffer));
}

void TIME(int connfd)
{
    char buff[MAXLINE];
    time_t ticks;
    
    ticks = time(NULL);
    snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
    write(connfd, buff, strlen(buff));
}

void USERS(int connfd)
{
    FILE *userfile = fopen("users.txt", "r");
    
    char readBuffer[100];

    if(userfile==NULL)
    {
        sprintf(readBuffer, "File error: The file does not exist\n");
        write(connfd, readBuffer, strlen(readBuffer));
    }

    while(fgets(readBuffer, 100, userfile))
    {
        write(connfd, readBuffer, strlen(readBuffer));
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
        write(connfd, readBuffer, strlen(readBuffer));
        printf("error\n");
    }

    while(fgets(readBuffer, 100, file))
    {
        write(connfd, readBuffer, strlen(readBuffer));
    }

    fclose(file);
}

void EXIT(int connfd)
{
    char buff[30]= "Terminating your MSA Session !";

    write(connfd, buff, strlen(buff));
}

// Sigfunc *signal(int signo, Sigfunc *func)
// {
//     struct sigaction act, oact;

//     act.sa_handler = func;

//     sigemptyset(&act.sa_mask);
//     act.sa_flags = 0;

//     if (sigaction(signo, &act, &oact) < 0)
//     {
//         return (SIG_ERR);
//     }
//     return (oact.sa_handler);
// }

// void sig_chld(int signo)
// {
//     pid_t pid;
//     int stat;

//     pid = wait(&stat);

//     return;
// }

void* process_client_request(void* clientStruct)
{
    struct threadclient* client;
    client = (struct threadclient*) clientStruct;

    char decision[50];
    close(client->serverConn);

    regex_t sumPattern;
    regex_t filePattern;

    const char space[2] = " ";
    char* token;

    int match_sum = regcomp(&sumPattern, "^SUM(\d+,\d+)$",0);
    int match_file = regcomp(&filePattern, "^FILES\s\w+\.txt$", 0);

    match_file = regexec(&filePattern, decision, 0, NULL, 0);
    match_sum = regexec(&sumPattern, decision, 0, NULL, 0);
    
    if(read(client->clientConn, decision, 50)==-1){
        write(client->clientConn, "Error reading data received from client", 39);
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
        token = strtok(decision, space);
        
        FILES(client->clientConn, token[1]);
    }
    else if(strcmp(decision, "EXIT")==0)
    {
        EXIT(client->clientConn);
    }
    else
    {
        write(client->clientConn, "Wrong command",13);
    }
    close(client->clientConn);
}

void log_user()
{
    
}

void remove_user()
{

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