#include "System.h"
#include <iostream>

/*
 * User Creation Rules (Option A - Academic-friendly approach):
 * 
 * 1. Customer Registration:
 *    - Customers can be registered via registerUser() by anyone (guest or logged-in)
 *    - This is the only user type that can be created through the public registration API
 * 
 * 2. Staff and Manager Creation:
 *    - Staff and Manager accounts can ONLY be created via:
 *      a) importData() - restoring from a backup file
 *      b) Bootstrap helper ensureBootstrapUsers() (for first run / demo)
 *    - This ensures proper access control and prevents unauthorized privilege escalation
 * 
 * 3. Bootstrap Accounts (for demo / first run):
 *    - ensureBootstrapUsers() will create two default privileged users IF they don't exist:
 *        * Manager: username \"admin\", password \"admin\"
 *        * Staff:   username \"staff\", password \"staff\"
 *    - This helper is called from main() after importData(). If a data file already defines
 *      Manager/Staff users (or specifically \"admin\"/\"staff\" usernames), no duplicates
 *      are created.
 * 
 * 4. Role-Based Authorization:
 *    - All System methods enforce role-based authorization
 *    - Authorization checks are at the System level, not in main.cpp
 *    - Even if menu bugs occur, unauthorized actions are rejected
 * 
 * 5. Persistence:
 *    - All user roles (Customer, Staff, Maintenance, Manager) are persisted in backup files
 *    - On import, users are restored with correct roles and inserted into AVL tree
 */

System::System()
    : currentUser(nullptr),
      nextReservationId(1),
      nextRentalId(1),
      nextMaintenanceRecordId(1) {}

void System::ensureBootstrapUsers() {
    // If admin or staff usernames already exist, do nothing
    if (userMap.contains("admin") || userMap.contains("staff")) {
        return;
    }

    Manager* defaultManager = new Manager("admin", "admin",
                                          "Default Manager", "admin@example.com");
    Staff* defaultStaff = new Staff("staff", "staff",
                                    "Default Staff", "staff@example.com");

    users.append(defaultManager);
    users.append(defaultStaff);
    userMap.insert(defaultManager->getUsername(), defaultManager);
    userMap.insert(defaultStaff->getUsername(), defaultStaff);
}

System::~System() {
    // Free all dynamically allocated objects
    for (auto it = users.begin(); it != users.end(); ++it) {
        delete *it;
    }
    for (auto it = vehicles.begin(); it != vehicles.end(); ++it) {
        delete *it;
    }
    for (auto it = reservations.begin(); it != reservations.end(); ++it) {
        delete *it;
    }
    for (auto it = rentals.begin(); it != rentals.end(); ++it) {
        delete *it;
    }
    for (auto it = maintenanceRecords.begin(); it != maintenanceRecords.end(); ++it) {
        delete *it;
    }
}

// -------- Helper methods --------

Vehicle* System::findVehicle(const std::string& vehicleId) const {
    for (auto it = const_cast<LinkedList<Vehicle*>&>(vehicles).begin();
         it != const_cast<LinkedList<Vehicle*>&>(vehicles).end(); ++it) {
        if ((*it)->getVehicleId() == vehicleId) return *it;
    }
    return nullptr;
}

Reservation* System::findReservation(const std::string& reservationId) const {
    for (auto it = const_cast<LinkedList<Reservation*>&>(reservations).begin();
         it != const_cast<LinkedList<Reservation*>&>(reservations).end(); ++it) {
        if ((*it)->getReservationId() == reservationId) return *it;
    }
    return nullptr;
}

Rental* System::findRental(const std::string& rentalId) const {
    for (auto it = const_cast<LinkedList<Rental*>&>(rentals).begin();
         it != const_cast<LinkedList<Rental*>&>(rentals).end(); ++it) {
        if ((*it)->getRentalId() == rentalId) return *it;
    }
    return nullptr;
}

std::string System::generateReservationId() {
    std::ostringstream oss;
    oss << "R" << nextReservationId++;
    return oss.str();
}

std::string System::generateRentalId() {
    std::ostringstream oss;
    oss << "L" << nextRentalId++;
    return oss.str();
}

std::string System::generateMaintenanceRecordId() {
    std::ostringstream oss;
    oss << "M" << nextMaintenanceRecordId++;
    return oss.str();
}

