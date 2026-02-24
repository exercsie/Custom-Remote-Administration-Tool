CLIENT_SRC = \
src\Client\Encryption\CaesarCipherShiftVictim.cpp \
src\Client\windowsVictim.cpp \
src\Client\System-Info\info.cpp \
src\Client\Receive-File\receiveFile.cpp

ifeq ($(OS),Windows_NT)
	CLIENT_FLAGS = -lws2_32 -lwinmm -static -static-libgcc -static-libstdc++
	RM = del /Q
else 
	RM = rm -f
endif

client:
	g++ $(CLIENT_SRC) -o src/Client/victim $(CLIENT_FLAGS)

SERVER_SRC = \
src/Server/linuxServer.cpp \
src/Server/Encryption/CaesarCipherShiftServer.cpp \
src/Server/Menu/menu.cpp \
src/Server/Send-File/sendFile.cpp

server:
	g++ $(SERVER_SRC) -o src/Server/kServer

clean:
	$(RM) victim server

