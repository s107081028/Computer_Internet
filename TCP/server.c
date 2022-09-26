#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define LENGTH 16
int ROUND = 50;

//======================================
// Data sequence
// sample sequence packet:
//    seq_num: sequence number
//    Data: data
// you can add another variables if need
//======================================
struct Data_sequence{
    int seq_num;
    char Data[LENGTH];
};

//================================
// ACK packet with sequence number
//================================
struct ack_pkt{
    int seq_num;
};

//======================
// Parameter declaration
//======================
int sockfd = 0;
struct sockaddr_in serverInfo;
int clientSockfd;
struct sockaddr_in clientInfo;
struct Data_sequence data_seq;
struct ack_pkt ACK;
int cwnd = 1;
int ssthresh = 8;
int losses[100];
int loss = 0;
int cur_loss = 0;
int cur_final = 0;
bool cut_half = false;
bool set_one = false;
bool not_changed = true;

bool not_in(int target)
{
    for(int i = 0; i < loss; i++)
    {
        if(target == losses[i])
        {
            return false;
        }
    }
    return true;
}

void sender(int client_sock, struct Data_sequence* data_seq){
    //===========================================
    // Write your send here
    // Todo: 1. send cwnd sequences of data that
    //          starts with right sequence number
    // Required format: 
    //       send: seq_num = [sequence number]
    //===========================================
    not_changed = false;
    int init = cur_final;
    for(int i = 0; i < cwnd; i++)
    {
        if (cur_loss == loss)
            data_seq->seq_num = cur_final;
        else
            data_seq->seq_num = losses[cur_loss++];
        if (send(client_sock, (void*)data_seq, sizeof(*data_seq), 0) < 0){
            printf("Can't send\n");
            return;
        }
        printf("send: seq_num = %d\n", data_seq->seq_num);
        data_seq->seq_num = data_seq->seq_num + 1;
        cur_final = (data_seq->seq_num > cur_final) ? data_seq->seq_num : cur_final;
    }
    if (init == cur_final)
        not_changed = true;
}

void receiver(int client_sock, struct ack_pkt* ACK)
{
    //====================================================
    // Write your recv here
    // Todo: 1. receive ACKs from client
    //       2. detect if 3-duplicate occurs
    //       3. update cwnd and ssthresh
    //       4. remember to print the above information
    // Required format: 
    //       ACK: [sequence number]
    //       3-duplicate ACKs: seq_num = [sequence number]
    //       cwnd = [cwnd], ssthresh = [ssthresh]
    //====================================================
    int duplicate_start, count = 0;
    for(int i = 0; i < cwnd; i++)
    {
        if(recv(client_sock, (void*)ACK, sizeof(*ACK), 0) < 0){
            return;
        }
        printf("ACK: seq_num = %d\n", ACK->seq_num);
        if(ACK->seq_num != (cur_final - cwnd + i + 1))
        {
            if (duplicate_start == ACK->seq_num)
                count += 1;
            else if (not_in(ACK->seq_num))
            {
                losses[loss] = ACK->seq_num;
                loss += 1;
                duplicate_start = ACK->seq_num;
                count = 2;
                cut_half = (not_changed) ? false : true;
            }
        }
        if(count == 3)
        {
            set_one = true;
            printf("3-duplicate ACKs: seq_num = %d, cwnd = %d, ssthresh = %d\n", duplicate_start, cwnd, ssthresh);
        }
    }
}

int main(int argc, char *argv[])
{
    //===========================================
	// Todo: Create TCP socket and TCP connection
	//===========================================
	int socket_desc, client_sock, client_size;
    int port = atoi(argv[1]);
    struct sockaddr_in server_addr, client_addr;
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_desc < 0){
        printf("Error while creating socket\n");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        printf("Couldn't bind to the port\n");
        return -1;
    }

    if(listen(socket_desc, 1) < 0){
        printf("Error while listening\n");
        return -1;
    }
    printf("Listening for incoming connections.....\n");

    client_size = sizeof(client_addr);
    client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);
    
    if (client_sock < 0){
        printf("Can't accept\n");
        return -1;
    }
    printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    

    //============================================================================
	// Start data transmission
    // To be simple, we receive ACKs after all cwnd sequences of data are sent.
    // Thus we can not react to 3-duplicate immediately, which is OK for this lab.
	//============================================================================
    printf("state: slow start\n");
    printf("cwnd = %d, ssthresh = %d\n", cwnd, ssthresh);
    while(ROUND--){
        sender(client_sock, &data_seq);
        receiver(client_sock, &ACK);
        
        if (cut_half)
        {
            ssthresh = cwnd / 2;
            cut_half = false;
        }
        if (set_one)
        {
            cwnd = 1;
            set_one = false;
            printf("state: slow start\n");
            printf("cwnd = %d, ssthresh = %d\n", cwnd, ssthresh);
        }
        else if (cwnd < ssthresh)
        {
            cwnd *= 2;
            printf("cwnd = %d, ssthresh = %d\n", cwnd, ssthresh);
        }
        else
        {
            cwnd += 1;
            printf("state: congestion avoidance\n");
            printf("cwnd = %d, ssthresh = %d\n", cwnd, ssthresh);
        }
    }

    close(client_sock);
    close(socket_desc);
    
    return 0;
}