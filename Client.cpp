#include <iostream>
#include <cstring>      // For memset()
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in structure
#include <arpa/inet.h>
#include <unistd.h>     // For close() function


int main(){
    int socket_fd = socket(AF_INET,SOCK_STREAM,0); 

    if(socket_fd < 0 ){
        std::cerr << "Failed creation of socket" << std::endl;
        return -1;
    }

    struct sockaddr_in server_address;
    memset(&server_address,0,sizeof(server_address));
    // Configure Addresses
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(5000);

    if(connect(socket_fd,(struct sockaddr*)&server_address,sizeof(server_address))<0){
        std::cerr << "Failed connection"<<std::endl;
        return -1;
    }

    char buffer[1024] = {0};
    int valread = recv(socket_fd, buffer, 1024, 0);
    std::cout << "Message received: " << buffer << std::endl;

    // const char *hello = "Hello from server";
    // send(socket_fd, hello, strlen(hello), 0);
    // std::cout << "Hello message sent" << std::endl;

    close(socket_fd);
    return 0; 
}