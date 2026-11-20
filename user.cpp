#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>

using namespace std;

// Cau truc du lieu nguoi dung theo Class Diagram & Data Model
struct User {
    int userID;
    string email;
    string password;
    string fullName;
    string dateOfBirth;
    string gender;
    string address;
    string phoneNumber;
    string licenseNumber;
    string role; // "Customer" hoac "Admin"
};

vector<User> userList;
User* currentUser = nullptr; // Luu tai khoan dang dang nhap

// --- CAC HAM HO TRO & LUU FILE ---

// Ma hoa mat khau don gian (theo yeu cau bao mat)
string hashPassword(const string& pass) {
    hash<string> hasher;
    return to_string(hasher(pass));
}

// Tao mat khau ngau nhien 8 ky tu cho chuc nang Reset Password
string generateRandomPassword(int length = 8) {
    const string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    string newPass = "";
    for (int i = 0; i < length; ++i) {
        newPass += chars[rand() % chars.length()];
    }
    return newPass;
}

// Luu danh sach tai khoan ra file users.txt
void saveUsersToFile() {
    ofstream file("users.txt");
    if (!file.is_open()) return;
    for (const auto& u : userList) {
        file << u.userID << "|" << u.email << "|" << u.password << "|"
             << u.fullName << "|" << u.dateOfBirth << "|" << u.gender << "|"
             << u.address << "|" << u.phoneNumber << "|" << u.licenseNumber << "|"
             << u.role << "\n";
    }
    file.close();
}

// Doc danh sach tai khoan tu file users.txt
void loadUsersFromFile() {
    userList.clear();
    ifstream file("users.txt");
    if (!file.is_open()) {
        // Tao san 1 tai khoan Admin mac dinh neu chua co file du lieu
        User admin = {1, "admin@gmail.com", hashPassword("admin123"), "Quan Tri Vien", "01/01/1990", "Nam", "Ha Noi", "0900000000", "ADMIN001", "Admin"};
        userList.push_back(admin);
        saveUsersToFile();
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string item;
        vector<string> tokens;
        while (getline(ss, item, '|')) {
            tokens.push_back(item);
        }
        if (tokens.size() >= 10) {
            User u;
            u.userID = stoi(tokens[0]);
            u.email = tokens[1];
            u.password = tokens[2];
            u.fullName = tokens[3];
            u.dateOfBirth = tokens[4];
            u.gender = tokens[5];
            u.address = tokens[6];
            u.phoneNumber = tokens[7];
            u.licenseNumber = tokens[8];
            u.role = tokens[9];
            userList.push_back(u);
        }
    }
    file.close();
}

// --- CAC CHUC NANG CHINH CUA MODULE USER ---

// 1. Dang ky tai khoan (Use-case Register account)
void registerAccount() {
    User u;
    string confirmPass, rawPass;
    cout << "\n=== DANG KY TAI KHOAN ===\n";
    cout << "Nhap Email (ten dang nhap): ";
    cin >> u.email;

    // Kiem tra trung email
    for (const auto& existing : userList) {
        if (existing.email == u.email) {
            cout << "[Loi] Email nay da duoc dang ky! Vui long dung email khac.\n";
            return;
        }
    }

    cout << "Nhap Mat khau (toi thieu 6 ky tu): ";
    cin >> rawPass;
    if (rawPass.length() < 6) {
        cout << "[Loi] Mat khau phai co it nhat 6 ky tu!\n";
        return;
    }
    cout << "Xac nhan lai Mat khau: ";
    cin >> confirmPass;
    if (rawPass != confirmPass) {
        cout << "[Loi] Mat khau xac nhan khong khop!\n";
        return;
    }
    u.password = hashPassword(rawPass);

    cin.ignore();
    cout << "Ho va ten (*): ";
    getline(cin, u.fullName);
    cout << "So dien thoai (*): ";
    getline(cin, u.phoneNumber);

    if (u.fullName.empty() || u.phoneNumber.empty()) {
        cout << "[Loi] Ho ten va So dien thoai khong duoc de trong!\n";
        return;
    }

    cout << "Ngay sinh (DD/MM/YYYY): ";
    getline(cin, u.dateOfBirth);
    cout << "Gioi tinh (Nam/Nu): ";
    getline(cin, u.gender);
    cout << "Dia chi: ";
    getline(cin, u.address);
    cout << "So GPLX / CCCD (co the bo qua, bo sung khi dat xe): ";
    getline(cin, u.licenseNumber);

    u.userID = userList.empty() ? 1 : userList.back().userID + 1;
    u.role = "Customer";

    userList.push_back(u);
    saveUsersToFile();
    cout << ">> Dang ky tai khoan thanh cong!\n";
}

