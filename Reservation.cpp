#include "Reservation.h"
#include "Utils.h"
#include <iostream>
using namespace std;


Reservation::Reservation(const string& resId, const string& username,
                         const string& vehicleId, const string& startDate,
                         const string& endDate)
    : reservationId(resId), username(username), vehicleId(vehicleId),
      startDate(startDate), endDate(endDate), isActive(true), isClaimed(false) {
    // Get current date for reservation date
    reservationDate = DateUtils::getCurrentDate();
}

void Reservation::display() const {
    cout << "\n=== Reservation ===" << endl;
    cout << "ID: " << reservationId << endl;
    cout << "Vehicle ID: " << vehicleId << endl;
    cout << "Start Date: " << startDate << endl;
    cout << "End Date: " << endDate << endl;
    cout << "Reservation Date: " << reservationDate << endl;
    cout << "Status: " << (isActive ? "Active" : "Inactive") << endl;
    cout << "Claimed: " << (isClaimed ? "Yes" : "No") << endl;
    cout << "==================\n" << endl;
}
