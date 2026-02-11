#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>
#include "DataStructures.h"
using namespace std;


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
    string vehicleId;
    string brand;
    string model;
    int year;
    VehicleType type;
    double pricePerDay;
    VehicleStatus status;
    string currentRentalEndDate; // If rented, when it should be returned
    
public:
    Vehicle(const string& id, const string& brand, const string& model,
            int year, VehicleType type, double pricePerDay);
    
    // Getters
    string getVehicleId() const { return vehicleId; }
    string getBrand() const { return brand; }
    string getModel() const { return model; }
    int getYear() const { return year; }
    VehicleType getType() const { return type; }
    double getPricePerDay() const { return pricePerDay; }
    VehicleStatus getStatus() const { return status; }
    string getCurrentRentalEndDate() const { return currentRentalEndDate; }
    
    // Setters
    void setBrand(const string& b) { brand = b; }
    void setModel(const string& m) { model = m; }
    void setYear(int y) { year = y; }
    void setType(VehicleType t) { type = t; }
    void setPricePerDay(double p) { pricePerDay = p; }
    void setStatus(VehicleStatus s) { status = s; }
    void setCurrentRentalEndDate(const string& date) { currentRentalEndDate = date; }
    
    // Utility
    string getTypeString() const;
    string getStatusString() const;
    void displayDetails() const;
};

#endif // VEHICLE_H
