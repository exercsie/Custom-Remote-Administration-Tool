#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>
#include <cstdint>
#include "menu.h"
#include <direct.h>

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << ARGV_ERROR << std::endl;
        return 1;
    }
    sockaddr_in serverAddress;
    WSADATA wsa;
    char buffer[BUFFERSIZE];
    SOCKET sock = INVALID_SOCKET;

    if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0) { //ver 2.2
        perror(WSA_FAIL);
        return 1;
    } else {
        std::cout << WSA_SUCCESS;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == INVALID_SOCKET) {
        perror(SOCKET_FAIL);
        WSACleanup();
        return 1;
    } else {
        std::cout << SOCKET_SUCCESS;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    
    std::string serverip = argv[1];

    if(inet_pton(AF_INET, serverip.c_str(), &serverAddress.sin_addr) <= 0) {
        std::cout << INVALID_IP;
        return 1;
    }

    if(connect(sock, (sockaddr*) &serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cout << CONNECTION_FAIL;
        closesocket(sock);
        WSACleanup();
        return 1;
    } else {
        std::cout << CONNECTION_ESTABLISHED;
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

            std::string clientMessage = "Client received: " + cmd + '\n';
            send(sock, clientMessage.c_str(), clientMessage.size(), 0);
        
        }
        
        if(type == TYPE_FILE) {
            int64_t fileSize;
            bytesRec = recv(sock, (char*)&fileSize, sizeof(fileSize), 0);
            if(bytesRec < 0 || fileSize < 0) {
                std::cout << FILE_INFORMATION_FAIL;
                continue;
            }

            int32_t fileNameLeng;
            bytesRec = recv(sock, (char*)&fileNameLeng, sizeof(fileNameLeng), 0);
            if (bytesRec <= 0) {
                std::cout << FILE_NAME_FAIL;
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
            std::cout << RECEIVED_FILE << filename << std::endl;
            std::cout << SAVING_FILE << path << std::endl;

            FILE* fileOutput = fopen(path.c_str(), "wb");
            if(!fileOutput) {
                std::cout << "\033[31mCannot create file\033[0m\n";
                continue;
            }
        
            int64_t dataRec = 0;
            while (dataRec < fileSize) {
                int receive = (fileSize - dataRec > BUFFERSIZE) ? BUFFERSIZE : (fileSize - dataRec);
                bytesRec = recv(sock, buffer, receive, 0);
                if (bytesRec <= 0) {
                    break;
                }
                fwrite(buffer, 1, bytesRec, fileOutput);
                dataRec += bytesRec;
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