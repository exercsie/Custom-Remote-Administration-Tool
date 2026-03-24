# Elenchos RAT Tool C++
A custom Remote Administration Tool written in C++, developed and tested in a controlled environment for educational purposes.

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Build](#build)
- [Usage](#usage)

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
#### Open a folder
Allows an Attacker to open a folder by typing in the file path, for example: "C:\Users\"

#### Execute a File
All attempted executions are defaulted to the same path as where the victim's .exe is saved. So when executing a file all you have to do is type the file name, which should be done after you inject the file. 
For example:
1. Use File transfer to inject test.txt
2. test.txt is saved to the same path the .exe is saved to
3. Choose Execute commands
4. Choose Execute a file
5. Type the file you injected to open it on the victim's computer

## Build


## Usage