bool System::hasOverlappingReservation(const std::string& vehicleId,
                                       const std::string& startDate,
                                       const std::string& endDate) const {
    for (auto it = const_cast<LinkedList<Reservation*>&>(reservations).begin();
         it != const_cast<LinkedList<Reservation*>&>(reservations).end(); ++it) {
        Reservation* r = *it;
        if (!r->getIsActive()) continue;
        if (r->getVehicleId() != vehicleId) continue;

        // Overlap if not (newEnd < existingStart or newStart > existingEnd)
        bool noOverlap =
            DateUtils::isDateBefore(endDate, r->getStartDate()) ||
            DateUtils::isDateAfter(startDate, r->getEndDate());
        if (!noOverlap) return true;
    }
    return false;
}

bool System::hasOverlappingRental(const std::string& vehicleId,
                                  const std::string& startDate,
                                  const std::string& endDate) const {
    for (auto it = const_cast<LinkedList<Rental*>&>(rentals).begin();
         it != const_cast<LinkedList<Rental*>&>(rentals).end(); ++it) {
        Rental* r = *it;
        if (!r->getIsActive()) continue;
        if (r->getVehicleId() != vehicleId) continue;

        std::string existingEnd = r->getExpectedEndDate();
        bool noOverlap =
            DateUtils::isDateBefore(endDate, r->getStartDate()) ||
            DateUtils::isDateAfter(startDate, existingEnd);
        if (!noOverlap) return true;
    }
    return false;
}

bool System::isVehicleAvailable(const std::string& vehicleId,
                                const std::string& startDate,
                                const std::string& endDate) const {
    Vehicle* v = findVehicle(vehicleId);
    if (!v) return false;
    if (v->getStatus() == VehicleStatus::Maintenance) return false;

    if (hasOverlappingReservation(vehicleId, startDate, endDate)) return false;
    if (hasOverlappingRental(vehicleId, startDate, endDate)) return false;
    return true;
}

void System::processReservationQueue(const std::string& vehicleId) {
    PriorityQueue<Reservation*, ReservationPtrCompare>* queuePtr =
        reservationQueues.find(vehicleId);
    if (!queuePtr) return;

    auto& queue = *queuePtr;
    std::string today = DateUtils::getCurrentDate();

    while (!queue.empty()) {
        Reservation* r = queue.top();
        queue.pop();

        if (!r->getIsActive()) continue;

        int daysSinceReservation =
            DateUtils::daysBetween(r->getReservationDate(), today);
        if (daysSinceReservation > Config::getClaimWindow()) {
            r->setIsActive(false);
            continue;
        }

        Vehicle* v = findVehicle(vehicleId);
        if (!v) continue;

        if (isVehicleAvailable(vehicleId, r->getStartDate(), r->getEndDate())) {
            v->setStatus(VehicleStatus::Reserved);
            break;
        }
    }
}

// -------- User Management --------

// User Creation Rule (Option A - Academic-friendly):
// - Customers can be registered via registerUser() by anyone (guest or logged-in user)
// - Staff and Manager accounts can ONLY be created via importData() or hardcoded bootstrap
//   This ensures proper access control and prevents unauthorized privilege escalation
bool System::registerUser(const std::string& username,
                          const std::string& password,
                          const std::string& fullName,
                          const std::string& email,
                          UserType type) {
    // Authorization: Only Customer registration is allowed via this method
    // Staff and Manager must be created via importData() or bootstrap
    if (type != UserType::Customer) {
        std::cout << "Only Customer accounts can be registered via this method.\n";
        std::cout << "Staff and Manager accounts must be created via data import.\n";
        return false;
    }

    if (userMap.contains(username)) {
        std::cout << "Username already exists.\n";
        return false;
    }

    User* u = new Customer(username, password, fullName, email);
    users.append(u);
    userMap.insert(username, u);
    std::cout << "Customer registered successfully.\n";
    return true;
}

User* System::login(const std::string& username, const std::string& password) {
    User** uptr = userMap.find(username);
    if (!uptr) {
        std::cout << "User not found.\n";
        return nullptr;
    }
    User* u = *uptr;
    if (u->getIsBlocked()) {
        std::cout << "User is blocked.\n";
        return nullptr;
    }
    if (!u->verifyPassword(password)) {
        std::cout << "Invalid password.\n";
        return nullptr;
    }
    currentUser = u;
    std::cout << "Login successful. Welcome, " << u->getFullName() << "!\n";
    return u;
}

void System::logout() {
    currentUser = nullptr;
    std::cout << "Logged out.\n";
}

// -------- Vehicle Management --------

