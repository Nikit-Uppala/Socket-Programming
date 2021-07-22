#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<stdio.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<signal.h>
#define PORT 8000
int main()
{
    long long int buff_size = 16000;
    char buffer[buff_size];
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketfd==-1)
    {
        perror("socket");
        return 0;
    }
    int opt=1;
    if(setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))==-1)
    {
        perror("setsockopt");
        return 0;
    }
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(PORT);
    bind(socketfd, (struct sockaddr*)&serv_addr, (socklen_t)sizeof(serv_addr));
    if(listen(socketfd, 1)==-1)
    {
        perror("listen");
        return 0;
    }
    int fd = -1;
    long long int size = sizeof(serv_addr);
    while (1)
    {
        if(fd==-1)
            fd = accept(socketfd, (struct sockaddr*)&serv_addr, (socklen_t*)&size);
        if(fd==-1)
        {
            perror("accept");
            return 0; 
        }
        int numFiles = 0;
        if(recv(fd, (void*)&numFiles, sizeof(numFiles), MSG_NOSIGNAL)==0)
        {
            fd = -1;
            continue;
        }
        numFiles = numFiles-1;
        long long int error = -1;
        for(int i=0;i<numFiles;i++)
        {
            buff_size = 16000;
            memset(buffer, 0, buff_size);
            if(recv(fd, (void*)buffer, buff_size, 0)==0)
            {
                fd=-1;
                break;
            }
            int file = open(buffer, O_RDONLY);
            if(file==-1)
            {
                if(send(fd, (void*)&error, sizeof(error), MSG_NOSIGNAL)==0)
                {
                    fd = -1;
                    break;
                }
                continue;
            }
            struct stat fileInfo;
            long long int check = stat(buffer, &fileInfo);
            if(check==-1)
            {
                if(send(fd, (void*)&error, sizeof(error), MSG_NOSIGNAL)==0)
                {
                    fd = -1;
                    break;
                }
                continue;
            }
            long long int file_size = fileInfo.st_size;
            printf("%lld\n", file_size);
            if(send(fd, (void*)&file_size, sizeof(file_size), MSG_NOSIGNAL)==0)
            {
                fd = -1;
                break;
            }
            long long int sent = 0;
            if(file_size<buff_size)
                buff_size = file_size;
            int start;
            if(recv(fd, (void*)&start, sizeof(start), MSG_NOSIGNAL)==0)
            {
                fd = -1;
                break;
            }
            if(start==0)
                continue;
            while(sent<file_size)
            {
                // send(fd, &buff_size, sizeof(buff_size), MSG_NOSIGNAL);
                read(file, buffer, buff_size);
                if(send(fd, buffer, buff_size, MSG_NOSIGNAL)==0)
                {
                    fd = -1;
                    break;
                }
                sent+=buff_size;
                if(file_size-sent<buff_size)
                    buff_size = file_size - sent;
            }
            if(fd==-1)
                break;
        }
    }
    return 0;
}