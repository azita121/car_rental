#include "Vehicle.h"
#include <iostream>
#include <iomanip>

Vehicle::Vehicle(const std::string& id, const std::string& brand, const std::string& model,
                 int year, VehicleType type, double pricePerDay)
    : vehicleId(id), brand(brand), model(model), year(year), type(type),
      pricePerDay(pricePerDay), status(VehicleStatus::Available), currentRentalEndDate("") {}

std::string Vehicle::getTypeString() const {
    switch (type) {
        case VehicleType::Sedan: return "Sedan";
        case VehicleType::SUV: return "SUV";
        case VehicleType::Hatchback: return "Hatchback";
        case VehicleType::Sports: return "Sports";
        case VehicleType::Luxury: return "Luxury";
        default: return "Unknown";
    }
}

std::string Vehicle::getStatusString() const {
    switch (status) {
        case VehicleStatus::Available: return "Available";
        case VehicleStatus::Reserved: return "Reserved";
        case VehicleStatus::Rented: return "Rented";
        case VehicleStatus::Maintenance: return "Maintenance";
        default: return "Unknown";
    }
}

void Vehicle::displayDetails() const {
    std::cout << "\n=== Vehicle Details ===" << std::endl;
    std::cout << "ID: " << vehicleId << std::endl;
    std::cout << "Brand: " << brand << std::endl;
    std::cout << "Model: " << model << std::endl;
    std::cout << "Year: " << year << std::endl;
    std::cout << "Type: " << getTypeString() << std::endl;
    std::cout << "Price per Day: $" << std::fixed << std::setprecision(2) << pricePerDay << std::endl;
    std::cout << "Status: " << getStatusString() << std::endl;
    if (status == VehicleStatus::Rented && !currentRentalEndDate.empty()) {
        std::cout << "Estimated Return Date: " << currentRentalEndDate << std::endl;
    }
    std::cout << "======================\n" << std::endl;
}