bool System::addVehicle(const std::string& vehicleId,
                        const std::string& brand,
                        const std::string& model,
                        int year,
                        VehicleType type,
                        double pricePerDay) {
    // Authorization: Only Staff can add vehicles to the fleet
    if (!currentUser || currentUser->getType() != UserType::Staff) {
        std::cout << "Only staff can add vehicles to the fleet.\n";
        return false;
    }

    if (findVehicle(vehicleId)) {
        std::cout << "Vehicle ID already exists.\n";
        return false;
    }
    Vehicle* v = new Vehicle(vehicleId, brand, model, year, type, pricePerDay);
    vehicles.append(v);
    std::cout << "Vehicle added.\n";
    return true;
}

void System::displayAllVehicles() const {
    for (auto it = const_cast<LinkedList<Vehicle*>&>(vehicles).begin();
         it != const_cast<LinkedList<Vehicle*>&>(vehicles).end(); ++it) {
        (*it)->displayDetails();
    }
}

void System::displayVehiclesByFilter(const std::string& brand,
                                     VehicleType type) const {
    for (auto it = const_cast<LinkedList<Vehicle*>&>(vehicles).begin();
         it != const_cast<LinkedList<Vehicle*>&>(vehicles).end(); ++it) {
        Vehicle* v = *it;
        if (!brand.empty() && v->getBrand() != brand) continue;
        if (v->getType() != type) continue;
        v->displayDetails();
    }
}

void System::displayVehicleDetails(const std::string& vehicleId) const {
    Vehicle* v = findVehicle(vehicleId);
    if (!v) {
        std::cout << "Vehicle not found.\n";
        return;
    }
    v->displayDetails();
}

// -------- Reservation Management --------

bool System::createReservation(const std::string& vehicleId,
                               const std::string& startDate,
                               const std::string& endDate) {
    if (!currentUser || currentUser->getType() != UserType::Customer) {
        std::cout << "Only logged-in customers can create reservations.\n";
        return false;
    }
    if (currentUser->getIsBlocked()) {
        std::cout << "You are blocked due to unpaid debt.\n";
        return false;
    }

    if (!DateUtils::isValidDate(startDate) || !DateUtils::isValidDate(endDate)) {
        std::cout << "Invalid date format. Use YYYY-MM-DD.\n";
        return false;
    }
    // Check if startDate is after endDate (invalid)
    // daysBetween(d1, d2) > 0  → d2 is after d1
    // So isDateAfter(endDate, startDate) is true exactly when startDate > endDate
    if (DateUtils::isDateAfter(endDate, startDate)) {
        std::cout << "Start date must be before or equal to end date.\n";
        return false;
    }

    int duration = DateUtils::daysBetween(startDate, endDate) + 1;
    if (duration < Config::MIN_RENTAL_DAYS ||
        duration > Config::MAX_RENTAL_DAYS) {
        std::cout << "Reservation duration out of allowed range.\n";
        return false;
    }

    if (!isVehicleAvailable(vehicleId, startDate, endDate)) {
        std::cout << "Vehicle not available for the selected dates.\n";
        return false;
    }

    std::string resId = generateReservationId();
    Reservation* r = new Reservation(resId, currentUser->getUsername(),
                                     vehicleId, startDate, endDate);
    reservations.append(r);

    PriorityQueue<Reservation*, ReservationPtrCompare>* qptr =
        reservationQueues.find(vehicleId);
    if (!qptr) {
        PriorityQueue<Reservation*, ReservationPtrCompare> newQ;
        newQ.push(r);
        reservationQueues.insert(vehicleId, newQ);
    } else {
        qptr->push(r);
    }

    Vehicle* v = findVehicle(vehicleId);
    if (v && v->getStatus() == VehicleStatus::Available) {
        v->setStatus(VehicleStatus::Reserved);
    }

    std::cout << "Reservation created with ID: " << resId << "\n";
    return true;
}

void System::displayUserReservations() const {
    if (!currentUser) {
        std::cout << "No user logged in.\n";
        return;
    }
    for (auto it = const_cast<LinkedList<Reservation*>&>(reservations).begin();
         it != const_cast<LinkedList<Reservation*>&>(reservations).end(); ++it) {
        if ((*it)->getUsername() == currentUser->getUsername()) {
            (*it)->display();
        }
    }
}