// 2. Dang nhap (Use-case Login)
bool loginUser() {
    string email, rawPass;
    cout << "\n=== DANG NHAP ===\n";
    cout << "Email: ";
    cin >> email;
    cout << "Mat khau: ";
    cin >> rawPass;

    string hashed = hashPassword(rawPass);
    for (auto& u : userList) {
        if (u.email == email && u.password == hashed) {
            currentUser = &u;
            cout << ">> Dang nhap thanh cong! Xin chao, " << u.fullName << " (" << u.role << ").\n";
            return true;
        }
    }
    cout << "[Loi] Sai email hoac mat khau!\n";
    return false;
}

// 3. Xem & Cap nhat thong tin ca nhan (Use-case Update profile)
void updateProfile() {
    if (currentUser == nullptr) {
        cout << "[Loi] Vui long dang nhap truoc!\n";
        return;
    }

    cout << "\n=== THONG TIN CA NHAN ===\n";
    cout << "ID: " << currentUser->userID << " | Vai tro: " << currentUser->role << "\n";
    cout << "Email (Khong the sua): " << currentUser->email << "\n";
    cout << "Ho ten: " << currentUser->fullName << "\n";
    cout << "Ngay sinh: " << currentUser->dateOfBirth << "\n";
    cout << "Gioi tinh: " << currentUser->gender << "\n";
    cout << "Dia chi: " << currentUser->address << "\n";
    cout << "So dien thoai: " << currentUser->phoneNumber << "\n";
    cout << "So GPLX/CCCD: " << currentUser->licenseNumber << "\n";

    cout << "\nBan co muon cap nhat thong tin khong? (y/n): ";
    char ans;
    cin >> ans;
    if (ans != 'y' && ans != 'Y') return;

    cin.ignore();
    string input;
    cout << "Ho ten moi (Enter de giu nguyen): ";
    getline(cin, input);
    if (!input.empty()) currentUser->fullName = input;

    cout << "Ngay sinh moi (Enter de giu nguyen): ";
    getline(cin, input);
    if (!input.empty()) currentUser->dateOfBirth = input;

    cout << "Gioi tinh moi (Enter de giu nguyen): ";
    getline(cin, input);
    if (!input.empty()) currentUser->gender = input;

    cout << "Dia chi moi (Enter de giu nguyen): ";
    getline(cin, input);
    if (!input.empty()) currentUser->address = input;

    cout << "So dien thoai moi (Enter de giu nguyen): ";
    getline(cin, input);
    if (!input.empty()) currentUser->phoneNumber = input;

    cout << "So GPLX/CCCD moi (Enter de giu nguyen): ";
    getline(cin, input);
    if (!input.empty()) currentUser->licenseNumber = input;

    cout << "Ban co muon doi mat khau khong? (y/n): ";
    cin >> ans;
    if (ans == 'y' || ans == 'Y') {
        string newPass, confirmPass;
        cout << "Nhap mat khau moi (>= 6 ky tu): ";
        cin >> newPass;
        if (newPass.length() < 6) {
            cout << "[Loi] Mat khau qua ngan! Huy doi mat khau.\n";
        } else {
            cout << "Xac nhan mat khau moi: ";
            cin >> confirmPass;
            if (newPass == confirmPass) {
                currentUser->password = hashPassword(newPass);
                cout << ">> Doi mat khau thanh cong!\n";
            } else {
                cout << "[Loi] Mat khau xac nhan khong khop!\n";
            }
        }
    }

    saveUsersToFile();
    cout << ">> Da luu thong tin cap nhat!\n";
}

