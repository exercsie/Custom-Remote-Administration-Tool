#include "info.h"
#include "..\..\Headers\menu.h"

std::string getSystemInfo() {
    std::stringstream info;

    char pcName[256];
    DWORD pcNameSize = sizeof(pcName);
    if(GetComputerNameA(pcName, &pcNameSize)) {
        info << SUCCESS_PREFIX << " Client Name: " << pcName << std::endl;
    }

    char pcUsername[256];
    DWORD pcUsernameSize = sizeof(pcUsername);
    if(GetUserNameA(pcUsername, &pcUsernameSize)) {
        info << SUCCESS_PREFIX << " Client Username: " << pcUsername << std::endl;
    }

    OSVERSIONINFOA osver;
    ZeroMemory(&osver, sizeof(OSVERSIONINFOA));
    osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);

    if(GetVersionExA(&osver)) {
        info << SUCCESS_PREFIX << " Client OS: Windows " << osver.dwMajorVersion << "." << osver.dwMinorVersion << std::endl;
    }

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    info << SUCCESS_PREFIX << " Client Architecture: ";
    switch(sysInfo.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64:
            info << "x64\n";
            break;
        case PROCESSOR_ARCHITECTURE_INTEL:
            info << "x86\n";
            break;
    }

    info << SUCCESS_PREFIX << " Client Processors: " << sysInfo.dwNumberOfProcessors << std::endl;

    MEMORYSTATUSEX memoryInfo;
    memoryInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if(GlobalMemoryStatusEx(&memoryInfo)) {
        info << SUCCESS_PREFIX << " Client Total RAM: " << (memoryInfo.ullTotalPhys / ( 1024 * 1024 * 1024)) << " GB\n";
        info << SUCCESS_PREFIX << " Client Avaliable RAM: " << (memoryInfo.ullAvailPhys / (1024 * 1024 * 1024)) << " GB\n";
    }

    return info.str();
}
