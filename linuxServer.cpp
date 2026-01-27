#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUFFERSIZE 4096
#define PORT 4444

int main() {
    sockaddr_in serverAddress{};
    int serverFileDescripter; // 0 = standard input, 1= standard output, 2= standard error, 3 = socket created
    char buffer[BUFFERSIZE];

    serverFileDescripter = socket(AF_INET, SOCK_STREAM, 0);
    if(serverFileDescripter < 0) {
        perror("socket not created");
        return 1;
    } else if(serverFileDescripter == 3) {
        std::cout << "Socket created successfully\n";
    }

    serverAddress.sin_family = AF_INET; // IPv4
    serverAddress.sin_addr.s_addr = INADDR_ANY; // ip address to bind to
    serverAddress.sin_port = htons(PORT); // htons converts port to network byte order
    if(bind(serverFileDescripter, (sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
        perror("bind failed");
        return 1;
    } else {
        std::cout << "Bind successful";
    }

    return 0;
}
