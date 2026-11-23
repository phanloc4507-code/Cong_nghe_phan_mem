#include "User.h"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

namespace UserModule {
namespace {
std::vector<User> userList;
int currentUserId = -1;
std::string userFile = "users.txt";

std::string hashPassword(const std::string& pass) {
    // Giu dinh dang tu module User ban dau. std::hash chi phu hop bai tap,
    // khong nen dung lam co che luu mat khau cho he thong thuc te.
    return std::to_string(std::hash<std::string>{}(pass));
}

std::string generateRandomPassword(int length = 8) {
    const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string result;
    for (int i = 0; i < length; ++i)
        result += chars[static_cast<std::size_t>(std::rand()) % chars.size()];
    return result;
}

User* findCurrent() {
    for (auto& user : userList)
        if (user.userID == currentUserId) return &user;
    return nullptr;
}

void saveUsersToFile() {
    std::ofstream file(userFile);
    if (!file) {
        std::cout << "[Loi] Khong the ghi file tai khoan: " << userFile << '\n';
        return;
    }
    for (const auto& u : userList) {
        file << u.userID << '|' << u.email << '|' << u.password << '|'
             << u.fullName << '|' << u.dateOfBirth << '|' << u.gender << '|'
             << u.address << '|' << u.phoneNumber << '|' << u.licenseNumber << '|'
             << u.role << '\n';
    }
}
}

void loadUsersFromFile(const std::string& path) {
    userFile = path;
    userList.clear();
    currentUserId = -1;
    std::ifstream file(userFile);
    if (!file) {
        User admin{1, "admin@gmail.com", hashPassword("admin123"), "Quan Tri Vien",
                   "01/01/1990", "Nam", "Ha Noi", "0900000000", "ADMIN001", "Admin"};
        userList.push_back(admin);
        saveUsersToFile();
        std::cout << "Da tao tai khoan Admin mac dinh: admin@gmail.com / admin123\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::vector<std::string> fields;
        std::string field;
        while (std::getline(ss, field, '|')) fields.push_back(field);
        if (fields.size() != 10) continue;
        try {
            User u;
            u.userID = std::stoi(fields[0]);
            u.email = fields[1]; u.password = fields[2]; u.fullName = fields[3];
            u.dateOfBirth = fields[4]; u.gender = fields[5]; u.address = fields[6];
            u.phoneNumber = fields[7]; u.licenseNumber = fields[8]; u.role = fields[9];
            userList.push_back(u);
        } catch (...) { /* bo qua dong du lieu khong hop le */ }
    }
    if (userList.empty()) {
        User admin{1, "admin@gmail.com", hashPassword("admin123"), "Quan Tri Vien",
                   "01/01/1990", "Nam", "Ha Noi", "0900000000", "ADMIN001", "Admin"};
        userList.push_back(admin);
        saveUsersToFile();
    }
}

bool registerAccount() {
    User u;
    std::string password, confirm;
    std::cout << "\n=== DANG KY TAI KHOAN ===\nEmail: ";
    std::getline(std::cin >> std::ws, u.email);
    if (u.email.empty() || u.email.find('@') == std::string::npos) {
        std::cout << "Email khong hop le.\n"; return false;
    }
    for (const auto& existing : userList) {
        if (existing.email == u.email) {
            std::cout << "Email da duoc dang ky.\n"; return false;
        }
    }
    std::cout << "Mat khau (it nhat 6 ky tu): "; std::getline(std::cin, password);
    if (password.size() < 6) { std::cout << "Mat khau qua ngan.\n"; return false; }
    std::cout << "Xac nhan mat khau: "; std::getline(std::cin, confirm);
    if (password != confirm) { std::cout << "Mat khau xac nhan khong khop.\n"; return false; }
    std::cout << "Ho va ten: "; std::getline(std::cin, u.fullName);
    std::cout << "So dien thoai: "; std::getline(std::cin, u.phoneNumber);
    if (u.fullName.empty() || u.phoneNumber.empty()) {
        std::cout << "Ho ten va so dien thoai khong duoc de trong.\n"; return false;
    }
    std::cout << "Ngay sinh (DD/MM/YYYY): "; std::getline(std::cin, u.dateOfBirth);
    std::cout << "Gioi tinh: "; std::getline(std::cin, u.gender);
    std::cout << "Dia chi: "; std::getline(std::cin, u.address);
    std::cout << "So GPLX/CCCD: "; std::getline(std::cin, u.licenseNumber);
    int maxId = 0;
    for (const auto& item : userList) if (item.userID > maxId) maxId = item.userID;
    u.userID = maxId + 1;
    u.password = hashPassword(password);
    u.role = "Customer"; // Khong cho nguoi dung tu dang ky quyen Admin.
    userList.push_back(u);
    saveUsersToFile();
    std::cout << "Dang ky tai khoan thanh cong.\n";
    return true;
}

bool loginUser() {
    std::string email, password;
    std::cout << "\n=== DANG NHAP ===\nEmail: ";
    std::getline(std::cin >> std::ws, email);
    std::cout << "Mat khau: "; std::getline(std::cin, password);
    const std::string hashed = hashPassword(password);
    for (const auto& u : userList) {
        if (u.email == email && u.password == hashed) {
            currentUserId = u.userID;
            std::cout << "Dang nhap thanh cong. Xin chao " << u.fullName
                      << " (" << u.role << ").\n";
            return true;
        }
    }
    std::cout << "Sai email hoac mat khau.\n";
    return false;
}

void logoutUser() {
    currentUserId = -1;
    std::cout << "Da dang xuat.\n";
}

const User* currentUser() { return findCurrent(); }
bool isLoggedIn() { return findCurrent() != nullptr; }
bool isAdmin() { const User* u = currentUser(); return u && u->role == "Admin"; }

void updateProfile() {
    User* u = findCurrent();
    if (!u) { std::cout << "Vui long dang nhap truoc.\n"; return; }
    std::cout << "\n=== THONG TIN CA NHAN ===\n"
              << "Email: " << u->email << "\nHo ten: " << u->fullName
              << "\nNgay sinh: " << u->dateOfBirth << "\nGioi tinh: " << u->gender
              << "\nDia chi: " << u->address << "\nSDT: " << u->phoneNumber
              << "\nGPLX/CCCD: " << u->licenseNumber << '\n';
    std::string input;
    std::cout << "Ho ten moi (Enter de giu nguyen): "; std::getline(std::cin >> std::ws, input);
    if (!input.empty()) u->fullName = input;
    std::cout << "Ngay sinh moi: "; std::getline(std::cin, input); if (!input.empty()) u->dateOfBirth = input;
    std::cout << "Gioi tinh moi: "; std::getline(std::cin, input); if (!input.empty()) u->gender = input;
    std::cout << "Dia chi moi: "; std::getline(std::cin, input); if (!input.empty()) u->address = input;
    std::cout << "SDT moi: "; std::getline(std::cin, input); if (!input.empty()) u->phoneNumber = input;
    std::cout << "GPLX/CCCD moi: "; std::getline(std::cin, input); if (!input.empty()) u->licenseNumber = input;
    saveUsersToFile();
    std::cout << "Da cap nhat ho so.\n";
}

void resetPassword() {
    std::string email;
    std::cout << "Email tai khoan: "; std::getline(std::cin >> std::ws, email);
    for (auto& u : userList) {
        if (u.email == email) {
            const std::string temp = generateRandomPassword();
            u.password = hashPassword(temp);
            saveUsersToFile();
            std::cout << "Mat khau tam (mo phong gui email): " << temp << '\n';
            return;
        }
    }
    std::cout << "Email khong ton tai.\n";
}

void manageUsersAdmin() {
    if (!isAdmin()) { std::cout << "Chi Admin duoc dung chuc nang nay.\n"; return; }
    int choice;
    do {
        std::cout << "\n=== QUAN LY NGUOI DUNG ===\n1. Xem danh sach\n2. Xoa tai khoan\n0. Quay lai\n";
        std::cout << "Chon: ";
        if (!(std::cin >> choice)) { std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); choice = -1; }
        else std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (choice == 1) {
            for (const auto& u : userList)
                std::cout << u.userID << " | " << u.email << " | " << u.fullName << " | " << u.role << '\n';
        } else if (choice == 2) {
            int id; std::cout << "ID can xoa: ";
            if (!(std::cin >> id)) { std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); continue; }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (id == currentUserId) { std::cout << "Khong the tu xoa tai khoan dang dang nhap.\n"; continue; }
            bool found = false;
            for (auto it = userList.begin(); it != userList.end(); ++it) {
                if (it->userID == id && it->role != "Admin") { userList.erase(it); found = true; break; }
            }
            if (found) { saveUsersToFile(); std::cout << "Da xoa tai khoan.\n"; }
            else std::cout << "Khong tim thay tai khoan hoac khong the xoa Admin.\n";
        }
    } while (choice != 0);
}

void accountMenu() {
    int choice;
    do {
        const User* u = currentUser();
        std::cout << "\n=== TAI KHOAN ===\n";
        if (u) std::cout << "Dang nhap: " << u->fullName << " (" << u->role << ")\n";
        else std::cout << "Chua dang nhap\n";
        std::cout << "1. Dang ky\n2. Dang nhap\n3. Quen mat khau\n4. Cap nhat ho so\n5. Quan ly nguoi dung (Admin)\n6. Dang xuat\n0. Quay lai\nChon: ";
        if (!(std::cin >> choice)) { std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); choice = -1; }
        else std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (choice) {
            case 1: registerAccount(); break;
            case 2: loginUser(); break;
            case 3: resetPassword(); break;
            case 4: updateProfile(); break;
            case 5: manageUsersAdmin(); break;
            case 6: logoutUser(); break;
            case 0: break;
            default: std::cout << "Lua chon khong hop le.\n";
        }
    } while (choice != 0);
}
}

}
