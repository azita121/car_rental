#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>
#include "DataStructures.h"

enum class VehicleStatus {
    Available,
    Reserved,
    Rented,
    Maintenance
};

enum class VehicleType {
    Sedan,
    SUV,
    Hatchback,
    Sports,
    Luxury
};

class Vehicle {
private:
    std::string vehicleId;
    std::string brand;
    std::string model;
    int year;
    VehicleType type;
    double pricePerDay;
    VehicleStatus status;
    std::string currentRentalEndDate; // If rented, when it should be returned
    
public:
    Vehicle(const std::string& id, const std::string& brand, const std::string& model,
            int year, VehicleType type, double pricePerDay);
    
    // Getters
    std::string getVehicleId() const { return vehicleId; }
    std::string getBrand() const { return brand; }
    std::string getModel() const { return model; }
    int getYear() const { return year; }
    VehicleType getType() const { return type; }
    double getPricePerDay() const { return pricePerDay; }
    VehicleStatus getStatus() const { return status; }
    std::string getCurrentRentalEndDate() const { return currentRentalEndDate; }
    
    // Setters
    void setStatus(VehicleStatus s) { status = s; }
    void setCurrentRentalEndDate(const std::string& date) { currentRentalEndDate = date; }
    
    // Utility
    std::string getTypeString() const;
    std::string getStatusString() const;
    void displayDetails() const;
};

#endif // VEHICLE_H
