#ifndef MAINTENANCE_RECORD_H
#define MAINTENANCE_RECORD_H

#include <string>
using namespace std;


class MaintenanceRecord {
private:
    string recordId;
    string vehicleId;
    string date;
    string description;
    double cost;
    
public:
    MaintenanceRecord(const string& recordId, const string& vehicleId,
                      const string& date, const string& description, double cost);
    
    // Getters
    string getRecordId() const { return recordId; }
    string getVehicleId() const { return vehicleId; }
    string getDate() const { return date; }
    string getDescription() const { return description; }
    double getCost() const { return cost; }
    
    void display() const;
};

#endif // MAINTENANCE_RECORD_H
