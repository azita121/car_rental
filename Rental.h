#ifndef RENTAL_H
#define RENTAL_H

#include <string>
using namespace std;


class Rental {
private:
    string rentalId;
    string username;
    string vehicleId;
    string startDate;
    string expectedEndDate;
    string actualEndDate;
    double totalCost;
    double lateFee;
    bool isActive;
    bool isPaid;
    
public:
    Rental(const string& rentalId, const string& username,
           const string& vehicleId, const string& startDate,
           const string& expectedEndDate, double totalCost);
    
    // Getters
    string getRentalId() const { return rentalId; }
    string getUsername() const { return username; }
    string getVehicleId() const { return vehicleId; }
    string getStartDate() const { return startDate; }
    string getExpectedEndDate() const { return expectedEndDate; }
    string getActualEndDate() const { return actualEndDate; }
    double getTotalCost() const { return totalCost; }
    double getLateFee() const { return lateFee; }
    bool getIsActive() const { return isActive; }
    bool getIsPaid() const { return isPaid; }
    
    // Setters
    void setExpectedEndDate(const string& date) { expectedEndDate = date; }
    void setActualEndDate(const string& date) { actualEndDate = date; }
    void setLateFee(double fee) { lateFee = fee; }
    void setIsActive(bool active) { isActive = active; }
    void setIsPaid(bool paid) { isPaid = paid; }
    
    void display() const;
    double calculateLateFee(const string& returnDate, double pricePerDay) const;
};

#endif // RENTAL_H
