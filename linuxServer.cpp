#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFERSIZE 4096
#define PORT 4444

int main() {

    int serverFileDescripter; // 0 = standard input, 1= standard output, 2= standard error
    char buffer[BUFFERSIZE];

    serverFileDescripter = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFileDescripter < 0) {
        perror("socket");
        return 1;
    }

    return 0;
}
