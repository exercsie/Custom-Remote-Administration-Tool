#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <iostream>

int x = rand();
const int SHIFT = x;

void caesarEncrypt(char *buf, size_t length) {
    for(size_t i = 0; i < length; i++) {
        buf[i] += SHIFT;
    }
}

void caesarDecrypt(char *buf, size_t length) {
    for(size_t i = 0; i < length; i++) {
        buf[i] -= SHIFT;
    }
}