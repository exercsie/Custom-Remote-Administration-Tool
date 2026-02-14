#define PORT 4444
#define BUFFERSIZE 4096
#define SERVER_STATUS_CLOSED "\033[31m[ERROR]\033[0m Connection closed by server"
#define ERROR_PREFIX "\033[31m[ERROR]\033[0m"
#define SUCCESS_PREFIX "\033[32m[SUCCESS]\033[0m"
#define PENDING_PREFIX "\033[33m[PENDING]\033[0m"
#define CONSOLE_PREFIX "\033[37m[CONSOLE]\033[0m"
#define ATTACKER_LABEL "\033[31m[Attacker]\033[0m: "

enum MessageType {
    TYPE_TEXT = 1,
    TYPE_FILE = 2,
    TYPE_EXIT = 3,
    TYPE_BACK = 4
};

int menu();