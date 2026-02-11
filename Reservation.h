#ifndef RESERVATION_H
#define RESERVATION_H

#include <string>

class Reservation {
private:
    std::string reservationId;
    std::string username;
    std::string vehicleId;
    std::string startDate;
    std::string endDate;
    std::string reservationDate;
    bool isActive;
    bool isClaimed;
    
public:
    Reservation(const std::string& resId, const std::string& username,
                const std::string& vehicleId, const std::string& startDate,
                const std::string& endDate);
    
    // Getters
    std::string getReservationId() const { return reservationId; }
    std::string getUsername() const { return username; }
    std::string getVehicleId() const { return vehicleId; }
    std::string getStartDate() const { return startDate; }
    std::string getEndDate() const { return endDate; }
    std::string getReservationDate() const { return reservationDate; }
    bool getIsActive() const { return isActive; }
    bool getIsClaimed() const { return isClaimed; }
    
    // Setters
    void setIsActive(bool active) { isActive = active; }
    void setIsClaimed(bool claimed) { isClaimed = claimed; }
    // Used during import to restore original reservation date from file
    void restoreReservationDate(const std::string& date) { reservationDate = date; }
    
    // Comparison for priority queue (earliest start date first)
    bool operator<(const Reservation& other) const {
        return startDate > other.startDate; // Reverse for min-heap behavior
    }
    
    void display() const;
};

#endif // RESERVATION_H
