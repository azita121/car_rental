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
using namespace std;


// Comparator for reservation pointers in priority queue (earliest start date first)
struct ReservationPtrCompare {
    bool operator()(Reservation* a, Reservation* b) const {
        return *a < *b;
    }
};

class System {
private:
    // Data Structures
    AVLTree<string, User*> userMap; // AVL Tree for fast username lookup
    LinkedList<User*> users; // Linked List for all users
    LinkedList<Vehicle*> vehicles; // Linked List for all vehicles
    LinkedList<Reservation*> reservations; // Linked List for all reservations
    LinkedList<Rental*> rentals; // Linked List for all rentals
    LinkedList<MaintenanceRecord*> maintenanceRecords; // Linked List for maintenance records
    
    // Priority Queue for reservation queues per vehicle (earliest reservation first)
    AVLTree<string, PriorityQueue<Reservation*, ReservationPtrCompare>> reservationQueues;
    
    // Queue for rental processing (pickup and return)
    queue<string> rentalProcessingQueue;
    
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
    bool isVehicleAvailable(const string& vehicleId, const string& startDate, const string& endDate) const;
    bool hasOverlappingReservation(const string& vehicleId, const string& startDate, const string& endDate) const;
    bool hasOverlappingRental(const string& vehicleId, const string& startDate, const string& endDate) const;
    Vehicle* findVehicle(const string& vehicleId) const;
    Reservation* findReservation(const string& reservationId) const;
    Rental* findRental(const string& rentalId) const;
    string generateReservationId();
    string generateRentalId();
    string generateMaintenanceRecordId();
    void processReservationQueue(const string& vehicleId);
    void clearAllData();
    
public:
    System();
    ~System();
    
    // Ensure bootstrap Manager/Staff users exist (used after import or on first run)
    void ensureBootstrapUsers();
    
    // User Management
    bool registerUser(const string& username, const string& password,
                      const string& fullName, const string& email, UserType type);
    User* login(const string& username, const string& password);
    void logout();
    User* getCurrentUser() const { return currentUser; }
    
    // Vehicle Management
    bool addVehicle(const string& vehicleId, const string& brand,
                    const string& model, int year, VehicleType type, double pricePerDay);
    bool editVehicle(const string& vehicleId, const string& brand,
                     const string& model, int year, VehicleType type, double pricePerDay);
    void displayAllVehicles() const;
    void displayVehiclesByFilter(const string& brand, VehicleType type) const;
    void displayVehicleDetails(const string& vehicleId) const;
    
    // Reservation Management
    bool createReservation(const string& vehicleId, const string& startDate,
                           const string& endDate);
    void displayUserReservations() const;
    bool extendRental(const string& rentalId, const string& newEndDate);
    
    // Rental Management
    bool convertReservationToRental(const string& reservationId);
    bool returnVehicle(const string& rentalId, const string& returnDate);
    void displayUserRentals() const;
    
    // Payment Management
    bool makePayment(double amount);
    void showPaymentSummaryForCurrentUser() const;
    
    // Staff Operations
    void processReservationQueues();
    void viewReservationQueue(const string& vehicleId);
    void addMaintenanceRecord(const string& vehicleId, const string& description, double cost);
    void setVehicleMaintenance(const string& vehicleId, bool inMaintenance);
    
    // Manager Operations
    void generateRevenueReport() const;
    bool exportRevenueReportCSV(const string& filename) const;
    void blockUser(const string& username);
    void unblockUser(const string& username);
    
    // Backup & Restore
    bool exportData(const string& filename) const;
    bool importData(const string& filename);
};

#endif // SYSTEM_H