bool System::extendRental(const std::string& rentalId,
                          const std::string& newEndDate) {
    if (!currentUser) {
        std::cout << "Login required.\n";
        return false;
    }
    Rental* r = findRental(rentalId);
    if (!r) {
        std::cout << "Rental not found.\n";
        return false;
    }
    if (r->getUsername() != currentUser->getUsername()) {
        std::cout << "You can only extend your own rentals.\n";
        return false;
    }

    if (!DateUtils::isValidDate(newEndDate)) {
        std::cout << "Invalid date format. Use YYYY-MM-DD.\n";
        return false;
    }

    std::string vehicleId = r->getVehicleId();
    std::string oldEnd = r->getExpectedEndDate();

    if (DateUtils::isDateBefore(newEndDate, oldEnd)) {
        std::cout << "New end date cannot be before current end date.\n";
        return false;
    }

    if (!isVehicleAvailable(vehicleId, oldEnd, newEndDate)) {
        std::cout << "Cannot extend rental due to future reservations.\n";
        return false;
    }

    int extraDays = DateUtils::daysBetween(oldEnd, newEndDate);
    Vehicle* v = findVehicle(vehicleId);
    if (!v) return false;

    double extraCost = extraDays * v->getPricePerDay();
    currentUser->addToBalance(extraCost);
    // We are extending the planned end date, not returning the car yet
    r->setExpectedEndDate(newEndDate);

    std::cout << "Rental extended. Additional cost: " << extraCost << "\n";
    return true;
}

// -------- Rental Management --------

bool System::convertReservationToRental(const std::string& reservationId) {
    if (!currentUser || currentUser->getType() != UserType::Staff) {
        std::cout << "Only staff can convert reservations.\n";
        return false;
    }
    Reservation* r = findReservation(reservationId);
    if (!r || !r->getIsActive()) {
        std::cout << "Reservation not found or inactive.\n";
        return false;
    }

    Vehicle* v = findVehicle(r->getVehicleId());
    if (!v) {
        std::cout << "Vehicle not found.\n";
        return false;
    }

    int days = DateUtils::daysBetween(r->getStartDate(), r->getEndDate()) + 1;
    double totalCost = days * v->getPricePerDay();

    std::string rentalId = generateRentalId();
    Rental* rental = new Rental(rentalId, r->getUsername(), r->getVehicleId(),
                                r->getStartDate(), r->getEndDate(), totalCost);
    rentals.append(rental);

    r->setIsActive(false);
    v->setStatus(VehicleStatus::Rented);
    v->setCurrentRentalEndDate(r->getEndDate());

    User** uPtr = userMap.find(r->getUsername());
    if (uPtr) {
        (*uPtr)->addToBalance(totalCost);
    }

    std::cout << "Reservation converted to rental. Rental ID: " << rentalId << "\n";
    return true;
}

bool System::returnVehicle(const std::string& rentalId,
                           const std::string& returnDate) {
    if (!currentUser || currentUser->getType() != UserType::Staff) {
        std::cout << "Only staff can process returns.\n";
        return false;
    }
    if (!DateUtils::isValidDate(returnDate)) {
        std::cout << "Invalid date format. Use YYYY-MM-DD.\n";
        return false;
    }

    Rental* r = findRental(rentalId);
    if (!r || !r->getIsActive()) {
        std::cout << "Rental not found or already completed.\n";
        return false;
    }

    Vehicle* v = findVehicle(r->getVehicleId());
    if (!v) {
        std::cout << "Vehicle not found.\n";
        return false;
    }

    double fee = r->calculateLateFee(returnDate, v->getPricePerDay());
    r->setLateFee(fee);
    r->setActualEndDate(returnDate);
    r->setIsActive(false);

    User** uPtr = userMap.find(r->getUsername());
    if (uPtr) {
        (*uPtr)->addToBalance(fee);
        if ((*uPtr)->getOutstandingBalance() >= Config::getBlockThreshold()) {
            (*uPtr)->setIsBlocked(true);
        }
    }

    v->setStatus(VehicleStatus::Available);
    v->setCurrentRentalEndDate("");

    processReservationQueue(v->getVehicleId());

    std::cout << "Vehicle returned. Late fee: " << fee << "\n";
    return true;
}

void System::displayUserRentals() const {
    if (!currentUser) {
        std::cout << "Login required.\n";
        return;
    }
    for (auto it = const_cast<LinkedList<Rental*>&>(rentals).begin();
         it != const_cast<LinkedList<Rental*>&>(rentals).end(); ++it) {
        if ((*it)->getUsername() == currentUser->getUsername()) {
            (*it)->display();
        }
    }
}

// -------- Payments --------

bool System::makePayment(double amount) {
    if (!currentUser) {
        std::cout << "Login required.\n";
        return false;
    }
    if (amount <= 0) {
        std::cout << "Amount must be positive.\n";
        return false;
    }

    if (amount > currentUser->getOutstandingBalance()) {
        amount = currentUser->getOutstandingBalance();
    }
    currentUser->subtractFromBalance(amount);
    if (currentUser->getOutstandingBalance() < Config::getBlockThreshold()) {
        currentUser->setIsBlocked(false);
    }
    std::cout << "Payment successful. Remaining balance: "
              << currentUser->getOutstandingBalance() << "\n";
    return true;
}

