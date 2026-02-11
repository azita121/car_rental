#ifndef USER_H
#define USER_H

#include <string>

enum class UserType {
    Guest,
    Customer,
    Staff,
    Maintenance,
    Manager
};

class User {
protected:
    std::string username;
    std::string passwordHash;
    std::string fullName;
    std::string email;
    UserType type;
    bool isBlocked;
    double outstandingBalance;
    
public:
    User(const std::string& username, const std::string& password,
         const std::string& fullName, const std::string& email, UserType type);
    
    virtual ~User() = default;
    
    // Getters
    std::string getUsername() const { return username; }
    std::string getPasswordHash() const { return passwordHash; }
    std::string getFullName() const { return fullName; }
    std::string getEmail() const { return email; }
    UserType getType() const { return type; }
    bool getIsBlocked() const { return isBlocked; }
    double getOutstandingBalance() const { return outstandingBalance; }
    
    // Setters
    void setPassword(const std::string& password);
    void restorePasswordHash(const std::string& hash) { passwordHash = hash; }
    void setIsBlocked(bool blocked) { isBlocked = blocked; }
    void addToBalance(double amount) { outstandingBalance += amount; }
    void subtractFromBalance(double amount) { outstandingBalance -= amount; }
    
    // Authentication
    bool verifyPassword(const std::string& password) const;
    
    // Utility
    std::string getTypeString() const;
    void displayInfo() const;
};

// Guest class (no login required)
class Guest {
public:
    static std::string getTypeString() { return "Guest"; }
};

// Customer class
class Customer : public User {
public:
    Customer(const std::string& username, const std::string& password,
             const std::string& fullName, const std::string& email);
};

// Staff class
class Staff : public User {
public:
    Staff(const std::string& username, const std::string& password,
          const std::string& fullName, const std::string& email);
};

// Maintenance class
class Maintenance : public User {
public:
    Maintenance(const std::string& username, const std::string& password,
                const std::string& fullName, const std::string& email);
};

// Manager class
class Manager : public User {
public:
    Manager(const std::string& username, const std::string& password,
            const std::string& fullName, const std::string& email);
};

#endif // USER_H
