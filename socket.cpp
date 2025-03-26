#include <iostream>
#include <cstring>      // For memset()
#include <sys/socket.h> // For socket functions
#include <arpa/inet.h>
#include <netinet/in.h> // For sockaddr_in structure
#include <unistd.h>     // For close() function


int main(){
    int server_fd, new_socket;
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address);

    // Create socket
    server_fd = socket(AF_INET,SOCK_STREAM,0);
    if (server_fd == 0 ){
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }

    // Set socket 
    setsockopt(server_fd,SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt,sizeof(opt));

    // Configure Addresses
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(5000);

    // Bind the Socket
    bind(server_fd,(struct sockaddr *)&address, sizeof(address));

    // Listen
    listen(server_fd,3);

    // Accept a Connection
    new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen);


    // Send and receive data
    const char *hello = "Hello from server";
    send(new_socket, hello, strlen(hello), 0);
    std::cout << "Hello message sent" << std::endl;

    // char buffer[1024] = {0};
    // int valread = recv(new_socket, buffer, 1024, 0);
    // std::cout << "Message received: " << buffer << std::endl;

    // Close the sockets 
    close(new_socket);
    close(server_fd);

    return 0; 
}