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
    // printf("port: ");    
    int port = atoi(argv[1]);
    // printf("%d\n", port);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        printf("Error creating socket!");
        return -1;
    }

    struct sockaddr_in addr, client_addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sock, (struct sockaddr*) &addr, sizeof(struct sockaddr_in)) < 0)
        printf("Error Binding!");
    
    int ls = listen(sock, 5);
    if(ls < 0)
        printf("Error Listening!");
    // printf("%d\n", ls);

    int Clientsockfd = 0;
    int addrlen = sizeof(client_addr);
    Clientsockfd = accept(sock, (struct sockaddr*) &client_addr, &addrlen);
    while(1)
    {
        if(recv(Clientsockfd, buffer, sizeof(buffer), 0) > 0)
            printf("recv from client: %s\n", buffer);
    }
    close(sock);
    return 0;
}