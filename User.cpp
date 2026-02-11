#include "User.h"
#include "Utils.h"
#include <iostream>
using namespace std;


User::User(const string& username, const string& password,
           const string& fullName, const string& email, UserType type)
    : username(username), passwordHash(PasswordHasher::hash(password)),
      fullName(fullName), email(email), type(type), isBlocked(false), outstandingBalance(0.0) {}

void User::setPassword(const string& password) {
    passwordHash = PasswordHasher::hash(password);
}

bool User::verifyPassword(const string& password) const {
    return PasswordHasher::verify(password, passwordHash);
}

string User::getTypeString() const {
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
    cout << "\n=== User Info ===" << endl;
    cout << "Username: " << username << endl;
    cout << "Full Name: " << fullName << endl;
    cout << "Email: " << email << endl;
    cout << "Type: " << getTypeString() << endl;
    cout << "Blocked: " << (isBlocked ? "Yes" : "No") << endl;
    cout << "Outstanding Balance: $" << outstandingBalance << endl;
    cout << "================\n" << endl;
}

Customer::Customer(const string& username, const string& password,
                   const string& fullName, const string& email)
    : User(username, password, fullName, email, UserType::Customer) {}

Staff::Staff(const string& username, const string& password,
             const string& fullName, const string& email)
    : User(username, password, fullName, email, UserType::Staff) {}

Maintenance::Maintenance(const string& username, const string& password,
                         const string& fullName, const string& email)
    : User(username, password, fullName, email, UserType::Maintenance) {}

Manager::Manager(const string& username, const string& password,
                 const string& fullName, const string& email)
    : User(username, password, fullName, email, UserType::Manager) {}
