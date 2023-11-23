#ifndef SOCKETUTIL_SOCKETUTIL_H
#define SOCKETUTIL_SOCKETUTIL_H
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string>
#include <vector>
#include <algorithm>
#include <stdint.h>


// Universal Function
int createTCPIpv4Socket(){
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd<0){
        printf("socket failed\n");
        return -1;
    }
    return fd;
}

sockaddr_in* createIPv4Address(const char* ip, int port){
    sockaddr_in* addr = (sockaddr_in*)malloc(sizeof(sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);

    if(strlen(ip)==0){
        addr->sin_addr.s_addr = INADDR_ANY;
        return addr;
    }
    inet_pton(AF_INET, ip, &addr->sin_addr.s_addr);
    return addr;
}




// server
namespace servers{
struct AcceptedSocket
{
    int acceptedSocketFD;
    struct sockaddr_in address;
    int error;
    bool acceptedSuccessfully;
};
// Global Variables
// kita akan ganti dengan vector
// struct AcceptedSocket acceptedSockets[10];
std::vector<struct AcceptedSocket> acceptedSockets;

// this is the number of accepted sockets
// we will use this to iterate through the acceptedSockets array
// we do increment inside the function acceptIncomingConnection
// we will decrement it inside the function receiveAndPrintIncomingData
// the decrement we put after the close function


// we will print the message about the client that connected to the server in function acceptIncomingConnection
// especially in under the if statement that check if the client_socket->acceptedSuccessfully is true

struct AcceptedSocket* acceptIncomingConnection(int server_fd){
    sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(sockaddr_in);
    int clientSockedFD = accept(server_fd, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_len);
    
    struct AcceptedSocket* acceptedSocket = (struct AcceptedSocket*)malloc(sizeof(struct AcceptedSocket));
    acceptedSocket->address = client_addr;
    acceptedSocket->acceptedSocketFD = clientSockedFD;
    acceptedSocket->acceptedSuccessfully = clientSockedFD>0;
    if(acceptedSocket->acceptedSuccessfully){
        acceptedSocket->error = clientSockedFD;
        // we will print the ip and port connected to the server
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, ip, INET_ADDRSTRLEN);
        printf("[CLIENT][CONNECTED]::%s:%d\n", ip, ntohs(client_addr.sin_port));
        
    }
    return acceptedSocket;
}

void sendReceivedMessageToTheOtherClients(const char* message, int senderSocketFD){
    for(int i=0;i<acceptedSockets.size();i++){
        if(acceptedSockets[i].acceptedSocketFD!=senderSocketFD){
            int statuss = send(acceptedSockets[i].acceptedSocketFD, message, 2008, 0);
        }
    }
}

void* receiveAndPrintIncomingData(void* socket_FD){
    char buffer[2008];
    int fd = *(reinterpret_cast<int*>(socket_FD));
    while (true)
    {
        ssize_t amount_recv = recv(fd, buffer, 2008, 0);
        if(amount_recv>1){
            buffer[amount_recv-1] = 0;
            printf("[CLIENT][MESSAGE]::%s\n", buffer);

            sendReceivedMessageToTheOtherClients(buffer, fd);
        }else{
            printf("[CLIENT][DISCONNECTED]::%d\n",fd);
            break;
        }
    }

    close(fd);
    auto vidx = std::find_if(acceptedSockets.begin(), acceptedSockets.end(), [fd](struct AcceptedSocket& acceptedSocket){
        return acceptedSocket.acceptedSocketFD == fd;
    });
    // auto vidx = std::find(acceptedSockets.begin(), acceptedSockets.end(), fd);
    if(vidx!=acceptedSockets.end()){
        acceptedSockets.erase(vidx);
    }
    // acceptedSocketsCount--;
    return NULL;

}
void receiveAndPrintIncomingDataOnSeparatedThread(struct AcceptedSocket* p_socket){
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, receiveAndPrintIncomingData, &(p_socket->acceptedSocketFD));

}
void* acceptNewConnectionAndReceiveAndPrintItsData(void* socket_fd){

    while(true){
        struct AcceptedSocket*  client_socket = acceptIncomingConnection(*(reinterpret_cast<int*>(socket_fd)));
        receiveAndPrintIncomingDataOnSeparatedThread(client_socket);   
    }
    return NULL;
}

void startAcceptingIncomingConnectionThread(int socket_fd){
    while(true){
        struct AcceptedSocket*  client_socket = acceptIncomingConnection(socket_fd);
        acceptedSockets.push_back(*client_socket);
        receiveAndPrintIncomingDataOnSeparatedThread(client_socket);   
    }
    // pthread_t thread_id;
    // pthread_create(&thread_id, NULL, acceptNewConnectionAndReceiveAndPrintItsData, reinterpret_cast<void*>(&socket_fd));

}

}

#endif // SOCKETUTIL_SOCKETUTIL_H