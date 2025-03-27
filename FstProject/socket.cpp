#include <iostream>
#include <cstring>      // For memset()
#include <sys/socket.h> // For socket functions
#include <arpa/inet.h>
#include <netinet/in.h> // For sockaddr_in structure
#include <unistd.h>     // For close() function
#include <fstream>
#include <vector>

std::vector<char> compressRLE(const std::vector<char>& data){
    std::vector<char> compressed;
    size_t i = 0;
    
    while (i < data.size()) {
        char current = data[i];
        unsigned char count = 1;
        
        while (i + 1 < data.size() && data[i + 1] == current && count < 255) {
            count++;
            i++;
        }
        compressed.push_back(count);
        compressed.push_back(current);
        i++;
    }
    return compressed;
}

void sendFile(int socket_fd, const std::string& filepath){
    std::ifstream file(filepath, std::ios::binary);
    if(!file){
        std::cerr <<"File not opened: " << filepath << std::endl;
        return;
    }

    file.seekg(0,std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0,std::ios::beg);

    std::vector<char> fileData(fileSize);
    file.read(fileData.data(),fileSize);

    std::vector<char> compressed();

    send(socket_fd,&fileSize,sizeof(fileSize),0);

    const size_t buffersize = 4096;
    std::vector<char> buffer(buffersize);

    size_t totalSent = 0; 
    while (totalSent < fileSize){
        size_t bytesToRead = std::min(buffersize,fileSize - totalSent);
        file.read(buffer.data(),bytesToRead);
        size_t bytesRead = file.gcount();

        size_t bytesSent = 0; 
        while(bytesSent < bytesRead){
            ssize_t result = send(socket_fd,buffer.data(),bytesRead-bytesSent,0);
            if (result<0){
                std::cerr << "Failed sending packet" << std::endl;
                return;
            }
            bytesSent += result;
        }
        totalSent += bytesRead;
        std::cout << "Progress: " << totalSent << std::endl;
    }   
    std::cout << "file sent succes" << std::endl;
}

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
    std::cout<<"Socket Created with fd: " << server_fd << std::endl;

    // Set socket 
    setsockopt(server_fd,SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt,sizeof(opt));

    // Configure Addresses
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(8080);

    // Bind the Socket
    bind(server_fd,(struct sockaddr *)&address, sizeof(address));

    // Listen
    listen(server_fd,3);

    // Accept a Connection
    std::cout << "Waiting for connections..." << std::endl;
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        std::cerr << "Connection accept failed" << std::endl;
        close(server_fd);
        return -1;
    }

    // Get client information
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &address.sin_addr, client_ip, INET_ADDRSTRLEN);
    std::cout << "Connection established with client " << client_ip 
              << ":" << ntohs(address.sin_port) << std::endl;

    // Send and receive data
    // const char *hello = "Hello from server";
    // send(new_socket, hello, strlen(hello), 0);
    // std::cout << "Hello message sent" << std::endl;

    sendFile(new_socket,"/Users/foverskov/Desktop/IMG_0245.mov");

    // char buffer[1024] = {0};
    // int valread = recv(new_socket, buffer, 1024, 0);
    // std::cout << "Message received: " << buffer << std::endl;

    // Close the sockets 
    close(new_socket);
    close(server_fd);

    return 0; 
}

