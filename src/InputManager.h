
#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <ESP32Encoder.h>
#include <ezButton.h>
//#include "pitches.h" // how to..l.?

/*
  InputManager Class (Stub)
  Designed to handle inputs from Rotary Encoder and Buttons.
  Currently empty for future expansion.
*/

class InputManager {
public:
	// class parameters
	ESP32Encoder encoder;
	//ezButton encoder_button;
	int mode;	
	
    // Setup hardware for inputs
    void begin();//(int DT, int CLK);

    // Update function to read inputs each loop
    long update();
	
	void set_mode(){
		// future
	}
	
	void reset();
};

#endif
