
#ifndef OUTPUT_MANAGER_H
#define OUTPUT_MANAGER_H

// #include <ESP32Encoder.h>
// #include <ezButton.h>
//#include <Button.h>
//#include "pitches.h" // how to..l.?

#include <Arduino.h>
//# include "Config.h"

/*
  OutputManager Class (Stub)
  Designed to handle output to LEDs and Buzzer.
  Currently empty for future expansion.
*/

class OutputManager {

public:
	// OutputManager(); // Constructor
	// class parameters
    bool led_state; // LED state

	
    void begin(); // Setup hardware for inputs
    void update(bool new_state); // Update function to read inputs each loop
	void reset();

};

#endif