// -------- Staff / Maintenance --------

void System::processReservationQueues() {
    // Authorization: Only Staff can manually trigger reservation queue processing
    if (!currentUser || currentUser->getType() != UserType::Staff) {
        std::cout << "Only staff can process reservation queues.\n";
        return;
    }

    for (auto it = vehicles.begin(); it != vehicles.end(); ++it) {
        processReservationQueue((*it)->getVehicleId());
    }
    std::cout << "Reservation queues processed.\n";
}

void System::addMaintenanceRecord(const std::string& vehicleId,
                                  const std::string& description,
                                  double cost) {
    if (!currentUser || currentUser->getType() != UserType::Maintenance) {
        std::cout << "Only maintenance staff can add maintenance records.\n";
        return;
    }
    Vehicle* v = findVehicle(vehicleId);
    if (!v) {
        std::cout << "Vehicle not found.\n";
        return;
    }

    std::string recordId = generateMaintenanceRecordId();
    std::string date = DateUtils::getCurrentDate();
    MaintenanceRecord* mr =
        new MaintenanceRecord(recordId, vehicleId, date, description, cost);
    maintenanceRecords.append(mr);
    v->setStatus(VehicleStatus::Maintenance);

    std::cout << "Maintenance record added.\n";
}

void System::setVehicleMaintenance(const std::string& vehicleId,
                                   bool inMaintenance) {
    if (!currentUser || currentUser->getType() != UserType::Maintenance) {
        std::cout << "Only maintenance staff can change maintenance status.\n";
        return;
    }
    Vehicle* v = findVehicle(vehicleId);
    if (!v) {
        std::cout << "Vehicle not found.\n";
        return;
    }
    v->setStatus(inMaintenance ? VehicleStatus::Maintenance
                               : VehicleStatus::Available);
    std::cout << "Vehicle maintenance status updated.\n";
}

// -------- Manager operations --------

void System::generateRevenueReport() const {
    // Authorization: Only Manager can generate revenue reports
    if (!currentUser || currentUser->getType() != UserType::Manager) {
        std::cout << "Only managers can generate revenue reports.\n";
        return;
    }
    double totalIncome = 0.0;
    int totalRentals = 0;
    double totalLateFees = 0.0;

    for (auto it = const_cast<LinkedList<Rental*>&>(rentals).begin();
         it != const_cast<LinkedList<Rental*>&>(rentals).end(); ++it) {
        Rental* r = *it;
        totalIncome += r->getTotalCost() + r->getLateFee();
        totalLateFees += r->getLateFee();
        totalRentals++;
    }

    int totalVehicles = vehicles.getSize();
    int rentedCount = 0;
    for (auto it = const_cast<LinkedList<Vehicle*>&>(vehicles).begin();
         it != const_cast<LinkedList<Vehicle*>&>(vehicles).end(); ++it) {
        if ((*it)->getStatus() == VehicleStatus::Rented) rentedCount++;
    }
    double utilizationRate =
        totalVehicles == 0 ? 0.0 : (double)rentedCount / totalVehicles * 100.0;

    int blockedUsers = 0;
    for (auto it = const_cast<LinkedList<User*>&>(users).begin();
         it != const_cast<LinkedList<User*>&>(users).end(); ++it) {
        if ((*it)->getIsBlocked()) blockedUsers++;
    }

    std::cout << "\n=== Revenue & Performance Report ===\n";
    std::cout << "Total income: " << totalIncome << "\n";
    std::cout << "Total rentals: " << totalRentals << "\n";
    std::cout << "Total late fees: " << totalLateFees << "\n";
    std::cout << "Fleet size: " << totalVehicles << "\n";
    std::cout << "Currently rented: " << rentedCount
              << " (" << utilizationRate << "%)\n";
    std::cout << "Blocked users: " << blockedUsers << "\n";

    std::cout << "\nPer-vehicle rental counts:\n";
    for (auto it = const_cast<LinkedList<Vehicle*>&>(vehicles).begin();
         it != const_cast<LinkedList<Vehicle*>&>(vehicles).end(); ++it) {
        Vehicle* v = *it;
        int count = 0;
        for (auto rit = const_cast<LinkedList<Rental*>&>(rentals).begin();
             rit != const_cast<LinkedList<Rental*>&>(rentals).end(); ++rit) {
            if ((*rit)->getVehicleId() == v->getVehicleId()) count++;
        }
        std::cout << "  " << v->getVehicleId() << " (" << v->getBrand() << " " << v->getModel()
                  << "): " << count << " rentals\n";
    }
}

