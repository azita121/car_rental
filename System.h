#ifndef SYSTEM_H
#define SYSTEM_H

#include "DataStructures.h"
#include "User.h"
#include "Vehicle.h"
#include "Reservation.h"
#include "Rental.h"
#include "MaintenanceRecord.h"
#include "Config.h"
#include "Utils.h"
#include <string>
#include <queue>
#include <sstream>
#include <fstream>

// Comparator for reservation pointers in priority queue (earliest start date first)
struct ReservationPtrCompare {
    bool operator()(Reservation* a, Reservation* b) const {
        return *a < *b;
    }
};

class System {
private:
    // Data Structures
    AVLTree<std::string, User*> userMap; // AVL Tree for fast username lookup
    LinkedList<User*> users; // Linked List for all users
    LinkedList<Vehicle*> vehicles; // Linked List for all vehicles
    LinkedList<Reservation*> reservations; // Linked List for all reservations
    LinkedList<Rental*> rentals; // Linked List for all rentals
    LinkedList<MaintenanceRecord*> maintenanceRecords; // Linked List for maintenance records
    
    // Priority Queue for reservation queues per vehicle (earliest reservation first)
    AVLTree<std::string, PriorityQueue<Reservation*, ReservationPtrCompare>> reservationQueues;
    
    // Queue for rental processing (pickup and return)
    std::queue<std::string> rentalProcessingQueue;
    
    // Current logged-in user
    User* currentUser;
    
    // ID counters
    int nextReservationId;
    int nextRentalId;
    int nextMaintenanceRecordId;

    // Authorization Model:
    // - Customer: Can create reservations, view vehicles, make payments, extend rentals
    // - Staff: Can convert reservations to rentals, process returns, add vehicles, process queues
    // - Maintenance: Can add maintenance records, set vehicle maintenance status
    // - Manager: Can generate reports, block/unblock users
    // All authorization checks are enforced at the System method level, not in main.cpp
    
    // Helper methods
    bool isVehicleAvailable(const std::string& vehicleId, const std::string& startDate, const std::string& endDate) const;
    bool hasOverlappingReservation(const std::string& vehicleId, const std::string& startDate, const std::string& endDate) const;
    bool hasOverlappingRental(const std::string& vehicleId, const std::string& startDate, const std::string& endDate) const;
    Vehicle* findVehicle(const std::string& vehicleId) const;
    Reservation* findReservation(const std::string& reservationId) const;
    Rental* findRental(const std::string& rentalId) const;
    std::string generateReservationId();
    std::string generateRentalId();
    std::string generateMaintenanceRecordId();
    void processReservationQueue(const std::string& vehicleId);
    void clearAllData();
    
public:
    System();
    ~System();
    
    // Ensure bootstrap Manager/Staff users exist (used after import or on first run)
    void ensureBootstrapUsers();
    
    // User Management
    bool registerUser(const std::string& username, const std::string& password,
                      const std::string& fullName, const std::string& email, UserType type);
    User* login(const std::string& username, const std::string& password);
    void logout();
    User* getCurrentUser() const { return currentUser; }
    
    // Vehicle Management
    bool addVehicle(const std::string& vehicleId, const std::string& brand,
                    const std::string& model, int year, VehicleType type, double pricePerDay);
    bool editVehicle(const std::string& vehicleId, const std::string& brand,
                     const std::string& model, int year, VehicleType type, double pricePerDay);
    void displayAllVehicles() const;
    void displayVehiclesByFilter(const std::string& brand, VehicleType type) const;
    void displayVehicleDetails(const std::string& vehicleId) const;
    
    // Reservation Management
    bool createReservation(const std::string& vehicleId, const std::string& startDate,
                           const std::string& endDate);
    void displayUserReservations() const;
    bool extendRental(const std::string& rentalId, const std::string& newEndDate);
    
    // Rental Management
    bool convertReservationToRental(const std::string& reservationId);
    bool returnVehicle(const std::string& rentalId, const std::string& returnDate);
    void displayUserRentals() const;
    
    // Payment Management
    bool makePayment(double amount);
    
    // Staff Operations
    void processReservationQueues();
    void addMaintenanceRecord(const std::string& vehicleId, const std::string& description, double cost);
    void setVehicleMaintenance(const std::string& vehicleId, bool inMaintenance);
    
    // Manager Operations
    void generateRevenueReport() const;
    bool exportRevenueReportCSV(const std::string& filename) const;
    void blockUser(const std::string& username);
    void unblockUser(const std::string& username);
    
    // Backup & Restore
    bool exportData(const std::string& filename) const;
    bool importData(const std::string& filename);
};

#endif // SYSTEM_H
