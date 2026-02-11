#include "System.h"
#include <iostream>

// Helper to safely read a line and strip/replace commas so CSV export stays valid.
// Commas entered by the user are replaced with spaces and documented as unsupported.
static std::string readLineNoComma(const std::string& prompt) {
    std::string line;
    std::cout << prompt;
    std::getline(std::cin, line);
    for (char& c : line) {
        if (c == ',') c = ' '; // commas are not allowed in stored text fields
    }
    return line;
}

void showGuestMenu() {
    std::cout << "\n=== Car Rental System (Guest) ===\n";
    std::cout << "1. Register (Customer)\n";
    std::cout << "2. Login\n";
    std::cout << "3. Browse Vehicles\n";
    std::cout << "0. Exit\n";
    std::cout << "Choice: ";
}

void showCustomerMenu() {
    std::cout << "\n=== Customer Menu ===\n";
    std::cout << "1. Browse Vehicles\n";
    std::cout << "2. View Vehicle Details\n";
    std::cout << "3. Create Reservation\n";
    std::cout << "4. View My Reservations\n";
    std::cout << "5. View My Rentals\n";
    std::cout << "6. Extend Rental\n";
    std::cout << "7. Make Payment\n";
    std::cout << "8. Export Data\n";
    std::cout << "9. Logout\n";
    std::cout << "Choice: ";
}

void showStaffMenu() {
    std::cout << "\n=== Staff Menu ===\n";
    std::cout << "1. Convert Reservation to Rental\n";
    std::cout << "2. Return Vehicle\n";
    std::cout << "3. Add Vehicle\n";
    std::cout << "4. Process Reservation Queues\n";
    std::cout << "5. Export Data\n";
    std::cout << "6. Logout\n";
    std::cout << "Choice: ";
}

void showMaintenanceMenu() {
    std::cout << "\n=== Maintenance Menu ===\n";
    std::cout << "1. Add Maintenance Record\n";
    std::cout << "2. Set Vehicle Maintenance Status\n";
    std::cout << "3. Export Data\n";
    std::cout << "4. Logout\n";
    std::cout << "Choice: ";
}

void showManagerMenu() {
    std::cout << "\n=== Manager Menu ===\n";
    std::cout << "1. Generate Revenue Report\n";
    std::cout << "2. Export Revenue Report CSV\n";
    std::cout << "3. Block User\n";
    std::cout << "4. Unblock User\n";
    std::cout << "5. Export Data\n";
    std::cout << "6. Logout\n";
    std::cout << "Choice: ";
}

int readIntSafe() {
    int v;
    while (!(std::cin >> v)) {
        std::cin.clear();
        std::cin.ignore(1024, '\n');
        std::cout << "Invalid input. Try again: ";
    }
    std::cin.ignore(1024, '\n');
    return v;
}

