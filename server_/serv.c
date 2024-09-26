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
void* process_request(void* connfd, void* listenfd, void** clientPtr)



int main(int argc, char **argv)
{
    int listenfd, connfd;
    ssize_t n;
    struct sockaddr_in servaddr;
    char buff[MAXLINE];
    pid_t pid;
    char decision[] = "FILES";
    int i;

    int max_client = 1;
    int max_time = 120;
    int client[max_client];
    int sockfd[max_client];
    pthread_t thread_id[max_client];

    if(argc>1)
    {
        /*Max client and Max time passed as argument*/
        max_client = atoi(argv[1]);
        max_time = atoi(argv[2]);
    }
    else
    {
        int sockfd[max_client];
        pthread_t thread_id[max_client];
        int client[max_client];
    }

    int threadCount = 0;

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
            close(connfd);
            for(i=0; i<max_client; i++)
            {
                /*search for an available thread*/ 
                if(client[i]==0)
                {
                    threadCount++;
                    pthread_create(&thread_id[i], NULL, process_request((int)connfd, (int)listenfd, (int**)&client), NULL);
                    pthread_join(thread_id[i], NULL);
                    break;
                }
            }
            exit(0);
        }
    }
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

void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    pid = wait(&stat);

    return;
}

void* process_request(void* connfd, void* listenfd, void** clientPtr)
{
    char decision[] = "TIME";

    close(listenfd);

    if(strcmp(decision, "TIME")==0)
    {
        TIME(connfd);
    }
    
    if(strcmp(decision, "USERS")==0)
    {
        USERS(connfd);               
    }

    if(strcmp(decision, "SUM")==0)
    {
        SUM(connfd, 5,6);
    }

    if(strcmp(decision, "FILES")==0)
    {
        FILES(connfd, "file1.txt");
    }

    if(strcmp(decision, "EXIT")==0)
    {
        EXIT(connfd);
    }

    close(connfd);
    
}