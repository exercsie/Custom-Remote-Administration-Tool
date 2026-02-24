#include "sendFile.h"
#include "../../Headers/menu.h"
#include <iostream>
#include <cstdint>
#include "../../Headers/CaesarCipherShift.h"

void sendFile(int clientFileDescriptor, const std::string &path) {
    FILE* file = fopen(path.c_str(), "rb");

    if(!file) {
        std::cout << ERROR_PREFIX << " Cannot send file" << std::endl;
        return;
    }

    fseek(file, 0, SEEK_END);
    int64_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    std::string fileName;
    size_t p = path.find_last_of("/\\");
    if (p != std::string::npos) {
        fileName = path.substr(p + 1);
    } else {
        fileName = path;
    }

    std::cout << PENDING_PREFIX << " Generating the encryption key...." << std::endl;
    send(clientFileDescriptor, &SHIFT, sizeof(SHIFT), 0);
    std::cout << SUCCESS_PREFIX << " Sent the encryption key: " << SHIFT << std::endl;
    std::cout << std::endl;

    send(clientFileDescriptor, &fileSize, sizeof(fileSize), 0);

    int32_t nameLength = fileName.size();
    send(clientFileDescriptor, &nameLength, sizeof(nameLength), 0);
    send(clientFileDescriptor, fileName.c_str(), nameLength, 0);

    char fileBuffer[BUFFERSIZE];
    int64_t dataSent = 0;
    std::cout << PENDING_PREFIX << " Encrypting the file: '" << fileName << "'...." << std::endl;
    while (dataSent < fileSize) {
        size_t readBytes = fread(fileBuffer, 1, BUFFERSIZE, file);

        // encryption
        caesarEncrypt(fileBuffer, readBytes);

        send(clientFileDescriptor, fileBuffer, readBytes, 0);
        dataSent += readBytes;
    }
    std::cout << SUCCESS_PREFIX << " File encrypted" << std::endl;
    std::cout << std::endl;
    std::cout << PENDING_PREFIX << " Sending the file: '" << fileName << "' of size " << fileSize << " bytes" << std::endl;
    fclose(file);
    std::cout << SUCCESS_PREFIX << " File sent" << std::endl;
}