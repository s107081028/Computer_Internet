#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 256

int main(int argc, char *argv[])
{
    char buffer[BUFFER_SIZE] = {};
    int bufsize = sizeof(buffer);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        printf("Error creating socket!");
        return -1;
    }

    printf("please input target IP: ");
    char ip[16];
    scanf("%s", ip);
    printf("please input target port number: ");
    int port;
    scanf("%d\n", &port);

    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    if(connect(sock, (struct sockaddr*) &addr, sizeof(struct sockaddr_in)) < 0)
        printf("Error connecting!");
    
    while(gets(buffer) != EOF)
    {
        send(sock, buffer, sizeof(buffer), 0);
    }
    close(sock);
    return 0;
}