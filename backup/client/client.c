#include "client.h"
void logging(const char input[MAXLINE]);

int main(int argc, char **argv)
{
    int sockfd, n;

    char receiveBuffer[MAXLINE];
    char sendBuffer[MAXLINE]="TIME";

    regex_t sumPattern;
    regex_t filePattern;

    int match_sum = regcomp(&sumPattern, "^SUM(\d+,\d+)$",0);
    int match_file = regcomp(&filePattern, "^FILES\s\w+\.txt$", 0);
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
    int terminate = 0;


    while (terminate!=1)
    {
        fgets(sendBuffer, MAXLINE, stdin);
        sendBuffer[strcspn(sendBuffer, "\n")] = 0;

        match_file = regexec(&filePattern, sendBuffer, 0, NULL, 0);
        match_sum = regexec(&sumPattern, sendBuffer, 0, NULL, 0);

        if(IS_EXIT) 
        {
            write(sockfd, sendBuffer, strlen(sendBuffer));
            terminate=1;
        }
        else if (IS_FILES)
        {
            write(sockfd, sendBuffer, strlen(sendBuffer));
        }
        
        else if (IS_SUM)
        {
            write(sockfd, sendBuffer, strlen(sendBuffer));
        }
        
        else if(IS_TIME)
        {
            write(sockfd, sendBuffer, strlen(sendBuffer));
        }
        else if (IS_USERS)
        {
            write(sockfd, sendBuffer, strlen(sendBuffer));
            logging(sendBuffer);
        }
        else
        {
            printf("Wrong command input. \n");
            printf("###########################################################################\n");
            printf("###                My Simple Application Commands 			###\n");
            printf("###									###\n");
            printf("### TIME                 : get current time from server			###\n");
            printf("### USERS                : get all users currently connected to server	###\n");
            printf("### FILES <filename.txt> : get all content of file given		###\n");
            printf("### SUM(value1,value2)   : get sum of 2 values				###\n");
            printf("### EXIT 	         : exit from application			###\n");
            printf("###########################################################################\n");
        }

        /*Check if data received from server, and either output error or log and print data*/
        if (read(sockfd, receiveBuffer, MAXLINE) == 0)
        {
            printf("Error server output");
        }
        else
        {
            fputs(receiveBuffer, stdout);
            logging(receiveBuffer);
        }
    }

    exit(0);
}


void logging(const char input[MAXLINE])
{
    FILE* logf = fopen("log.txt", "a");

    fprintf(logf, input);

    fclose(logf);
}