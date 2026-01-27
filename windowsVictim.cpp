#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>

#define PORT 4444
#define BUFFERSIZE 4096
#define SERVER_STATUS_CLOSED "Connection closed by server"

int main() {
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
    inet_pton(AF_INET, "192.168.56.102", &serverAddress.sin_addr);

    if(connect(sock, (sockaddr*) &serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        perror("Connection failed\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    } else {
        std::cout << "Connection established\n";
    }

    while(true) {
        memset(buffer, 0, BUFFERSIZE);
        int bytesRec = recv(sock, buffer, BUFFERSIZE, 0);
        if(bytesRec <= 0) {
            std::cout << SERVER_STATUS_CLOSED;
            break;
        }

        std::string cmd(buffer);
        std::cout << "Attacker: " << cmd << std::endl;

        if(cmd == "exit") {
            std::cout << SERVER_STATUS_CLOSED;
            break;
        }

        std::string clientMessage = "Client received: " + cmd;
        send(sock, clientMessage.c_str(), clientMessage.size(), 0);
    }
    closesocket(sock);
    WSACleanup;
    return 0;
}