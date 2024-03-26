#include "task1-functions.h"
#include <iostream>
#include <math.h>
using namespace std;

#define KM_PER_MILE 1.609344

// setEngineMode - outputs the engine mode based on speed S (mph) and battery level B (%)
// Returns 0 for battery mode and 1 for petrol
int setEngineMode(int S, int B) {

    int E = 1; // Had to initialize E with a value to avoid runtime error
    int kmhPerHour = round(S * KM_PER_MILE); // Calculate km/h, rounded to the nearest int
    cout << kmhPerHour << endl; // Print km/h and end line

    // Determine engine mode based on speed and battery level

    if (0 <= S && S < 10) // If speed is between 0 and 10 mph
    {
        if (B > 25) { // If battery level is greater than 25%
            E = 0; // Set engine mode to battery
        }
    }
    else if (!(0 <= S && S < 10)) // If speed is between 0 and 10 mph is false
    {
        if (10 <= S && S < 30) // If speed is between 10 and 30 mph
        {
            if (B >= 45) // If battery level is greater than or equal to 45%
            {
                E = 0; // Set engine mode to battery
            }
            else if (!(B >= 45)) // If battery level is greater than or equal to 45% is false
            {
				E = 1; // Set engine mode to petrol
			}
        }
        else if (!(10 <= S && S < 30)) // If speed is between 10 and 30 mph is false
        {
            E = 1; // Set engine mode to petrol
        }
    }
        
    // Display the engine mode
    cout << (E == 0 ? "Battery" : "Petrol") << endl; // Display engine mode using terany operator and end line

    // Return engine mode
    return E;
}

