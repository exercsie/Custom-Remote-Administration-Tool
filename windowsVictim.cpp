#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>

#define PORT 4444
#define BUFFERSIZE 4096

int main() {
    WSADATA wsa;
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
}