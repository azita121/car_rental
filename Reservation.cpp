#include "Reservation.h"
#include "Utils.h"
#include <iostream>

Reservation::Reservation(const std::string& resId, const std::string& username,
                         const std::string& vehicleId, const std::string& startDate,
                         const std::string& endDate)
    : reservationId(resId), username(username), vehicleId(vehicleId),
      startDate(startDate), endDate(endDate), isActive(true), isClaimed(false) {
    // Get current date for reservation date
    reservationDate = DateUtils::getCurrentDate();
}

void Reservation::display() const {
    std::cout << "\n=== Reservation ===" << std::endl;
    std::cout << "ID: " << reservationId << std::endl;
    std::cout << "Vehicle ID: " << vehicleId << std::endl;
    std::cout << "Start Date: " << startDate << std::endl;
    std::cout << "End Date: " << endDate << std::endl;
    std::cout << "Reservation Date: " << reservationDate << std::endl;
    std::cout << "Status: " << (isActive ? "Active" : "Inactive") << std::endl;
    std::cout << "Claimed: " << (isClaimed ? "Yes" : "No") << std::endl;
    std::cout << "==================\n" << std::endl;
}
