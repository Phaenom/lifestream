
#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <ESP32Encoder.h>
#include <ezButton.h>
//#include <Button.h>
//#include "pitches.h" // how to..l.?

/*
  InputManager Class (Stub)
  Designed to handle inputs from Rotary Encoder and Buttons.
  Currently empty for future expansion.
*/

class InputManager {

private:
	long change;
	long oldPosition;
	long newPosition;

	long pressDuration;

public:
	InputManager(); // Constructor
	// class parameters
	
	// ENCODER STUFF
	ESP32Encoder encoder;
	
	//volatile long newPosition;

	//ezButton encoder_button;
	int mode;	

	// LED BUTTON STUFF
	//Button led_button;
	ezButton led_button;
	bool isPressed;
	bool isLongPress;
	long pressTime;

    // Setup hardware for inputs
    void begin();//(int DT, int CLK);

    // Update function to read inputs each loop
    long update_encoder();
	bool update_button();
	void set_mode();
	void reset();

};

#endif
