#ifndef RESERVATION_H
#define RESERVATION_H

#include <string>
using namespace std;


class Reservation {
private:
    string reservationId;
    string username;
    string vehicleId;
    string startDate;
    string endDate;
    string reservationDate;
    bool isActive;
    bool isClaimed;
    
public:
    Reservation(const string& resId, const string& username,
                const string& vehicleId, const string& startDate,
                const string& endDate);
    
    // Getters
    string getReservationId() const { return reservationId; }
    string getUsername() const { return username; }
    string getVehicleId() const { return vehicleId; }
    string getStartDate() const { return startDate; }
    string getEndDate() const { return endDate; }
    string getReservationDate() const { return reservationDate; }
    bool getIsActive() const { return isActive; }
    bool getIsClaimed() const { return isClaimed; }
    
    // Setters
    void setIsActive(bool active) { isActive = active; }
    void setIsClaimed(bool claimed) { isClaimed = claimed; }
    // Used during import to restore original reservation date from file
    void restoreReservationDate(const string& date) { reservationDate = date; }
    
    // Comparison for priority queue (earliest start date first)
    bool operator<(const Reservation& other) const {
        return startDate > other.startDate; // Reverse for min-heap behavior
    }
    
    void display() const;
};

#endif // RESERVATION_H
