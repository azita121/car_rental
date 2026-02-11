#ifndef USER_H
#define USER_H

#include <string>
using namespace std;


enum class UserType {
    Guest,
    Customer,
    Staff,
    Maintenance,
    Manager
};

class User {
protected:
    string username;
    string passwordHash;
    string fullName;
    string email;
    UserType type;
    bool isBlocked;
    double outstandingBalance;
    
public:
    User(const string& username, const string& password,
         const string& fullName, const string& email, UserType type);
    
    virtual ~User() = default;
    
    // Getters
    string getUsername() const { return username; }
    string getPasswordHash() const { return passwordHash; }
    string getFullName() const { return fullName; }
    string getEmail() const { return email; }
    UserType getType() const { return type; }
    bool getIsBlocked() const { return isBlocked; }
    double getOutstandingBalance() const { return outstandingBalance; }
    
    // Setters
    void setPassword(const string& password);
    void restorePasswordHash(const string& hash) { passwordHash = hash; }
    void setIsBlocked(bool blocked) { isBlocked = blocked; }
    void addToBalance(double amount) { outstandingBalance += amount; }
    void subtractFromBalance(double amount) { outstandingBalance -= amount; }
    
    // Authentication
    bool verifyPassword(const string& password) const;
    
    // Utility
    string getTypeString() const;
    void displayInfo() const;
};

// Guest class (no login required)
class Guest {
public:
    static string getTypeString() { return "Guest"; }
};

// Customer class
class Customer : public User {
public:
    Customer(const string& username, const string& password,
             const string& fullName, const string& email);
};

// Staff class
class Staff : public User {
public:
    Staff(const string& username, const string& password,
          const string& fullName, const string& email);
};

// Maintenance class
class Maintenance : public User {
public:
    Maintenance(const string& username, const string& password,
                const string& fullName, const string& email);
};

// Manager class
class Manager : public User {
public:
    Manager(const string& username, const string& password,
            const string& fullName, const string& email);
};

#endif // USER_H
