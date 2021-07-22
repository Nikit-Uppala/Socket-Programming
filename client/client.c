#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#define PORT 8000
int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in cli_addr;
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    cli_addr.sin_port = htons(PORT);
    int connection = connect(sockfd, (struct sockaddr*)&cli_addr, (socklen_t)sizeof(cli_addr));
    if(connection==-1)
    {
        perror("connect");
        return 0;
    }
    printf("Connected to server\n");
    int argc = 0;
    char* argv[1001] = {NULL};
    long long int buff_size = 16000;
    char buffer[buff_size];
    int temp = argc-1, start=0;
    long long int feedback;
    size_t size = 1001;
    char* command = calloc(sizeof(char), 1001);
    while(1)
    {
        printf("client>");
        getline(&command, &size, stdin);
        command[strlen(command)-1] = '\0';
        if(strlen(command)<=0)
            continue;
        for(int i=0; i<argc; i++)
            if(argv[i]!=NULL) free(argv[i]);
        argc = 0;
        for(char* token = strtok(command, " \t"); token!=NULL; token = strtok(NULL, " \t"))
        {
            argv[argc] = calloc(sizeof(char), strlen(token));
            strcpy(argv[argc++], token);
        }
        if(strcmp(argv[0], "exit")==0)
        {
            free(command);
            return 0;
        }
        if(strcmp(argv[0], "get")!=0)
        {
            printf("Invalid command\n");
            continue;
        }
        send(sockfd, (void*)&argc, sizeof(argc), 0);
        for(int i=1;i<argc;i++)
        {
            feedback = 0;
            buff_size = 16000;
            memset(buffer, 0, buff_size);
            printf("%s\n", argv[i]);
            send(sockfd, (void*)argv[i], strlen(argv[i]), 0);
            if(recv(sockfd, (void*)&feedback, sizeof(feedback), 0)==-1)
            {
                perror("recv");
                printf("\n");
                return 0;
            }
            if(feedback==-1)
            {
                printf("File is either missing or cannot be accessed\n\n");
                continue;
            }
            long long int recieved = 0;
            if(feedback<buff_size)
                buff_size = feedback;
            int file = open(argv[i], O_WRONLY | O_TRUNC | O_CREAT, 0644);
            if(file==-1)
            {
                perror(argv[i]);
                printf("\n");
                start=0;
                send(sockfd, (void*)&start, sizeof(int), 0);
                continue;
            }
            start = 1;
            send(sockfd, (void*)&start, sizeof(start), 0);
            while(recieved<feedback)
            {
                if(recv(sockfd, buffer, buff_size, 0)<=0)
                {
                    perror("recv");
                    printf("\n");
                    break;
                }
                write(file, buffer, buff_size);
                recieved+=buff_size;
                if(feedback-recieved<buff_size)
                    buff_size = feedback - recieved;
                printf("\rPercentage Downloaded - %.2f%%", (float)recieved/(float)feedback * 100.0);
            }
            printf("\n\n");
        }
    }
    return 0;
}