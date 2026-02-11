#ifndef RENTAL_H
#define RENTAL_H

#include <string>

class Rental {
private:
    std::string rentalId;
    std::string username;
    std::string vehicleId;
    std::string startDate;
    std::string expectedEndDate;
    std::string actualEndDate;
    double totalCost;
    double lateFee;
    bool isActive;
    bool isPaid;
    
public:
    Rental(const std::string& rentalId, const std::string& username,
           const std::string& vehicleId, const std::string& startDate,
           const std::string& expectedEndDate, double totalCost);
    
    // Getters
    std::string getRentalId() const { return rentalId; }
    std::string getUsername() const { return username; }
    std::string getVehicleId() const { return vehicleId; }
    std::string getStartDate() const { return startDate; }
    std::string getExpectedEndDate() const { return expectedEndDate; }
    std::string getActualEndDate() const { return actualEndDate; }
    double getTotalCost() const { return totalCost; }
    double getLateFee() const { return lateFee; }
    bool getIsActive() const { return isActive; }
    bool getIsPaid() const { return isPaid; }
    
    // Setters
    void setExpectedEndDate(const std::string& date) { expectedEndDate = date; }
    void setActualEndDate(const std::string& date) { actualEndDate = date; }
    void setLateFee(double fee) { lateFee = fee; }
    void setIsActive(bool active) { isActive = active; }
    void setIsPaid(bool paid) { isPaid = paid; }
    
    void display() const;
    double calculateLateFee(const std::string& returnDate, double pricePerDay) const;
};

#endif // RENTAL_H
