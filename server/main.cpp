#include "../sockutils/socketutil.h"




int main(){
    // setup socket
    int server_fd = createTCPIpv4Socket();
    sockaddr_in* server_addr = createIPv4Address("", 2000); 
    // Binding
    if(bind(server_fd, (sockaddr*)server_addr, sizeof(sockaddr_in)) == 0){
        printf("server bind success\n");
    }else{
        printf("server bind failed\n");
        return -1;
    }
    // Listening
    int listenResult = listen(server_fd, 10);
    // Accepting
    // struct AcceptedSocket*  client_socket = acceptIncomingConnection(server_fd);
    servers::startAcceptingIncomingConnectionThread(server_fd);

// Free the allocated memory
free(server_addr);
// Close the socket
// close(client_socket->acceptedSocketFD);
// Shutdown the socket
shutdown(server_fd, SHUT_RDWR);
// close(server_fd);  // Close the socket

    
return 0;
}