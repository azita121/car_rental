#include "User.h"
#include "Utils.h"
#include <iostream>

User::User(const std::string& username, const std::string& password,
           const std::string& fullName, const std::string& email, UserType type)
    : username(username), passwordHash(PasswordHasher::hash(password)),
      fullName(fullName), email(email), type(type), isBlocked(false), outstandingBalance(0.0) {}

void User::setPassword(const std::string& password) {
    passwordHash = PasswordHasher::hash(password);
}

bool User::verifyPassword(const std::string& password) const {
    return PasswordHasher::verify(password, passwordHash);
}

std::string User::getTypeString() const {
    switch (type) {
        case UserType::Guest: return "Guest";
        case UserType::Customer: return "Customer";
        case UserType::Staff: return "Staff";
        case UserType::Maintenance: return "Maintenance";
        case UserType::Manager: return "Manager";
        default: return "Unknown";
    }
}

void User::displayInfo() const {
    std::cout << "\n=== User Info ===" << std::endl;
    std::cout << "Username: " << username << std::endl;
    std::cout << "Full Name: " << fullName << std::endl;
    std::cout << "Email: " << email << std::endl;
    std::cout << "Type: " << getTypeString() << std::endl;
    std::cout << "Blocked: " << (isBlocked ? "Yes" : "No") << std::endl;
    std::cout << "Outstanding Balance: $" << outstandingBalance << std::endl;
    std::cout << "================\n" << std::endl;
}

Customer::Customer(const std::string& username, const std::string& password,
                   const std::string& fullName, const std::string& email)
    : User(username, password, fullName, email, UserType::Customer) {}

Staff::Staff(const std::string& username, const std::string& password,
             const std::string& fullName, const std::string& email)
    : User(username, password, fullName, email, UserType::Staff) {}

Maintenance::Maintenance(const std::string& username, const std::string& password,
                         const std::string& fullName, const std::string& email)
    : User(username, password, fullName, email, UserType::Maintenance) {}

Manager::Manager(const std::string& username, const std::string& password,
                 const std::string& fullName, const std::string& email)
    : User(username, password, fullName, email, UserType::Manager) {}
