#include "MaintenanceRecord.h"
#include <iostream>
#include <iomanip>
using namespace std;


MaintenanceRecord::MaintenanceRecord(const string& recordId, const string& vehicleId,
                                     const string& date, const string& description, double cost)
    : recordId(recordId), vehicleId(vehicleId), date(date), description(description), cost(cost) {}

void MaintenanceRecord::display() const {
    cout << "\n=== Maintenance Record ===" << endl;
    cout << "ID: " << recordId << endl;
    cout << "Vehicle ID: " << vehicleId << endl;
    cout << "Date: " << date << endl;
    cout << "Description: " << description << endl;
    cout << "Cost: $" << fixed << setprecision(2) << cost << endl;
    cout << "========================\n" << endl;
}
