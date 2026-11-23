#include <iostream>
#include "Utils.h"
#include "FileManager.h"

void showMainMenu() {
    std::cout << "=== HE THONG QUAN LY THUE / BAN XE ===\n";
    std::cout << "1. Quan ly xe & loai xe\n";
    std::cout << "2. Quan ly tai khoan\n";
    std::cout << "3. Gio hang & dat xe\n";
    std::cout << "4. Quan ly don hang\n";
    std::cout << "0. Thoat\n";
}

int main() {
    int choice;
    do {
        Utils::clearScreen();
        showMainMenu();
        choice = Utils::getIntInput("Chon chuc nang: ");

        switch (choice) {
            case 1:
                // TODO: Goi module Car / Category (Nguoi 2)
                std::cout << "[Chuc nang Xe & Loai xe]\n";
                Utils::pauseScreen();
                break;
            case 2:
                // TODO: Goi module User (Nguoi 3)
                std::cout << "[Chuc nang Tai khoan]\n";
                Utils::pauseScreen();
                break;
            case 3:
                // TODO: Goi module Cart & Tao don (Nguoi 4)
                std::cout << "[Chuc nang Gio hang & Dat xe]\n";
                Utils::pauseScreen();
                break;
            case 4:
                // TODO: Goi module Quan ly Order (Nguoi 5)
                std::cout << "[Chuc nang Quan ly don hang]\n";
                Utils::pauseScreen();
                break;
            case 0:
                std::cout << "Dang thoat chuong trinh...\n";
                break;
            default:
                std::cout << "Lua chon khong hop le!\n";
                Utils::pauseScreen();
        }
    } while (choice != 0);

    return 0;
}