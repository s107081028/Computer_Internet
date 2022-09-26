#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define LENGTH 16
struct Data_sequence{
    int seq_num;
    char Data[LENGTH];
};
struct ack_pkt{
    int seq_num;
};

int main(int argc , char *argv[])
{
    //===========================================
	// Todo: Create TCP socket and TCP connection
	//===========================================
    int socket_desc;
    int losses[100];
    int loss = 0;
    int cur_loss = loss;
    int cur_final = 0;
    int port = atoi(argv[1]);
    struct sockaddr_in server_addr;
    
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    
    if(socket_desc < 0){
        printf("Unable to create socket\n");
        return -1;
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    if(connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        printf("Unable to connect\n");
        return -1;
    }
    printf("Connected successfully\n");


    //========================
	// Receive data / send ack
	//========================
    struct Data_sequence data_seq;
    struct ack_pkt ACK;
    while(1){
        //=================================================================
        // Todo: 1. receive data and send ACKs with correct sequence number
        //       2. simulate packet loss (or you can implement in server.c)
        // Required format: 
        //       received: seq_num = [sequence number]
        //       loss: seq_num = [seq_num]
        //=================================================================
        // Get input from the user:
        
        if(recv(socket_desc, (void*)&data_seq, sizeof(data_seq), 0) < 0){
            printf("Error while receiving server's msg\n");
            return -1;
        }
        
        cur_final = (data_seq.seq_num > cur_final) ? data_seq.seq_num : cur_final;
        if ((data_seq.seq_num == 8 && cur_final == 8) || (data_seq.seq_num == 13 && cur_final == 13) || (data_seq.seq_num == 250 && cur_final == 250))
        {
            printf("loss: seq_num = %d\n", data_seq.seq_num);
            losses[loss] = data_seq.seq_num;
            loss += 1;
            ACK.seq_num = ACK.seq_num;
        }
        else if (data_seq.seq_num == ACK.seq_num && cur_loss != loss)
        {
            printf("received: seq_num = %d\n", data_seq.seq_num);
            if (cur_loss == loss - 1)
            {
                ACK.seq_num = cur_final + 1;
                cur_loss++;
            }
            else
                ACK.seq_num = losses[++cur_loss];
        } 
        else if (cur_loss == loss)
        {
            ACK.seq_num = data_seq.seq_num + 1;
            printf("received: seq_num = %d\n", data_seq.seq_num);
        }
        else
            printf("received: seq_num = %d\n", data_seq.seq_num);

        if(send(socket_desc, (void*)&ACK, sizeof(ACK), 0) < 0){
            printf("Unable to send message\n");
            return -1;
        }
    }
    
    close(socket_desc);
    
    return 0;
}