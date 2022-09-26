#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 256

void *serverThread(void *input)
{
    int portNum = *(int *)input;
    /*
     * Write your server thread here.
    **/
    char buffer[BUFFER_SIZE] = {};
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        printf("Error creating socket!");
        return;
    }

    struct sockaddr_in addr, client_addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(portNum);
    addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sock, (struct sockaddr*) &addr, sizeof(struct sockaddr_in)) < 0)
        printf("Error Binding!");
    
    int ls = listen(sock, 5);
    if(ls < 0)
        printf("Error Listening!");

    int Clientsockfd = 0;
    int addrlen = sizeof(client_addr);
    Clientsockfd = accept(sock, (struct sockaddr*) &client_addr, &addrlen);
    while(1)
    {
        if(recv(Clientsockfd, buffer, sizeof(buffer), 0) > 0)
            printf("recv from client: %s\n", buffer);
    }
    close(sock);
    return;
}

void *clientThread(void *input){
    /*
     * Write your client thread here.
    **/
    char buffer[BUFFER_SIZE] = {};
    int bufsize = sizeof(buffer);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        printf("Error creating socket!");
        return;
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
    return;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        return 0;
    }
    int portNum = atoi(argv[1]);
    pthread_t clientTid, serverTid;
    pthread_create(&clientTid, NULL, clientThread, NULL);
    pthread_create(&serverTid, NULL, serverThread, &portNum);
    pthread_join(clientTid, NULL);
    pthread_join(serverTid, NULL);
    return 0;
}