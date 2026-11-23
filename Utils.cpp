// Utils.cpp
#include "Utils.h"
#include <iostream>
#include <limits>

namespace Utils {
    int getIntInput(const std::string& prompt) {
        int value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            }
            std::cout << "Nhap sai dinh dang. Vui long nhap so!\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    std::string getStringInput(const std::string& prompt) {
        std::string value;
        std::cout << prompt;
        std::getline(std::cin, value);
        return value;
    }

    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    void pauseScreen() {
        std::cout << "\nNhan Enter de tiep tuc...";
        std::cin.get();
    }
}