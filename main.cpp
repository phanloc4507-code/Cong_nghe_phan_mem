#include <iostream>
#include <limits>
#include <string>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <unordered_map>
#include "Utils.h"
#include "Order.h"
#include "User.h"
#include "Cart/Cart.h"
#include "car/CarMenu.h"

namespace {
const std::string CAR_PATH = "car/data/cars.txt";
const std::string CATEGORY_PATH = "car/data/categories.txt";

int readInt(const std::string& prompt, int minValue, int maxValue) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= minValue && value <= maxValue) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cout << "Vui long nhap so tu " << minValue << " den " << maxValue << ".\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::string readLine(const std::string& prompt) {
    std::string value;
    std::cout << prompt;
    std::getline(std::cin >> std::ws, value);
    return value;
}

bool carIdToProductId(const std::string& carId, int& productId) {
    if (carId.size() < 2 || (carId[0] != 'C' && carId[0] != 'c')) return false;
    try {
        std::size_t used = 0;
        int value = std::stoi(carId.substr(1), &used);
        if (used != carId.size() - 1 || value <= 0) return false;
        productId = value;
        return true;
    } catch (...) { return false; }
}

std::string productIdToCarId(int productId) {
    std::ostringstream out;
    out << 'C' << std::setw(3) << std::setfill('0') << productId;
    return out.str();
}

void showCarAndCategoryMenu(CarManager& cars, CategoryManager& categories) {
    if (UserModule::isAdmin())
        adminCarMenu(cars, categories, CAR_PATH, CATEGORY_PATH);
    else
        guestCarMenu(cars, categories, CAR_PATH);
}

void showCart(const Cart& cart) {
    if (cart.empty()) {
        std::cout << "Gio hang dang trong.\n";
        return;
    }
    std::cout << "\n--- GIO HANG (so ngay thue) ---\n";
    for (const auto& item : cart.items()) {
        std::cout << productIdToCarId(item.productId) << " | " << item.productName
                  << " | " << item.unitPrice << " VND/ngay | " << item.quantity
                  << " ngay | Thanh tien: " << item.lineTotal() << " VND\n";
    }
    std::cout << "Tam tinh: " << cart.subtotal() << " VND\n";
}

void cartMenu(Cart& cart, CarManager& cars, const std::string& username) {
    int choice;
    do {
        std::cout << "\n=== GIO HANG & DAT XE ===\n"
                  << "1. Xem xe con san sang\n"
                  << "2. Them xe vao gio\n"
                  << "3. Xem gio hang\n"
                  << "4. Sua so ngay thue\n"
                  << "5. Xoa xe khoi gio\n"
                  << "6. Thanh toan / Tao don hang\n"
                  << "0. Quay lai\n";
        choice = readInt("Chon: ", 0, 6);

        if (choice == 1) {
            bool found = false;
            for (const Car& car : cars.all()) {
                if (car.isAvailable()) {
                    std::cout << car.id() << " | " << car.name() << " | "
                              << car.finalPrice() << " VND/ngay\n";
                    found = true;
                }
            }
            if (!found) std::cout << "Hien khong co xe san sang.\n";
        } else if (choice == 2) {
            const std::string id = readLine("Nhap ma xe (vi du C001): ");
            Car* car = cars.findById(id);
            if (!car) { std::cout << "Khong tim thay xe.\n"; continue; }
            if (!car->isAvailable()) { std::cout << "Xe khong o trang thai san sang.\n"; continue; }
            int productId;
            if (!carIdToProductId(car->id(), productId)) {
                std::cout << "Ma xe khong dung dinh dang C001.\n"; continue;
            }
            const int days = readInt("So ngay thue: ", 1, 365);
            if (cart.addItem(productId, car->name(), car->finalPrice(), days))
                std::cout << "Da them xe vao gio.\n";
            else std::cout << "Khong the them xe vao gio.\n";
        } else if (choice == 3) {
            showCart(cart);
        } else if (choice == 4 || choice == 5) {
            showCart(cart);
            if (cart.empty()) continue;
            int id;
            const std::string carId = readLine("Nhap ma xe can thao tac: ");
            if (!carIdToProductId(carId, id)) { std::cout << "Ma xe khong hop le.\n"; continue; }
            if (choice == 5) {
                std::cout << (cart.removeItem(id) ? "Da xoa khoi gio.\n" : "Xe khong co trong gio.\n");
            } else {
                const int days = readInt("Nhap so ngay moi (0 de xoa): ", 0, 365);
                std::cout << (cart.setQuantity(id, days) ? "Da cap nhat gio hang.\n" : "Xe khong co trong gio.\n");
            }
        } else if (choice == 6) {
            if (cart.empty()) { std::cout << "Gio hang dang trong.\n"; continue; }
            showCart(cart);
            std::cout << "1. Tien mat khi nhan xe\n2. Thanh toan online (mo phong)\n";
            const int paymentChoice = readInt("Chon phuong thuc: ", 1, 2);
            OrderDraft draft;
            ShippingPolicy rentalPolicy;
            rentalPolicy.baseFee = 0; // Dat thue xe khong tinh phi giao hang.
            rentalPolicy.freeShippingFrom = 0;
            const PaymentMethod method = paymentChoice == 1 ? PaymentMethod::Cash : PaymentMethod::Online;
            if (!cart.checkout(method, draft, rentalPolicy)) {
                std::cout << "Khong the checkout gio hang.\n"; continue;
            }
            bool created = false;
            for (const auto& item : draft.items) {
                const std::string carId = productIdToCarId(item.productId);
                Car* car = cars.findById(carId);
                if (!car || !car->isAvailable()) {
                    std::cout << "Bo qua " << carId << " vi xe khong con san sang.\n";
                    continue;
                }
                addOrder(username, carId, static_cast<double>(item.lineTotal()),
                         paymentMethodName(method));
                car->setStatus(CarStatus::Rented);
                car->increaseRentCount();
                created = true;
            }
            cars.save(CAR_PATH);
            if (created) std::cout << "Da tao don hang. Vao muc Quan ly don hang de theo doi.\n";
            else std::cout << "Khong tao duoc don hang nao.\n";
        }
    } while (choice != 0);
}

