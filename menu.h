#define PORT 4444
#define BUFFERSIZE 4096
#define SERVER_STATUS_CLOSED "\033[31m[ERROR]\033[0m Connection closed by server"
#define WSA_FAIL "\033[31m[ERROR]\033[0m WSAStartup failed\n"
#define SOCKET_FAIL "\033[31m[ERROR]\033[0m Socket failed to create\n"
#define CONNECTION_FAIL "\033[31m[ERROR]\033[0m Connection failed\n"
#define WSA_SUCCESS "\033[32m[SUCCESS]\033[0m WSAStartup created\n"
#define SOCKET_SUCCESS "\033[32m[SUCCESS]\033[0m Socket created\n"
#define INVALID_IP "\033[31m[ERROR]\033[0m Invalid ip address\n"
#define CONNECTION_ESTABLISHED "\033[32m[SUCCESS]\033[0m Connection established\n"
#define FILE_INFORMATION_FAIL "\033[31m[ERROR]\033[0m Cannot receive file information\n"
#define FILE_NAME_FAIL "\033[31m[ERROR]\033[0m Cannot retrieve file name length\n"
#define RECEIVED_FILE "\033[32m[SUCCESS]\033[0m Received file: "
#define SAVING_FILE "\033[32m[SUCCESS]\033[0m Saving file to: "
#define ATTACKER_LABEL "\033[31m[Attacker]\033[0m: "
#define ARGV_ERROR "\033[31m[ERROR]\033[0m Incorrect usage, please use: \033[32m./victim IP\033[0m"

enum MessageType {
    TYPE_TEXT = 1,
    TYPE_FILE = 2,
    TYPE_EXIT = 3,
    TYPE_BACK = 4
};

int menu();