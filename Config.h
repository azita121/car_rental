#ifndef CONFIG_H
#define CONFIG_H

#include <string>

// Business rules configuration
class Config {
public:
    // Rental duration limits
    static const int MIN_RENTAL_DAYS = 1;
    static const int MAX_RENTAL_DAYS = 30;
    
    // Late fee rate per day
    static const double LATE_FEE_RATE;
    
    // Reservation claim window (days)
    static const int RESERVATION_CLAIM_WINDOW = 3;
    
    // Maximum reservations per customer
    static const int MAX_RESERVATIONS_PER_CUSTOMER = 5;
    
    // Minimum payment threshold for blocking
    static const double BLOCK_THRESHOLD;
    
    static double getLateFeeRate() { return LATE_FEE_RATE; }
    static int getClaimWindow() { return RESERVATION_CLAIM_WINDOW; }
    static int getMaxReservations() { return MAX_RESERVATIONS_PER_CUSTOMER; }
    static double getBlockThreshold() { return BLOCK_THRESHOLD; }
};

#endif // CONFIG_H
