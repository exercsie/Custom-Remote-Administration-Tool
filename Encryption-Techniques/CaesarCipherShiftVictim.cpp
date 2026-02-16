#include "Encryption-Techniques/CaesarCipherShift.h"

int SHIFT = 0;

void receiverKey(int key) {
    SHIFT = key;
}

void caesarDecrypt(char *buf, size_t length) {
    for(size_t i = 0; i < length; i++) {
        buf[i] -= SHIFT;
    }
}