#include "System.h"
#include <iostream>
using namespace std;

// Helper to safely read a line and strip/replace commas so CSV export stays valid.
// Commas entered by the user are replaced with spaces and documented as unsupported.
static string readLineNoComma(const string& prompt) {
    string line;
    cout << prompt;
    getline(cin, line);
    for (char& c : line) {
        if (c == ',') c = ' '; // commas are not allowed in stored text fields
    }
    return line;
}

void showGuestMenu() {
    cout << "\n=== Car Rental System (Guest) ===\n";
    cout << "1. Register (Customer)\n";
    cout << "2. Login\n";
    cout << "3. Browse Vehicles\n";
    cout << "0. Exit\n";
    cout << "Choice: ";
}

void showCustomerMenu() {
    cout << "\n=== Customer Menu ===\n";
    cout << "1. Browse Vehicles\n";
    cout << "2. View Vehicle Details\n";
    cout << "3. Create Reservation\n";
    cout << "4. View My Reservations\n";
    cout << "5. View My Rentals\n";
    cout << "6. Extend Rental\n";
    cout << "7. Make Payment\n";
    cout << "8. Export Data\n";
    cout << "9. Logout\n";
    cout << "Choice: ";
}

void showStaffMenu() {
    cout << "\n=== Staff Menu ===\n";
    cout << "1. Convert Reservation to Rental\n";
    cout << "2. Return Vehicle\n";
    cout << "3. Add Vehicle\n";
    cout << "4. Edit Vehicle\n";
    cout << "5. View Reservation Queue\n";
    cout << "6. Process Reservation Queues\n";
    cout << "7. Export Data\n";
    cout << "8. Logout\n";
    cout << "Choice: ";
}

void showMaintenanceMenu() {
    cout << "\n=== Maintenance Menu ===\n";
    cout << "1. Add Maintenance Record\n";
    cout << "2. Set Vehicle Maintenance Status\n";
    cout << "3. Export Data\n";
    cout << "4. Logout\n";
    cout << "Choice: ";
}

void showManagerMenu() {
    cout << "\n=== Manager Menu ===\n";
    cout << "1. Generate Revenue Report\n";
    cout << "2. Export Revenue Report CSV\n";
    cout << "3. Block User\n";
    cout << "4. Unblock User\n";
    cout << "5. Export Data\n";
    cout << "6. Logout\n";
    cout << "Choice: ";
}

int readIntSafe() {
    int v;
    while (!(cin >> v)) {
        cin.clear();
        cin.ignore(1024, '\n');
        cout << "Invalid input. Try again: ";
    }
    cin.ignore(1024, '\n');
    return v;
}

