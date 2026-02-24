#include <iostream>

extern int SHIFT;

void receiverKey(int key);
void caesarEncrypt(char *buf, size_t length);
void caesarDecrypt(char *buf, size_t length);