#include "Vehicle.h"
#include <iostream>
#include <iomanip>
using namespace std;


Vehicle::Vehicle(const string& id, const string& brand, const string& model,
                 int year, VehicleType type, double pricePerDay)
    : vehicleId(id), brand(brand), model(model), year(year), type(type),
      pricePerDay(pricePerDay), status(VehicleStatus::Available), currentRentalEndDate("") {}

string Vehicle::getTypeString() const {
    switch (type) {
        case VehicleType::Sedan: return "Sedan";
        case VehicleType::SUV: return "SUV";
        case VehicleType::Hatchback: return "Hatchback";
        case VehicleType::Sports: return "Sports";
        case VehicleType::Luxury: return "Luxury";
        default: return "Unknown";
    }
}

string Vehicle::getStatusString() const {
    switch (status) {
        case VehicleStatus::Available: return "Available";
        case VehicleStatus::Reserved: return "Reserved";
        case VehicleStatus::Rented: return "Rented";
        case VehicleStatus::Maintenance: return "Maintenance";
        default: return "Unknown";
    }
}

void Vehicle::displayDetails() const {
    cout << "\n=== Vehicle Details ===" << endl;
    cout << "ID: " << vehicleId << endl;
    cout << "Brand: " << brand << endl;
    cout << "Model: " << model << endl;
    cout << "Year: " << year << endl;
    cout << "Type: " << getTypeString() << endl;
    cout << "Price per Day: $" << fixed << setprecision(2) << pricePerDay << endl;
    cout << "Status: " << getStatusString() << endl;
    if (status == VehicleStatus::Rented && !currentRentalEndDate.empty()) {
        cout << "Estimated Return Date: " << currentRentalEndDate << endl;
    }
    cout << "======================\n" << endl;
}
