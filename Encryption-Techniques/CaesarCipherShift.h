#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <random>
#include <iostream>

std::random_device x;
std::mt19937 gen(x());
std::uniform_int_distribution<> dist(1, 26);

int randNum = dist(gen);

//int x = rand();
int SHIFT = randNum;

void receiverKey(int key) {
    SHIFT = key;
}

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