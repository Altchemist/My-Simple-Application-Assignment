#include "client.h"

int main(int argc, char **argv)
{
    int sockfd, n;

    char recvline[MAXLINE+1];
    struct sockaddr_in servaddr;

    if(argc!=2)
    {
        printf("usage: msa.out <server's IPaddress>\n");
    }

    if ((sockfd=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        printf("Socket Error\n");
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(52000);

    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
    {
        printf("inet_pton error for %s\n", argv[1]);
    }

    if(connect(sockfd, (SockAddr*) &servaddr, sizeof(servaddr)) < 0)
    {
        printf("connect error\n");
    }

    while (( n = read(sockfd, recvline, MAXLINE)) > 0)
    {
        recvline[n] = 0;
        if (fputs(recvline, stdout) == EOF)
        {
            printf("fputs error\n");
        }

        if (n<0){
            printf("read error\n");
        }

        exit(0);
    }
}