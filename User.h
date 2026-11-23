#ifndef USER_H
#define USER_H

#include <string>

namespace UserModule {
struct User {
    int userID{};
    std::string email;
    std::string password;
    std::string fullName;
    std::string dateOfBirth;
    std::string gender;
    std::string address;
    std::string phoneNumber;
    std::string licenseNumber;
    std::string role; // Customer hoac Admin
};

void loadUsersFromFile(const std::string& path = "users.txt");
bool registerAccount();
bool loginUser();
void logoutUser();
void updateProfile();
void resetPassword();
void manageUsersAdmin();
const User* currentUser();
bool isLoggedIn();
bool isAdmin();
void accountMenu();
}

#endif