// Persistence policy:
// - On startup: importData(DATA_FILE) is called once to restore all state.
// - Autosave happens to DATA_FILE after every mutating operation:
//   * Successful customer registration
//   * Successful reservation creation / rental extension / payment
//   * Successful staff operations: convert reservation->rental, return vehicle, add vehicle, edit vehicle, process queues
//   * Maintenance operations: add maintenance record, change maintenance status
//   * Manager operations: block/unblock users
//   * Any logout and on program exit
// - Manual export/import to custom filenames is still available via menu options.
int main() {
    System system;
    const string DATA_FILE = "data.txt";
    bool running = true;

    // Load existing data at startup
    cout << "Loading data from " << DATA_FILE << "...\n";
    if (system.importData(DATA_FILE)) {
        cout << "Data loaded successfully.\n";
    } else {
        cout << "No existing data file found. Starting with empty system.\n";
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
                cout << "Saving data to " << DATA_FILE << "...\n";
                system.exportData(DATA_FILE);
                running = false;
            } else if (choice == 1) {
                string username = readLineNoComma("Username: ");
                string password = readLineNoComma("Password: ");
                string fullName = readLineNoComma("Full name: ");
                string email    = readLineNoComma("Email: ");
                if (system.registerUser(username, password, fullName, email, UserType::Customer)) {
                    // Auto-save after successful registration
                    system.exportData(DATA_FILE);
                }
            } else if (choice == 2) {
                string username = readLineNoComma("Username: ");
                string password = readLineNoComma("Password: ");
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
                    string vid = readLineNoComma("Vehicle ID: ");
                    system.displayVehicleDetails(vid);
                } else if (c == 3) {
                    string vid   = readLineNoComma("Vehicle ID: ");
                    string start = readLineNoComma("Start date (YYYY-MM-DD): ");
                    string end   = readLineNoComma("End date (YYYY-MM-DD): ");
                    if (system.createReservation(vid, start, end)) {
                        system.exportData(DATA_FILE);
                    }
                } else if (c == 4) {
                    system.displayUserReservations();
                } else if (c == 5) {
                    system.displayUserRentals();
                } else if (c == 6) {
                    string rentalId = readLineNoComma("Rental ID: ");
                    string newEnd   = readLineNoComma("New end date (YYYY-MM-DD): ");
                    if (system.extendRental(rentalId, newEnd)) {
                        system.exportData(DATA_FILE);
                    }
                } else if (c == 7) {
                    system.showPaymentSummaryForCurrentUser();
                    User* u = system.getCurrentUser();
                    if (u) {
                        double outstanding = u->getOutstandingBalance();
                        if (outstanding <= 0.0) {
                            cout << "You have no outstanding balance.\n";
                        } else {
                            char confirm;
                            cout << "Proceed with payment? (y/n): ";
                            cin >> confirm;
                            cin.ignore(1024, '\n');
                            if (confirm == 'y' || confirm == 'Y') {
                                cout << "Amount to pay (max " << outstanding << "): ";
                                double amt;
                                while (!(cin >> amt)) {
                                    cin.clear();
                                    cin.ignore(1024, '\n');
                                    cout << "Invalid amount. Try again: ";
                                }
                                cin.ignore(1024, '\n');
                                if (system.makePayment(amt)) {
                                    system.exportData(DATA_FILE);
                                }
                            } else {
                                cout << "Payment cancelled.\n";
                            }
                        }
                    }
                } else if (c == 8) {
                    string fname = readLineNoComma("Filename: ");
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
                    string rid = readLineNoComma("Reservation ID: ");
                    if (system.convertReservationToRental(rid)) {
                        system.exportData(DATA_FILE);
                    }
                } else if (c == 2) {
                    string rentalId = readLineNoComma("Rental ID: ");
                    string retDate  = readLineNoComma("Return date (YYYY-MM-DD): ");
                    if (system.returnVehicle(rentalId, retDate)) {
                        system.exportData(DATA_FILE);
                    }
                } else if (c == 3) {
                    string vid   = readLineNoComma("Vehicle ID: ");
                    string brand = readLineNoComma("Brand: ");
                    string model = readLineNoComma("Model: ");
                    int year;
                    int typeInt;
                    double price;
                    cout << "Year: ";
                    year = readIntSafe();
                    cout << "Type (0=Sedan,1=SUV,2=Hatchback,3=Sports,4=Luxury): ";
                    typeInt = readIntSafe();
                    cout << "Price per day: ";
                    while (!(cin >> price)) {
                        cin.clear();
                        cin.ignore(1024, '\n');
                        cout << "Invalid price. Try again: ";
                    }
                    cin.ignore(1024, '\n');
                    if (system.addVehicle(vid, brand, model, year,
                                          static_cast<VehicleType>(typeInt), price)) {
                        system.exportData(DATA_FILE);
                    }
                } else if (c == 4) {
                    string vid   = readLineNoComma("Vehicle ID to edit: ");
                    string brand = readLineNoComma("New brand: ");
                    string model = readLineNoComma("New model: ");
                    int year;
                    int typeInt;
                    double price;
                    cout << "New year: ";
                    year = readIntSafe();
                    cout << "New type (0=Sedan,1=SUV,2=Hatchback,3=Sports,4=Luxury): ";
                    typeInt = readIntSafe();
                    if (typeInt < 0 || typeInt > 4) {
                        cout << "Invalid type selection.\n";
                    } else {
                        cout << "New price per day: ";
                        while (!(cin >> price)) {
                            cin.clear();
                            cin.ignore(1024, '\n');
                            cout << "Invalid price. Try again: ";
                        }
                        cin.ignore(1024, '\n');
                        if (system.editVehicle(vid, brand, model, year,
                                               static_cast<VehicleType>(typeInt), price)) {
                            system.exportData(DATA_FILE);
                        }
                    }
                } else if (c == 5) {
                    string vid = readLineNoComma("Vehicle ID: ");
                    system.viewReservationQueue(vid);
                } else if (c == 6) {
                    system.processReservationQueues();
                    system.exportData(DATA_FILE);
                } else if (c == 7) {
                    string fname = readLineNoComma("Filename: ");
                    system.exportData(fname);
                } else if (c == 8) {
                    system.logout();
                    // Auto-save after logout
                    system.exportData(DATA_FILE);
                }
            } else if (type == UserType::Maintenance) {
                showMaintenanceMenu();
                int c = readIntSafe();
                if (c == 1) {
                    string vid  = readLineNoComma("Vehicle ID: ");
                    string desc = readLineNoComma("Description: ");
                    double cost;
                    cout << "Cost: ";
                    while (!(cin >> cost)) {
                        cin.clear();
                        cin.ignore(1024, '\n');
                        cout << "Invalid cost. Try again: ";
                    }
                    cin.ignore(1024, '\n');
                    system.addMaintenanceRecord(vid, desc, cost);
                    system.exportData(DATA_FILE);
                } else if (c == 2) {
                    string vid = readLineNoComma("Vehicle ID: ");
                    int flag;
                    cout << "1 = In Maintenance, 0 = Available: ";
                    flag = readIntSafe();
                    system.setVehicleMaintenance(vid, flag == 1);
                    system.exportData(DATA_FILE);
                } else if (c == 3) {
                    string fname = readLineNoComma("Filename: ");
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
                    string fname = readLineNoComma("Filename: ");
                    system.exportRevenueReportCSV(fname);
                } else if (c == 3) {
                    string uname = readLineNoComma("Username to block: ");
                    system.blockUser(uname);
                    system.exportData(DATA_FILE);
                } else if (c == 4) {
                    string uname = readLineNoComma("Username to unblock: ");
                    system.unblockUser(uname);
                    system.exportData(DATA_FILE);
                } else if (c == 5) {
                    string fname = readLineNoComma("Filename: ");
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