bool System::exportRevenueReportCSV(const std::string& filename) const {
    // Authorization: Only Manager can export revenue reports
    if (!currentUser || currentUser->getType() != UserType::Manager) {
        std::cout << "Only managers can export revenue reports.\n";
        return false;
    }

    std::ofstream out(filename.c_str());
    if (!out) {
        std::cout << "Failed to open file for writing.\n";
        return false;
    }

    double totalIncome = 0.0;
    double totalLateFees = 0.0;
    int totalRentals = 0;
    for (auto it = const_cast<LinkedList<Rental*>&>(rentals).begin();
         it != const_cast<LinkedList<Rental*>&>(rentals).end(); ++it) {
        Rental* r = *it;
        totalIncome += r->getTotalCost() + r->getLateFee();
        totalLateFees += r->getLateFee();
        totalRentals++;
    }

    int blockedUsers = 0;
    for (auto it = const_cast<LinkedList<User*>&>(users).begin();
         it != const_cast<LinkedList<User*>&>(users).end(); ++it) {
        if ((*it)->getIsBlocked()) blockedUsers++;
    }

    out << "metric,value\n";
    out << "total_income," << totalIncome << "\n";
    out << "total_rentals," << totalRentals << "\n";
    out << "total_late_fees," << totalLateFees << "\n";
    out << "blocked_users," << blockedUsers << "\n";

    out << "\nvehicle_id,brand,model,rental_count\n";
    for (auto it = const_cast<LinkedList<Vehicle*>&>(vehicles).begin();
         it != const_cast<LinkedList<Vehicle*>&>(vehicles).end(); ++it) {
        Vehicle* v = *it;
        int count = 0;
        for (auto rit = const_cast<LinkedList<Rental*>&>(rentals).begin();
             rit != const_cast<LinkedList<Rental*>&>(rentals).end(); ++rit) {
            if ((*rit)->getVehicleId() == v->getVehicleId()) count++;
        }
        out << v->getVehicleId() << "," << v->getBrand() << ","
            << v->getModel() << "," << count << "\n";
    }

    std::cout << "Revenue report exported to " << filename << "\n";
    return true;
}

void System::blockUser(const std::string& username) {
    // Authorization: Only Manager can block users
    if (!currentUser || currentUser->getType() != UserType::Manager) {
        std::cout << "Only managers can block users.\n";
        return;
    }

    User** uptr = userMap.find(username);
    if (!uptr) {
        std::cout << "User not found.\n";
        return;
    }
    (*uptr)->setIsBlocked(true);
    std::cout << "User blocked.\n";
}

void System::unblockUser(const std::string& username) {
    // Authorization: Only Manager can unblock users
    if (!currentUser || currentUser->getType() != UserType::Manager) {
        std::cout << "Only managers can unblock users.\n";
        return;
    }

    User** uptr = userMap.find(username);
    if (!uptr) {
        std::cout << "User not found.\n";
        return;
    }
    (*uptr)->setIsBlocked(false);
    std::cout << "User unblocked.\n";
}

// -------- Backup & Restore (very simple CSV-like format) --------

