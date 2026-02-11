#include "Rental.h"
#include "Utils.h"
#include "Config.h"
#include <iostream>
#include <iomanip>

Rental::Rental(const std::string& rentalId, const std::string& username,
               const std::string& vehicleId, const std::string& startDate,
               const std::string& expectedEndDate, double totalCost)
    : rentalId(rentalId), username(username), vehicleId(vehicleId),
      startDate(startDate), expectedEndDate(expectedEndDate), actualEndDate(""),
      totalCost(totalCost), lateFee(0.0), isActive(true), isPaid(false) {}

double Rental::calculateLateFee(const std::string& returnDate, double pricePerDay) const {
    if (DateUtils::isDateAfter(returnDate, expectedEndDate)) {
        int daysLate = DateUtils::daysBetween(expectedEndDate, returnDate);
        return daysLate * pricePerDay * Config::getLateFeeRate();
    }
    return 0.0;
}

void Rental::display() const {
    std::cout << "\n=== Rental ===" << std::endl;
    std::cout << "ID: " << rentalId << std::endl;
    std::cout << "Vehicle ID: " << vehicleId << std::endl;
    std::cout << "Start Date: " << startDate << std::endl;
    std::cout << "Expected End Date: " << expectedEndDate << std::endl;
    if (!actualEndDate.empty()) {
        std::cout << "Actual End Date: " << actualEndDate << std::endl;
    }
    std::cout << "Total Cost: $" << std::fixed << std::setprecision(2) << totalCost << std::endl;
    if (lateFee > 0) {
        std::cout << "Late Fee: $" << std::fixed << std::setprecision(2) << lateFee << std::endl;
    }
    std::cout << "Status: " << (isActive ? "Active" : "Completed") << std::endl;
    std::cout << "Paid: " << (isPaid ? "Yes" : "No") << std::endl;
    std::cout << "=============\n" << std::endl;
}
