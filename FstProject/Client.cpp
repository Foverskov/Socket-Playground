#include <iostream>
#include <cstring>      // For memset()
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in structure
#include <arpa/inet.h>
#include <unistd.h>     // For close() function
#include <fstream>

std::vector<char> decompressRLE(const std::vector<char>& compressed) {
    std::vector<char> decompressed;
    
    for (size_t i = 0; i < compressed.size(); i += 2) {
        if (i + 1 >= compressed.size()) break;
        
        unsigned char count = compressed[i];
        char value = compressed[i + 1];
        
        for (unsigned char j = 0; j < count; j++) {
            decompressed.push_back(value);
        }
    }
    return decompressed;
}

void receiveFile(int socket_fd, const std::string& saveFilePath){
    // Receive file size
    size_t fileSize = 0; 
    recv(socket_fd,&fileSize,sizeof(fileSize),0);
    std::cout << "receiving file of size: " << fileSize<< std::endl;

    //open file for writing 
    std::ofstream file(saveFilePath,std::ios::binary);
    if (!file){
        std::cerr << "Didnt open write file"<< std::endl;
        return;
    }

    // Init buffer the received data
    size_t buffersize = 4096;
    std::vector<char> buffer(buffersize);

    // Receive file chunks
    size_t totalReceived = 0; 
    while (totalReceived < fileSize){
        size_t bytesToReceive = std::min(buffersize,fileSize-totalReceived);
        size_t bytesReceived = 0; 
        while (bytesReceived < bytesToReceive){
            size_t result = recv(socket_fd,buffer.data()+bytesReceived,bytesToReceive-bytesReceived,0);
            if (result < 0){
                std::cerr << "Error receiving file chunks" << std::endl;
                return;
            }
            bytesReceived += result;
        }
        file.write(buffer.data(),bytesReceived);
        totalReceived += bytesReceived;
        std::cout << "Progress: " << totalReceived << "/" << fileSize << std::endl;
    }
    std::cout << "File received " << std::endl;
}   

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
    server_address.sin_port = htons(8080);

    if(connect(socket_fd,(struct sockaddr*)&server_address,sizeof(server_address))<0){
        std::cerr << "Failed connection"<<std::endl;
        return -1;
    }

    receiveFile(socket_fd,"/Users/foverskov/Documents/github/2_SoftwareAAU/IWP/TCPsocket/FstProject/recieve.mov");
    
    close(socket_fd);
    return 0; 
}

