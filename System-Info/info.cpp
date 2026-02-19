#include "info.h"
#include "..\menu.h"

std::string getSystemInfo() {
    std::stringstream info;

    char pcName[256];
    DWORD size = sizeof(pcName);
    if(GetComputerNameA(pcName, &size)) {
        info << SUCCESS_PREFIX << " Computer Name: " << pcName << std::endl;
    }

    OSVERSIONINFOA osver;
    ZeroMemory(&osver, sizeof(OSVERSIONINFOA));
    osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);

    if(GetVersionExA(&osver)) {
        info << SUCCESS_PREFIX << " OS: Windows " << osver.dwMajorVersion << std::endl;
    }

    return info.str();
}
