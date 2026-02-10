#include <iostream>
#include "menu.h"

int menu() {
    int choice;

    std::cout << "1 - Send messages\n";
    std::cout << "2 - Send files\n";
    std::cout << "3 - Exit\n";

    std::cin >> choice;

    return choice;
}