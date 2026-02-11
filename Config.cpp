#include "Config.h"

// Define static const double members
// These must be defined in a .cpp file (not just declared in .h) because:
// 1. Floating-point constants cannot always be treated as compile-time constants
// 2. The linker needs actual memory locations for these symbols
// 3. When code references these values (especially via getter methods), the linker
//    needs to find their definitions to resolve the references
const double Config::LATE_FEE_RATE = 0.1; // 10% per day
const double Config::BLOCK_THRESHOLD = 100.0; // Block users with $100+ debt
