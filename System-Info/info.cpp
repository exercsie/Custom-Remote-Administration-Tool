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

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    info << SUCCESS_PREFIX << " Client Architecture: ";
    switch(sysInfo.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64:
            info << "AMD x64\n";
            break;
        case PROCESSOR_ARCHITECTURE_INTEL:
            info << "INTEL x86\n";
    }

    info << SUCCESS_PREFIX << " Client has " << sysInfo.dwNumberOfProcessors << " processors\n";

    MEMORYSTATUSEX memoryInfo;
    memoryInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if(GlobalMemoryStatusEx(&memoryInfo)) {
        info << SUCCESS_PREFIX << " Total RAM: " << (memoryInfo.ullTotalPhys / ( 1024 * 1024 * 1024)) << " GB\n";
        info << SUCCESS_PREFIX << " Avaliable RAM: " << (memoryInfo.ullAvailPhys / (1024 * 1024 * 1024)) << " GB\n";
    }

    return info.str();
}
