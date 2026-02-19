#include "info.h"
#include "..\menu.h"

std::string getSystemInfo() {
    std::stringstream info;

    char pcName[256];
    DWORD size = sizeof(pcName);
    if(GetComputerNameA(pcName, &size)) {
        info << SUCCESS_PREFIX << " Computer Name: " << pcName << std::endl;
    }

    return info.str();
}