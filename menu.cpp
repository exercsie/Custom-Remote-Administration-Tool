#include <iostream>
#include "menu.h"
#include <limits>

int menu() {
    int choice;

    while(true) {
        std::cout << "\n" << CONSOLE_PREFIX << " Elenchos RAT tool (C++ 1.3)\n" << std::endl;
        std::cout << CONSOLE_PREFIX << " 1 - Send messages\n";
        std::cout << CONSOLE_PREFIX << " 2 - Send files\n";
        std::cout << CONSOLE_PREFIX << " 3 - View system information\n";
        std::cout << CONSOLE_PREFIX << " 4 - Exit\n" << std::endl;

        std::cout << CONSOLE_PREFIX << " Choose an option: ";
        std::cin >> choice;
        std::cout << std::endl;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << ERROR_PREFIX << " Invalid usage, please enter a number between 1-4.\n";
            continue;
        } else if (!(choice >= 1 && choice <= 4)) {
            std::cout << ERROR_PREFIX << " Invalid usage, please enter a number between 1-4.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        break;
    }
    return choice;
}