#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "menu.h"
#include <unistd.h>

int main() {
    sockaddr_in serverAddress{}, clientAddress{};
    socklen_t clientLength = sizeof(clientAddress);
    int serverFileDescripter; // 0 = standard input, 1= standard output, 2= standard error, 3 = socket created
    int clientFileDescriptor;
    char buffer[BUFFERSIZE];

    serverFileDescripter = socket(AF_INET, SOCK_STREAM, 0);
    if(serverFileDescripter < 0) {
        std::cout << ERROR_PREFIX << " Socket failed to create\n";
        return 1;
    } else if(serverFileDescripter == 3) {
        std::cout << SUCCESS_PREFIX << " Socket created\n";
    }

    serverAddress.sin_family = AF_INET; // IPv4
    serverAddress.sin_addr.s_addr = INADDR_ANY; // ip address to bind to
    serverAddress.sin_port = htons(PORT); // htons converts port to network byte order
    if(bind(serverFileDescripter, (sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
        std::cout << ERROR_PREFIX << " Bind failed to create\n";
        perror(ERROR_PREFIX);
        return 1;
    } else {
        std::cout << SUCCESS_PREFIX << " Bind created\n";
    }

    if(listen(serverFileDescripter, 1) < 0) {
        std::cout << ERROR_PREFIX << " Failed to find incoming connection requests\n";
        return 1;
    } else {
        std::cout << PENDING_PREFIX << " Listening on port: " << PORT << std::endl; 
    }

    clientFileDescriptor = accept(serverFileDescripter, (sockaddr*) &clientAddress, &clientLength);
    if(clientFileDescriptor < 0) {
        std::cout << ERROR_PREFIX << " Connection failed\n";
    } else {
        std::cout << SUCCESS_PREFIX << " Connection established\n";
    }

    while(true) {
        int choice = menu();

        if(choice == TYPE_TEXT) {
            std::cout << CONSOLE_PREFIX << " Type /back to return to the menu.\n";

            while(true) {
                std::string cmd;
                std::cout << CONSOLE_PREFIX << " Type here: ";
                std::getline(std::cin, cmd);

                if(cmd == "/back") {
                    int back = TYPE_BACK;
                    send(clientFileDescriptor, &back, sizeof(back), 0);
                    break;
                }

                int msg = TYPE_TEXT;
                send(clientFileDescriptor, &msg, sizeof(msg), 0);
                send(clientFileDescriptor, cmd.c_str(), cmd.size(), 0);

                memset(buffer, 0, BUFFERSIZE);
                recv(clientFileDescriptor, buffer, BUFFERSIZE, 0);

                std::cout << buffer << std::endl;
            }
        }

        if(choice == TYPE_FILE) {           
            std::string path;
            std::cout << CONSOLE_PREFIX << " Enter path: ";
            std::getline(std::cin, path);

            FILE* file = fopen(path.c_str(), "rb");

            if(!file) {
                std::cout << ERROR_PREFIX << " Cannot send file\n";
                continue;
            }

            fseek(file, 0, SEEK_END);
            int64_t fileSize = ftell(file);
            fseek(file, 0, SEEK_SET);

            std::string fileName;
            size_t p = path.find_last_of("/\\");
            if (p != std::string::npos) {
                fileName = path.substr(p + 1);
            } else {
                fileName = path;
            }

            std::cout << PENDING_PREFIX << " Sending the file: '" << fileName << "'" << std::endl;

            int type = TYPE_FILE;
            send(clientFileDescriptor, &type, sizeof(type), 0);
            send(clientFileDescriptor, &fileSize, sizeof(fileSize), 0);

            int32_t nameLength = fileName.size();
            send(clientFileDescriptor, &nameLength, sizeof(nameLength), 0);
            send(clientFileDescriptor, fileName.c_str(), nameLength, 0);

            char fileBuffer[BUFFERSIZE];
            int64_t dataSent = 0;
            while (dataSent < fileSize) {
                size_t readBytes = fread(fileBuffer, 1, BUFFERSIZE, file);
                send(clientFileDescriptor, fileBuffer, readBytes, 0);
                dataSent += readBytes;
            }
            fclose(file);
            std::cout << SUCCESS_PREFIX << " Sent the file: '" << fileName << "' " << fileSize << " bytes\n";
        }

        if(choice == TYPE_EXIT) {
            int type = TYPE_EXIT;
            send(clientFileDescriptor, &type, sizeof(type), 0);
            break;
        }
    }

    return 0;
}
