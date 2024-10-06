#include "client.h"
void logging(const char input[MAXLINE]);

int main(int argc, char **argv)
{
    int sockfd, n;

    char receiveBuffer[MAXLINE];
    char sendBuffer[MAXLINE];

    regex_t sumPattern;
    regex_t filePattern;

    int match_sum = regcomp(&sumPattern, "^SUM\\((\\d+),(\\d+)\\)$",0);
    int match_file = regcomp(&filePattern, "^FILES?\\s\\w+\\.txt$", 0);
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


    while (1)
    {
        printf("Client> ");
        if(fgets(sendBuffer, MAXLINE, stdin)==NULL)
        {
            break;
        };

        /* Remove newline character */
        sendBuffer[strcspn(sendBuffer, "\n")] = '\0';

        if (strlen(sendBuffer) == 0) {
            continue;
        }

        match_file = regexec(&filePattern, sendBuffer, 0, NULL, 0);
        match_sum = regexec(&sumPattern, sendBuffer, 0, NULL, 0);

        fputs(sendBuffer,stdout);

  
        if (IS_FILES || IS_SUM || IS_TIME || IS_USERS || IS_EXIT)
        {
            send(sockfd, sendBuffer, strlen(sendBuffer),0);
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
        int bytes_amount = recv(sockfd, receiveBuffer, MAXLINE-1, 0);
        if (bytes_amount < 0)
        {
            printf("Error server data\n");
        }
        else if (bytes_amount > 0)
        {
            fputs(receiveBuffer, stdout);
            logging(receiveBuffer);
        }
        else
        {
            printf("Server closed the connection\n");
        }

        if(IS_EXIT) 
        {
            break;
        }
    }

    regfree(&sumPattern);
    regfree(&filePattern);
    exit(0);
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

void logging(const char input[MAXLINE])
{
    FILE* logf = fopen("log.txt", "a");
    if(logf!=NULL)
    {
        fprintf(logf, "%s\n" ,input);
        fflush(logf);
        fclose(logf);
    }
    else
    {
        perror("Error opening log file\n");
    }
    
}