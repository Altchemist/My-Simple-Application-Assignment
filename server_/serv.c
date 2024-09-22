#include "server.h"

int main(int argc, char **argv)
{
    int listenfd, connfd;
    struct sockaddr_in servaddr;
    char buff[MAXLINE];
    pid_t pid;

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
            ticks = time(NULL);
            snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
            write(connfd, buff, strlen(buff));
            close(connfd);
            exit(0);
        }
        close(connfd);
    }
}

int SUM(int x, int y)
{
    return x+y;
}

