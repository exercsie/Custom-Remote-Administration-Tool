#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "menu.h"

#define BUFFERSIZE 4096
#define PORT 4444

int main() {
    sockaddr_in serverAddress{}, clientAddress{};
    socklen_t clientLength = sizeof(clientAddress);
    int serverFileDescripter; // 0 = standard input, 1= standard output, 2= standard error, 3 = socket created
    int clientFileDescriptor;
    char buffer[BUFFERSIZE];

    serverFileDescripter = socket(AF_INET, SOCK_STREAM, 0);
    if(serverFileDescripter < 0) {
        perror("Socket not created\n");
        return 1;
    } else if(serverFileDescripter == 3) {
        std::cout << "Socket created successfully\n";
    }

    serverAddress.sin_family = AF_INET; // IPv4
    serverAddress.sin_addr.s_addr = INADDR_ANY; // ip address to bind to
    serverAddress.sin_port = htons(PORT); // htons converts port to network byte order
    if(bind(serverFileDescripter, (sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
        perror("Bind failed\n");
        return 1;
    } else {
        std::cout << "Bind successful\n";
    }

    if(listen(serverFileDescripter, 1) < 0) {
        perror("Failed to find incoming connection requests\n");
        return 1;
    } else {
        std::cout << "Listening on port " << PORT << std::endl; 
    }

    clientFileDescriptor = accept(serverFileDescripter, (sockaddr*) &clientAddress, &clientLength);
    if(clientFileDescriptor < 0) {
        perror("Client connection failed\n");
    } else {
        std::cout << "Client connected\n";
    }

    while(true) {
        int choice = menu();

        if(choice == 1) {
            int type = TEXT;
            send(clientFileDescriptor, &type, sizeof(type), 0);

            std::cout << "Type /back to return to the menu.\n";

            while(true) {
                std::string cmd;
                std::cout << "Type here: ";
                std::getline(std::cin, cmd);

                if(cmd == "/back") {
                    int back = BACK;
                    send(clientFileDescriptor, &back, sizeof(back), 0);
                    break;
                }

                send(clientFileDescriptor, cmd.c_str(), cmd.size(), 0);

                memset(buffer, 0, BUFFERSIZE);
                recv(clientFileDescriptor, buffer, BUFFERSIZE, 0);

                std::cout << buffer << std::endl;
            }
        }

        if(choice == 3) {
            int type = EXIT;
            send(clientFileDescriptor, &type, sizeof(type), 0);
            break;
        }
    }

    return 0;
}
