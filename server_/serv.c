#include "server.h"

/*Prototyping functions*/
void TIME(int connfd);
void USERS(int connfd);
void SUM(int connfd, int x, int y);
void FILES(const char filename[10]);
void EXIT(int connfd);


int main(int argc, char **argv)
{
    int listenfd, connfd;
    ssize_t n;
    struct sockaddr_in servaddr;
    char buff[MAXLINE];
    pid_t pid;

    char decision[] = "USERS";

    if(argc>1)
    {
        /*Max client and Max time passed as argument*/
        int max_client = atoi(argv[1]);
        int max_time = atoi(argv[2]);
    }

    
    time_t ticks;

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

        if ( (pid==fork()) == 0)
        {
            close(listenfd);

        //  again:
        //     while ( (n = read(connfd, buff, MAXLINE)) > 0)
        //     {
        //         write(connfd, buff, n);
        //         if (n < 0 && errno == EINTR)
        //         {
        //             goto again;
        //         }
        //         else if (n < 0)
        //         {
        //             printf("str_echo: read error");

        //         }
        //     }


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
                printf("FILES NOT IMPLEMENTED");
            }

            if(strcmp(decision, "EXIT")==0)
            {
                EXIT(connfd);
            }

            close(connfd);
            exit(0);
        }
        close(connfd);
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

void FILES(const char filename[10])
{
    FILE *userfile = fopen(filename, "r");

}

void EXIT(int connfd)
{
    char buff[30]= "Terminating your MSA Session !";

    write(connfd, buff, strlen(buff));
}