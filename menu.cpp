#include <iostream>
#include "menu.h"
#include <limits>

int menu() {
    int choice;

    while(true) {
        std::cout << "\n1 - Send messages\n";
        std::cout << "2 - Send files\n";
        std::cout << "3 - Exit\n";

        std::cout << "\nChoose an option: ";
        std::cin >> choice;
        std::cout << std::endl;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "\033[31m[ERROR]\033[0m Invalid usage, please enter a number between 1-3.\n";
            continue;
        } else if (!(choice >= 1 && choice <= 3)) {
            std::cout << "\033[31m[ERROR]\033[0m Invalid usage, please enter a number between 1-3.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        break;
    }
    return choice;
}