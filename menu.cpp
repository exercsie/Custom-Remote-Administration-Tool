#include <iostream>
#include "menu.h"
#include <limits>

int menu() {
    int choice;

    std::cout << "1 - Send messages\n";
    std::cout << "2 - Send files\n";
    std::cout << "3 - Exit\n";

    std::cout << "\nChoose an option: ";
    std::cin >> choice;
    std::cout << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return choice;
}