// 4. Quen mat khau (Use-case Reset password)
void resetPassword() {
    string email;
    cout << "\n=== KHOI PHUC MAT KHAU ===\n";
    cout << "Nhap Email tai khoan: ";
    cin >> email;

    for (auto& u : userList) {
        if (u.email == email) {
            string tempPass = generateRandomPassword(8);
            u.password = hashPassword(tempPass);
            saveUsersToFile();
            cout << ">> Da khoi phuc mat khau va gui toi email " << email << ".\n";
            cout << ">> [He thong gui mail] Mat khau moi cua ban la: " << tempPass << "\n";
            return;
        }
    }
    cout << "[Loi] Email khong ton tai trong he thong!\n";
}

// 5. Quan ly nguoi dung danh cho Admin (Use-case Manage user)
void manageUsersAdmin() {
    if (currentUser == nullptr || currentUser->role != "Admin") {
        cout << "[Loi] Chuc nang nay chi danh cho Admin!\n";
        return;
    }

    int choice;
    do {
        cout << "\n=== QUAN LY NGUOI DUNG (ADMIN) ===\n";
        cout << "1. Xem danh sach tat ca tai khoan\n";
        cout << "2. Them tai khoan moi\n";
        cout << "3. Xoa tai khoan theo ID\n";
        cout << "0. Quay lai\n";
        cout << "Chon: ";
        cin >> choice;

        if (choice == 1) {
            cout << "\n--- DANH SACH TAI KHOAN ---\n";
            for (const auto& u : userList) {
                cout << "ID: " << u.userID << " | Email: " << u.email 
                     << " | Ten: " << u.fullName << " | SDT: " << u.phoneNumber 
                     << " | Vai tro: " << u.role << "\n";
            }
        } else if (choice == 2) {
            registerAccount();
        } else if (choice == 3) {
            int delID;
            cout << "Nhap ID tai khoan can xoa: ";
            cin >> delID;
            if (delID == currentUser->userID) {
                cout << "[Loi] Khong the tu xoa tai khoan Admin dang dang nhap!\n";
                continue;
            }
            bool found = false;
            for (auto it = userList.begin(); it != userList.end(); ++it) {
                if (it->userID == delID) {
                    userList.erase(it);
                    saveUsersToFile();
                    cout << ">> Da xoa tai khoan thanh cong!\n";
                    found = true;
                    break;
                }
            }
            if (!found) cout << "[Loi] Khong tim thay ID!\n";
        }
    } while (choice != 0);
}

// Menu chay thu nghiem module User
int main() {
    srand(time(0));
    loadUsersFromFile();

    int choice;
    do {
        cout << "\n==============================\n";
        cout << "   HE THONG QUAN LY TAI KHOAN \n";
        if (currentUser != nullptr) {
            cout << "   [Dang nhap: " << currentUser->fullName << " - " << currentUser->role << "]\n";
        }
        cout << "==============================\n";
        cout << "1. Dang ky tai khoan\n";
        cout << "2. Dang nhap\n";
        cout << "3. Quen mat khau\n";
        cout << "4. Xem / Cap nhat ho so ca nhan\n";
        cout << "5. Quan ly nguoi dung (Admin)\n";
        cout << "6. Dang xuat\n";
        cout << "0. Thoat\n";
        cout << "Nhap lua chon: ";
        cin >> choice;

        switch (choice) {
            case 1: registerAccount(); break;
            case 2: loginUser(); break;
            case 3: resetPassword(); break;
            case 4: updateProfile(); break;
            case 5: manageUsersAdmin(); break;
            case 6: 
                currentUser = nullptr; 
                cout << ">> Da dang xuat!\n"; 
                break;
            case 0: cout << "Ket thuc chuong trinh.\n"; break;
            default: cout << "Lua chon khong hop le!\n";
        }
    } while (choice != 0);

    return 0;
}