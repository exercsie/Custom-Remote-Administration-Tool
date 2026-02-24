#include <iostream>
#include <string>
#include <sys/socket.h>

void sendFile(int clientFileDescriptor, const std::string &path);