// Persistence policy:
// - On startup: importData(DATA_FILE) is called once to restore all state.
// - Autosave happens to DATA_FILE after every mutating operation:
//   * Successful customer registration
//   * Successful reservation creation / rental extension / payment
//   * Successful staff operations: convert reservation->rental, return vehicle, add vehicle, process queues
//   * Maintenance operations: add maintenance record, change maintenance status
//   * Manager operations: block/unblock users
//   * Any logout and on program exit
// - Manual export/import to custom filenames is still available via menu options.
int main() {
    System system;
    const std::string DATA_FILE = "data.txt";
    bool running = true;

    // Load existing data at startup
    std::cout << "Loading data from " << DATA_FILE << "...\n";
    if (system.importData(DATA_FILE)) {
        std::cout << "Data loaded successfully.\n";
    } else {
        std::cout << "No existing data file found. Starting with empty system.\n";
    }
    // Ensure default Manager/Staff exist for demo/administration use
    system.ensureBootstrapUsers();

    while (running) {
        User* current = system.getCurrentUser();
        if (!current) {
            showGuestMenu();
            int choice = readIntSafe();
            if (choice == 0) {
                // Auto-save before exit
                std::cout << "Saving data to " << DATA_FILE << "...\n";
                system.exportData(DATA_FILE);
                running = false;
            } else if (choice == 1) {
                std::string username = readLineNoComma("Username: ");
                std::string password = readLineNoComma("Password: ");
                std::string fullName = readLineNoComma("Full name: ");
                std::string email    = readLineNoComma("Email: ");
                if (system.registerUser(username, password, fullName, email, UserType::Customer)) {
                    // Auto-save after successful registration
                    system.exportData(DATA_FILE);
                }
            } else if (choice == 2) {
                std::string username = readLineNoComma("Username: ");
                std::string password = readLineNoComma("Password: ");
                system.login(username, password);
            } else if (choice == 3) {
                system.displayAllVehicles();
            }
        } else {
            UserType type = current->getType();
            if (type == UserType::Customer) {
                showCustomerMenu();
                int c = readIntSafe();
                if (c == 1) {
                    system.displayAllVehicles();
                } else if (c == 2) {
                    std::string vid = readLineNoComma("Vehicle ID: ");
                    system.displayVehicleDetails(vid);
                } else if (c == 3) {
                    std::string vid   = readLineNoComma("Vehicle ID: ");
                    std::string start = readLineNoComma("Start date (YYYY-MM-DD): ");
                    std::string end   = readLineNoComma("End date (YYYY-MM-DD): ");
                    if (system.createReservation(vid, start, end)) {
                        system.exportData(DATA_FILE);
                    }
                } else if (c == 4) {
                    system.displayUserReservations();
                } else if (c == 5) {
                    system.displayUserRentals();
                } else if (c == 6) {
                    std::string rentalId = readLineNoComma("Rental ID: ");
                    std::string newEnd   = readLineNoComma("New end date (YYYY-MM-DD): ");
                    if (system.extendRental(rentalId, newEnd)) {
                        system.exportData(DATA_FILE);
                    }
                } else if (c == 7) {
                    std::cout << "Amount: ";
                    double amt;
                    while (!(std::cin >> amt)) {
                        std::cin.clear();
                        std::cin.ignore(1024, '\n');
                        std::cout << "Invalid amount. Try again: ";
                    }
                    std::cin.ignore(1024, '\n');
                    if (system.makePayment(amt)) {
                        system.exportData(DATA_FILE);
                    }
                } else if (c == 8) {
                    std::string fname = readLineNoComma("Filename: ");
                    system.exportData(fname);
                } else if (c == 9) {
                    system.logout();
                    // Auto-save after logout
                    system.exportData(DATA_FILE);
                }
            } else if (type == UserType::Staff) {
                showStaffMenu();
                int c = readIntSafe();
                if (c == 1) {
                    std::string rid = readLineNoComma("Reservation ID: ");
                    if (system.convertReservationToRental(rid)) {
                        system.exportData(DATA_FILE);
                    }
                } else if (c == 2) {
                    std::string rentalId = readLineNoComma("Rental ID: ");
                    std::string retDate  = readLineNoComma("Return date (YYYY-MM-DD): ");
                    if (system.returnVehicle(rentalId, retDate)) {
                        system.exportData(DATA_FILE);
                    }
                } else if (c == 3) {
                    std::string vid   = readLineNoComma("Vehicle ID: ");
                    std::string brand = readLineNoComma("Brand: ");
                    std::string model = readLineNoComma("Model: ");
                    int year;
                    int typeInt;
                    double price;
                    std::cout << "Year: ";
                    year = readIntSafe();
                    std::cout << "Type (0=Sedan,1=SUV,2=Hatchback,3=Sports,4=Luxury): ";
                    typeInt = readIntSafe();
                    std::cout << "Price per day: ";
                    while (!(std::cin >> price)) {
                        std::cin.clear();
                        std::cin.ignore(1024, '\n');
                        std::cout << "Invalid price. Try again: ";
                    }
                    std::cin.ignore(1024, '\n');
                    if (system.addVehicle(vid, brand, model, year,
                                          static_cast<VehicleType>(typeInt), price)) {
                        system.exportData(DATA_FILE);
                    }
                } else if (c == 4) {
                    system.processReservationQueues();
                    system.exportData(DATA_FILE);
                } else if (c == 5) {
                    std::string fname = readLineNoComma("Filename: ");
                    system.exportData(fname);
                } else if (c == 6) {
                    system.logout();
                    // Auto-save after logout
                    system.exportData(DATA_FILE);
                }
            } else if (type == UserType::Maintenance) {
                showMaintenanceMenu();
                int c = readIntSafe();
                if (c == 1) {
                    std::string vid  = readLineNoComma("Vehicle ID: ");
                    std::string desc = readLineNoComma("Description: ");
                    double cost;
                    std::cout << "Cost: ";
                    while (!(std::cin >> cost)) {
                        std::cin.clear();
                        std::cin.ignore(1024, '\n');
                        std::cout << "Invalid cost. Try again: ";
                    }
                    std::cin.ignore(1024, '\n');
                    system.addMaintenanceRecord(vid, desc, cost);
                    system.exportData(DATA_FILE);
                } else if (c == 2) {
                    std::string vid = readLineNoComma("Vehicle ID: ");
                    int flag;
                    std::cout << "1 = In Maintenance, 0 = Available: ";
                    flag = readIntSafe();
                    system.setVehicleMaintenance(vid, flag == 1);
                    system.exportData(DATA_FILE);
                } else if (c == 3) {
                    std::string fname = readLineNoComma("Filename: ");
                    system.exportData(fname);
                } else if (c == 4) {
                    system.logout();
                    // Auto-save after logout
                    system.exportData(DATA_FILE);
                }
            } else if (type == UserType::Manager) {
                showManagerMenu();
                int c = readIntSafe();
                if (c == 1) {
                    system.generateRevenueReport();
                } else if (c == 2) {
                    std::string fname = readLineNoComma("Filename: ");
                    system.exportRevenueReportCSV(fname);
                } else if (c == 3) {
                    std::string uname = readLineNoComma("Username to block: ");
                    system.blockUser(uname);
                    system.exportData(DATA_FILE);
                } else if (c == 4) {
                    std::string uname = readLineNoComma("Username to unblock: ");
                    system.unblockUser(uname);
                    system.exportData(DATA_FILE);
                } else if (c == 5) {
                    std::string fname = readLineNoComma("Filename: ");
                    system.exportData(fname);
                } else if (c == 6) {
                    system.logout();
                    // Auto-save after logout
                    system.exportData(DATA_FILE);
                }
            } else {
                // Fallback: logout unknown type
                system.logout();
            }
        }
    }

    return 0;
}

