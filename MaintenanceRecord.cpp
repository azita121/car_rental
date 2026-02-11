#include "MaintenanceRecord.h"
#include <iostream>
#include <iomanip>

MaintenanceRecord::MaintenanceRecord(const std::string& recordId, const std::string& vehicleId,
                                     const std::string& date, const std::string& description, double cost)
    : recordId(recordId), vehicleId(vehicleId), date(date), description(description), cost(cost) {}

void MaintenanceRecord::display() const {
    std::cout << "\n=== Maintenance Record ===" << std::endl;
    std::cout << "ID: " << recordId << std::endl;
    std::cout << "Vehicle ID: " << vehicleId << std::endl;
    std::cout << "Date: " << date << std::endl;
    std::cout << "Description: " << description << std::endl;
    std::cout << "Cost: $" << std::fixed << std::setprecision(2) << cost << std::endl;
    std::cout << "========================\n" << std::endl;
}
