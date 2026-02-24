#include <iostream>
#include <cstdint>
#include <direct.h>
#include "receiveFile.h"
#include "..\..\Headers\menu.h"
#include "..\..\Headers\CaesarCipherShift.h"

std::string receiveFile(SOCKET sock, char* buffer) {
    int encryptKey, bytesRec;
    bytesRec = recv(sock, (char*)&encryptKey, sizeof(encryptKey), 0);
    if(bytesRec <= 0) {
        std::cout << ERROR_PREFIX << " Cannot receive encryption key\n";
        return "";
    } else {
        receiverKey(encryptKey);
        std::cout << SUCCESS_PREFIX << " Received encryption key: " << SHIFT << std::endl;
    }

    int64_t fileSize;
    bytesRec = recv(sock, (char*)&fileSize, sizeof(fileSize), 0);
    if(bytesRec < 0 || fileSize < 0) {
        std::cout << ERROR_PREFIX << " Cannot receive file information\n";
        return "";
    }

    int32_t fileNameLeng;
    bytesRec = recv(sock, (char*)&fileNameLeng, sizeof(fileNameLeng), 0);
    if (bytesRec <= 0) {
        std::cout << ERROR_PREFIX << " Cannot receive file name length\n";
        return "";
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
        perror("fopen error");
        return "";
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
    return path;
}