void syncCarStatusFromOrders(CarManager& cars) {
    // Trang thai don hang moi nhat cua moi xe duoc dung de dong bo trang thai xe.
    std::unordered_map<std::string, std::string> latestStatus;
    for (const auto& order : getAllOrders()) latestStatus[order.carId] = order.status;
    for (const auto& entry : latestStatus) {
        Car* car = cars.findById(entry.first);
        if (!car) continue;
        if (entry.second == "Pending" || entry.second == "Confirmed" || entry.second == "Delivering") {
            car->setStatus(CarStatus::Rented);
        } else if ((entry.second == "Cancelled" || entry.second == "Completed") && car->status() == CarStatus::Rented) {
            car->setStatus(CarStatus::Ready);
        }
    }
    cars.save(CAR_PATH);
}

void showMainMenu() {
    const UserModule::User* user = UserModule::currentUser();
    std::cout << "\n========================================\n"
              << "      HE THONG QUAN LY THUE XE\n"
              << "========================================\n";
    if (user) std::cout << "Tai khoan: " << user->email << " | Vai tro: " << user->role << '\n';
    else std::cout << "Trang thai: Guest (chua dang nhap)\n";
    std::cout << "1. Xem / Quan ly xe va loai xe\n"
              << "2. Tai khoan (dang ky / dang nhap / ho so)\n"
              << "3. Gio hang va dat xe (Member)\n"
              << "4. Quan ly don hang\n"
              << "0. Thoat\n";
}
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    CarManager cars;
    CategoryManager categories;
    Cart cart;
    std::string cartOwner;

    if (!categories.load(CATEGORY_PATH))
        std::cout << "Canh bao: khong doc duoc " << CATEGORY_PATH << '\n';
    if (!cars.load(CAR_PATH))
        std::cout << "Canh bao: khong doc duoc " << CAR_PATH << '\n';
    UserModule::loadUsersFromFile("users.txt");
    loadOrders();
    syncCarStatusFromOrders(cars);

    int choice;
    do {
        showMainMenu();
        choice = readInt("Chon chuc nang: ", 0, 4);
        switch (choice) {
            case 1:
                showCarAndCategoryMenu(cars, categories);
                break;
            case 2:
                UserModule::accountMenu();
                break;
            case 3: {
                const UserModule::User* user = UserModule::currentUser();
                if (!user || user->role != "Customer") {
                    std::cout << "Ban can dang nhap bang tai khoan Customer de dat xe.\n";
                    break;
                }
                if (cartOwner != user->email) {
                    cart.clear();
                    cartOwner = user->email;
                }
                cartMenu(cart, cars, user->email);
                break;
            }
            case 4: {
                const UserModule::User* user = UserModule::currentUser();
                if (!user) {
                    std::cout << "Vui long dang nhap truoc khi xem don hang.\n";
                    break;
                }
                if (user->role == "Admin") adminOrderMenu();
                else memberOrderMenu(user->email);
                syncCarStatusFromOrders(cars);
                break;
            }
            case 0:
                std::cout << "Dang luu du lieu va thoat...\n";
                cars.save(CAR_PATH);
                categories.save(CATEGORY_PATH);
                break;
        }
    } while (choice != 0);
    return 0;
}
