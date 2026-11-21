// File chay thu rieng phan cua Nguoi 2 (khong can code cua nguoi khac).
// Khi ghep, Nguoi 1 bo file nay va goi guestCarMenu / adminCarMenu tu main.cpp.
#include "CarMenu.h"
#include <iostream>

int main() {
    const std::string carPath = "data/cars.txt", catPath = "data/categories.txt";
    CarManager cars;
    CategoryManager cats;
    if (!cats.load(catPath)) std::cout << "(chua co " << catPath << ", se tao moi khi luu)\n";
    if (!cars.load(carPath))  std::cout << "(chua co " << carPath << ", se tao moi khi luu)\n";

    while (true) {
        std::cout << "\n=== TEST PHAN XE (NGUOI 2) ===\n1) Menu Guest/Member (xem, tim xe)\n2) Menu Admin (quan ly xe)\n0) Thoat\nChon: ";
        std::string c;
        if (!std::getline(std::cin, c) || c == "0") break;
        if (c == "1") guestCarMenu(cars, cats, carPath);
        else if (c == "2") adminCarMenu(cars, cats, carPath, catPath);
    }
    return 0;
}
