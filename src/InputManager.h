#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <Arduino.h>

// InputManager handles rotary encoder rotation and button presses
class InputManager {
public:
	void begin();	// Initialize encoder and button GPIO
	void update();	// Update encoder state; call regularly
	int getRotation();	// Returns rotary movement since last call
	bool wasButtonPressed();	// Returns true once per button press

private:
	int lastEncoderState = 0;
	int rotationDelta = 0;
	bool buttonPressed = false;

	void IRAM_ATTR onEncoderInterrupt();	// ISR for encoder pin change
	void IRAM_ATTR onButtonInterrupt();		// ISR for button press
};

#endif // INPUT_MANAGER_H