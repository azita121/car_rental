#ifndef MAINTENANCE_RECORD_H
#define MAINTENANCE_RECORD_H

#include <string>

class MaintenanceRecord {
private:
    std::string recordId;
    std::string vehicleId;
    std::string date;
    std::string description;
    double cost;
    
public:
    MaintenanceRecord(const std::string& recordId, const std::string& vehicleId,
                      const std::string& date, const std::string& description, double cost);
    
    // Getters
    std::string getRecordId() const { return recordId; }
    std::string getVehicleId() const { return vehicleId; }
    std::string getDate() const { return date; }
    std::string getDescription() const { return description; }
    double getCost() const { return cost; }
    
    void display() const;
};

#endif // MAINTENANCE_RECORD_H