bool System::exportData(const std::string& filename) const {
    std::ofstream out(filename.c_str());
    if (!out) return false;

    out << "[USERS]\n";
    for (auto it = const_cast<LinkedList<User*>&>(users).begin();
         it != const_cast<LinkedList<User*>&>(users).end(); ++it) {
        User* u = *it;
        out << u->getUsername() << "," << u->getPasswordHash() << ","
            << u->getFullName() << "," << u->getEmail() << ","
            << static_cast<int>(u->getType()) << ","
            << u->getOutstandingBalance() << ","
            << (u->getIsBlocked() ? 1 : 0) << "\n";
    }

    out << "[VEHICLES]\n";
    for (auto it = const_cast<LinkedList<Vehicle*>&>(vehicles).begin();
         it != const_cast<LinkedList<Vehicle*>&>(vehicles).end(); ++it) {
        Vehicle* v = *it;
        out << v->getVehicleId() << "," << v->getBrand() << ","
            << v->getModel() << "," << v->getYear() << ","
            << static_cast<int>(v->getType()) << ","
            << v->getPricePerDay() << ","
            << static_cast<int>(v->getStatus()) << ","
            << v->getCurrentRentalEndDate() << "\n";
    }

    out << "[RESERVATIONS]\n";
    for (auto it = const_cast<LinkedList<Reservation*>&>(reservations).begin();
         it != const_cast<LinkedList<Reservation*>&>(reservations).end(); ++it) {
        Reservation* r = *it;
        out << r->getReservationId() << "," << r->getUsername() << ","
            << r->getVehicleId() << "," << r->getStartDate() << ","
            << r->getEndDate() << "," << r->getReservationDate() << ","
            << (r->getIsActive() ? 1 : 0) << ","
            << (r->getIsClaimed() ? 1 : 0) << "\n";
    }

    out << "[RENTALS]\n";
    for (auto it = const_cast<LinkedList<Rental*>&>(rentals).begin();
         it != const_cast<LinkedList<Rental*>&>(rentals).end(); ++it) {
        Rental* r = *it;
        out << r->getRentalId() << "," << r->getUsername() << ","
            << r->getVehicleId() << "," << r->getStartDate() << ","
            << r->getExpectedEndDate() << "," << r->getActualEndDate() << ","
            << r->getTotalCost() << "," << r->getLateFee() << ","
            << (r->getIsActive() ? 1 : 0) << ","
            << (r->getIsPaid() ? 1 : 0) << "\n";
    }

    out << "[MAINTENANCE]\n";
    for (auto it = const_cast<LinkedList<MaintenanceRecord*>&>(maintenanceRecords).begin();
         it != const_cast<LinkedList<MaintenanceRecord*>&>(maintenanceRecords).end(); ++it) {
        MaintenanceRecord* m = *it;
        out << m->getRecordId() << "," << m->getVehicleId() << ","
            << m->getDate() << "," << m->getDescription() << ","
            << m->getCost() << "\n";
    }

    out << "[END]\n";
    std::cout << "Data exported successfully to " << filename << "\n";
    return true;
}

