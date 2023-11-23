#include "../sockutils/socketutil.h"
#include "../des/Des.h"

void* listenAndPrint(void* socket_FD);
void startListeningMessageThread(int &socket_fd);


struct Data{
    uint64_t data[250];
    uint64_t size;
};
const size_t DATA_SIZE = sizeof(Data);
// Des below error because of the 
DES des_send;
DES des_recv;

int main(){
    std::string keyinp;
    std::cout<<"Enter key (16-Hex): ";
    getline(std::cin, keyinp);
    if(keyinp.length() > 16){
        std::cout<<"Key must be 16 hex digit"<<std::endl;
        return -1;
    }
    des_send.setKeyStrHex(keyinp);
    des_recv.setKeyStrHex(keyinp);
    // setup socket
    int client_fd = createTCPIpv4Socket();
    struct sockaddr_in* client_addr = createIPv4Address("127.0.0.1", 2000); 


    int result = connect(client_fd, (struct sockaddr*)client_addr, sizeof(struct sockaddr_in));

    if (result < 0){
        printf("\t\t==[SERVER][FAILED]==\n");
        free(client_addr); // Free the allocated memory
        close(client_fd);  // Close the socket
        return -1;
    }else{
        printf("\t\t==[SERVER][CONNECTED]==\n");
    }

    char* line = NULL;
    size_t line_size = 0;
    
    char* name = NULL;
    size_t name_size = 0;
    printf("Enter your name: ");
    ssize_t name_count = getline(&name, &name_size, stdin);
    name[name_count-1] = 0;
    startListeningMessageThread(client_fd);

    printf("Write and Press Enter to broadcast message to server\n");
    char buffer[2000];
    Data tosend;
    while(true){
        
        ssize_t char_count = getline(&line, &line_size, stdin);

        if(char_count > 0){
            if(strcmp(line, "!exit\n") == 0){
                sprintf(buffer, "%s has left the chat...", name);
                des_send.encrypt(buffer);
                // put into tosend
                memcpy(tosend.data, des_send.getPlaintextBin(), DATA_SIZE);
                tosend.size = des_send.getPlaintextBlock();
                ssize_t amount_sent = send(client_fd,&tosend, DATA_SIZE, 0);
                break;
            }
            if(strcmp(line, "!clear\n") == 0){
                system("clear");
                continue;
            }
        }
        line[char_count-1] = 0;
        sprintf(buffer, "%s\t: %s", name, line);
        // Encrypt process
        des_send.encrypt(buffer);
        // put into tosend
        memcpy(tosend.data, des_send.getPlaintextBin(), DATA_SIZE);
        tosend.size = des_send.getPlaintextBlock();
        // send
        ssize_t amount_sent = send(client_fd,&tosend, DATA_SIZE, 0);
    }

free(client_addr); // Free the allocated memory
close(client_fd);  // Close the socket
    
return 0;
}
void* listenAndPrint(void* socket_FD){
    // char buffer[DATA_SIZE];
    Data received;

    int fd= *(reinterpret_cast<int*>(socket_FD));
    while (true)
    {
        ssize_t amount_recv = recv(fd, &received , DATA_SIZE, 0);
        if(amount_recv>1){
            for(int i = 0; i < received.size; i++){
                des_recv.decrypt(received.data[i]);
            }printf("\n");
        }else{break;}
    }

    printf("\t\t==[SERVER][DISCONNECTED]==\n");

    close(fd);

    return NULL;

}

void startListeningMessageThread(int &socket_fd){
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, listenAndPrint, &socket_fd);
}


// void sendStruct(int socket_fd, const Data& data){
//     char buffer[DATA_SIZE];
//     ssize_t amount_sent = send(socket_fd, &data, sizeof(Data), 0);
// }
