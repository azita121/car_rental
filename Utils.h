#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <functional>

// Password hashing using simple hash function
class PasswordHasher {
public:
    static std::string hash(const std::string& password) {
        std::hash<std::string> hasher;
        size_t hashValue = hasher(password);
        std::stringstream ss;
        ss << std::hex << hashValue;
        return ss.str();
    }
    
    static bool verify(const std::string& password, const std::string& hash) {
        return hash == PasswordHasher::hash(password);
    }
};

// Date handling utilities
class DateUtils {
public:
    static bool isValidDate(const std::string& date) {
        std::tm tm = {};
        std::istringstream ss(date);
        ss >> std::get_time(&tm, "%Y-%m-%d");
        return !ss.fail();
    }
    
    static std::string getCurrentDate() {
        std::time_t now = std::time(0);
        std::tm* timeinfo = std::localtime(&now);
        char buffer[11];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
        return std::string(buffer);
    }
    
    static int daysBetween(const std::string& date1, const std::string& date2) {
        std::tm tm1 = {}, tm2 = {};
        std::istringstream ss1(date1), ss2(date2);
        ss1 >> std::get_time(&tm1, "%Y-%m-%d");
        ss2 >> std::get_time(&tm2, "%Y-%m-%d");
        
        if (ss1.fail() || ss2.fail()) return 0;
        
        std::time_t time1 = std::mktime(&tm1);
        std::time_t time2 = std::mktime(&tm2);
        
        return static_cast<int>(std::difftime(time2, time1) / (60 * 60 * 24));
    }
    
    static bool isDateAfter(const std::string& date1, const std::string& date2) {
        return daysBetween(date1, date2) > 0;
    }
    
    static bool isDateBefore(const std::string& date1, const std::string& date2) {
        return daysBetween(date1, date2) < 0;
    }
    
    static bool isDateEqual(const std::string& date1, const std::string& date2) {
        return daysBetween(date1, date2) == 0;
    }
    
    static std::string addDays(const std::string& date, int days) {
        std::tm tm = {};
        std::istringstream ss(date);
        ss >> std::get_time(&tm, "%Y-%m-%d");
        
        if (ss.fail()) return date;
        
        std::time_t time = std::mktime(&tm);
        time += days * 24 * 60 * 60;
        std::tm* newTm = std::localtime(&time);
        
        char buffer[11];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", newTm);
        return std::string(buffer);
    }
};

#endif // UTILS_H