bool System::importData(const std::string& filename) {
    // This method is the ONLY way to create Staff and Manager users
    // (along with hardcoded bootstrap if implemented)
    // Customer users can also be restored via this method
    
    std::ifstream in(filename.c_str());
    if (!in) {
        // File doesn't exist yet - this is normal on first run
        return false;
    }

    users.clear();
    vehicles.clear();
    reservations.clear();
    rentals.clear();
    maintenanceRecords.clear();
    userMap.clear();
    reservationQueues.clear();

    std::string line;
    enum Section { NONE, USERS, VEHICLES, RESERVATIONS, RENTALS, MAINT } section = NONE;

    while (std::getline(in, line)) {
        if (line == "[USERS]") {
            section = USERS;
            continue;
        }
        if (line == "[VEHICLES]") {
            section = VEHICLES;
            continue;
        }
        if (line == "[RESERVATIONS]") {
            section = RESERVATIONS;
            continue;
        }
        if (line == "[RENTALS]") {
            section = RENTALS;
            continue;
        }
        if (line == "[MAINTENANCE]") {
            section = MAINT;
            continue;
        }
        if (line == "[END]") break;
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string field;

        if (section == USERS) {
            std::string username, hash, fullName, email;
            int typeInt, blockedInt;
            double balance;

            std::getline(iss, username, ',');
            std::getline(iss, hash, ',');
            std::getline(iss, fullName, ',');
            std::getline(iss, email, ',');
            std::getline(iss, field, ','); typeInt = std::stoi(field);
            std::getline(iss, field, ','); balance = std::stod(field);
            std::getline(iss, field, ','); blockedInt = std::stoi(field);

            UserType type = static_cast<UserType>(typeInt);
            User* u = nullptr;
            // Restore user with correct role - this is where Staff and Manager are created
            switch (type) {
                case UserType::Customer:
                    u = new Customer(username, "", fullName, email);
                    break;
                case UserType::Staff:
                    u = new Staff(username, "", fullName, email);
                    break;
                case UserType::Maintenance:
                    u = new Maintenance(username, "", fullName, email);
                    break;
                case UserType::Manager:
                    u = new Manager(username, "", fullName, email);
                    break;
                default:
                    break;
            }
            if (!u) continue;
            // Restore password hash, blocked status, and balance
            u->restorePasswordHash(hash);
            u->setIsBlocked(blockedInt != 0);
            u->addToBalance(balance);
            users.append(u);
            userMap.insert(username, u); // Rebuild AVL tree for fast lookup
        } else if (section == VEHICLES) {
            std::string id, brand, model, typeStr, priceStr, statusStr, yearStr, endDate;
            int typeInt, statusInt, year;
            double price;

            std::getline(iss, id, ',');
            std::getline(iss, brand, ',');
            std::getline(iss, model, ',');
            std::getline(iss, yearStr, ','); year = std::stoi(yearStr);
            std::getline(iss, typeStr, ','); typeInt = std::stoi(typeStr);
            std::getline(iss, priceStr, ','); price = std::stod(priceStr);
            std::getline(iss, statusStr, ','); statusInt = std::stoi(statusStr);
            std::getline(iss, endDate, ',');

            Vehicle* v = new Vehicle(id, brand, model, year,
                                     static_cast<VehicleType>(typeInt),
                                     price);
            v->setStatus(static_cast<VehicleStatus>(statusInt));
            v->setCurrentRentalEndDate(endDate);
            vehicles.append(v);
        } else if (section == RESERVATIONS) {
            std::string id, username, vehicleId, start, end, resDate;
            int activeInt, claimedInt;

            std::getline(iss, id, ',');
            std::getline(iss, username, ',');
            std::getline(iss, vehicleId, ',');
            std::getline(iss, start, ',');
            std::getline(iss, end, ',');
            std::getline(iss, resDate, ',');
            std::getline(iss, field, ','); activeInt = std::stoi(field);
            std::getline(iss, field, ','); claimedInt = std::stoi(field);

            Reservation* r = new Reservation(id, username, vehicleId, start, end);
            // Restore original reservation date and flags so claim-window logic
            // remains correct across restarts
            r->restoreReservationDate(resDate);
            r->setIsActive(activeInt != 0);
            r->setIsClaimed(claimedInt != 0);
            reservations.append(r);

            if (r->getIsActive()) {
                PriorityQueue<Reservation*, ReservationPtrCompare>* qptr =
                    reservationQueues.find(vehicleId);
                if (!qptr) {
                    PriorityQueue<Reservation*, ReservationPtrCompare> newQ;
                    newQ.push(r);
                    reservationQueues.insert(vehicleId, newQ);
                } else {
                    qptr->push(r);
                }
            }
        } else if (section == RENTALS) {
            std::string id, username, vehicleId, start, expectedEnd, actualEnd;
            std::string totalStr, lateStr;
            int activeInt, paidInt;
            double total, late;

            std::getline(iss, id, ',');
            std::getline(iss, username, ',');
            std::getline(iss, vehicleId, ',');
            std::getline(iss, start, ',');
            std::getline(iss, expectedEnd, ',');
            std::getline(iss, actualEnd, ',');
            std::getline(iss, totalStr, ','); total = std::stod(totalStr);
            std::getline(iss, lateStr, ','); late = std::stod(lateStr);
            std::getline(iss, field, ','); activeInt = std::stoi(field);
            std::getline(iss, field, ','); paidInt = std::stoi(field);

            Rental* r = new Rental(id, username, vehicleId, start, expectedEnd, total);
            r->setActualEndDate(actualEnd);
            r->setLateFee(late);
            r->setIsActive(activeInt != 0);
            r->setIsPaid(paidInt != 0);
            rentals.append(r);
        } else if (section == MAINT) {
            std::string id, vehicleId, date, desc, costStr;
            double cost;

            std::getline(iss, id, ',');
            std::getline(iss, vehicleId, ',');
            std::getline(iss, date, ',');
            std::getline(iss, desc, ',');
            std::getline(iss, costStr, ','); cost = std::stod(costStr);

            MaintenanceRecord* m = new MaintenanceRecord(id, vehicleId, date, desc, cost);
            maintenanceRecords.append(m);
        }
    }

    // Rebuild ID counters based on maximum existing IDs
    int maxResId = 0, maxRentalId = 0, maxMaintId = 0;
    for (auto it = reservations.begin(); it != reservations.end(); ++it) {
        const std::string& id = (*it)->getReservationId();
        if (id.size() > 1) {
            int n = std::atoi(id.c_str() + 1);
            if (n > maxResId) maxResId = n;
        }
    }
    for (auto it = rentals.begin(); it != rentals.end(); ++it) {
        const std::string& id = (*it)->getRentalId();
        if (id.size() > 1) {
            int n = std::atoi(id.c_str() + 1);
            if (n > maxRentalId) maxRentalId = n;
        }
    }
    for (auto it = maintenanceRecords.begin(); it != maintenanceRecords.end(); ++it) {
        const std::string& id = (*it)->getRecordId();
        if (id.size() > 1) {
            int n = std::atoi(id.c_str() + 1);
            if (n > maxMaintId) maxMaintId = n;
        }
    }
    nextReservationId = maxResId + 1;
    nextRentalId = maxRentalId + 1;
    nextMaintenanceRecordId = maxMaintId + 1;

    std::cout << "Data imported successfully. Users, vehicles, reservations, rentals, and maintenance records restored.\n";
    return true;
}

