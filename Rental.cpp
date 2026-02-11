#include "Rental.h"
#include "Utils.h"
#include "Config.h"
#include <iostream>
#include <iomanip>
using namespace std;


Rental::Rental(const string& rentalId, const string& username,
               const string& vehicleId, const string& startDate,
               const string& expectedEndDate, double totalCost)
    : rentalId(rentalId), username(username), vehicleId(vehicleId),
      startDate(startDate), expectedEndDate(expectedEndDate), actualEndDate(""),
      totalCost(totalCost), lateFee(0.0), isActive(true), isPaid(false) {}

double Rental::calculateLateFee(const string& returnDate, double pricePerDay) const {
    if (DateUtils::isDateAfter(returnDate, expectedEndDate)) {
        int daysLate = DateUtils::daysBetween(expectedEndDate, returnDate);
        return daysLate * pricePerDay * Config::getLateFeeRate();
    }
    return 0.0;
}

void Rental::display() const {
    cout << "\n=== Rental ===" << endl;
    cout << "ID: " << rentalId << endl;
    cout << "Vehicle ID: " << vehicleId << endl;
    cout << "Start Date: " << startDate << endl;
    cout << "Expected End Date: " << expectedEndDate << endl;
    if (!actualEndDate.empty()) {
        cout << "Actual End Date: " << actualEndDate << endl;
    }
    cout << "Total Cost: $" << fixed << setprecision(2) << totalCost << endl;
    if (lateFee > 0) {
        cout << "Late Fee: $" << fixed << setprecision(2) << lateFee << endl;
    }
    cout << "Status: " << (isActive ? "Active" : "Completed") << endl;
    cout << "Paid: " << (isPaid ? "Yes" : "No") << endl;
    cout << "=============\n" << endl;
}
