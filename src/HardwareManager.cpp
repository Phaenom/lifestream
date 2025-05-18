#include "HardwareManager.h"
#include "Config.h"

//#include <ESP32Encoder.h> // https://github.com/madhephaestus/ESP32Encoder.git 

HardwareManager::HardwareManager():
	led_button(EPD_BUTTON),
	encoder_button(EPD_BUTTON_ENCODER),
	encoder() 
	{
	// Constructor
	// change = 0;
	// oldPosition = 0;
	// newPosition = 0;
	// pressDuration = 0;
	// mode = 0; // future

	// ROTARY ENCODER SETUP
	encoder.attachHalfQuad( EPD_DT, EPD_CLK ); 	//attachFullQuad option available
	encoder.clearCount(); // reset encoder count to 0
	//encoder.attachFullQuad( EPD_DT, EPD_CLK ); 	//attachFullQuad option available

	oldPosition = 0; // initial position
	newPosition = 0;
	delta = 0;

	isPressed = false;
	// isLongPress = false;
	// pressTime = 0;
}

void HardwareManager::begin(){
	// THIS SHOULDNT DO ANYTHING SINCE I AM USING DELTA
	encoder.setCount(40); // set initial count to 40 (20 life * 2 for encoder)
	oldPosition = encoder.getCount()/ 2; // set old position to initial count divided by 2
	
	// BUITTON SETUP
	led_button.setDebounceTime(50); // Set debounce time to 50ms
	encoder_button.setDebounceTime(50); // Set debounce time to 50ms

    // future - create buzzer object
    // future - create LED object

    // LEDs
    pinMode(EPD_BUTTON_LED, OUTPUT);   // set ESP32 pin to output mode
    // pinMode(EPD_ONBOARD_LED, OUTPUT);   // set ESP32 pin to output mode

    // start OFF
    led_state = LOW; // Set initial LED state to LOW (off)
    digitalWrite(EPD_BUTTON_LED, led_state); 
    // digitalWrite(EPD_ONBOARD_LED, led_state);

    // BLINK ON STARTUP
    // digitalWrite(EPD_BUTTON_LED, LOW); 
    // digitalWrite(EPD_ONBOARD_LED, LOW);
    // delay(200); // wait for 0.2 seconds
    // digitalWrite(EPD_BUTTON_LED, HIGH); 
    // digitalWrite(EPD_ONBOARD_LED, HIGH);

    // BUZZ ON STARTUP
    // tone(EPD_BUZZ, 440, 100); // Set the buzzer to a frequency of 440Hz for 100ms

}

// Update function to read inputs each loop
void HardwareManager::update(bool new_state) {
    digitalWrite(EPD_BUTTON_LED, new_state); // Update the button LED state
    // buzzer code here
}

// Update function to read inputs each loop
long HardwareManager::update_encoder() {
	newPosition = encoder.getCount()/ 2;
	delta = newPosition - oldPosition; // calculate delta
	oldPosition = newPosition;
	return delta;
}

// GET BUTTON STATE EVERY LOOP
bool HardwareManager::get_encoder_button() {
	encoder_button.loop(); 	// updates button state- have to call!

	isReleased = false;
	if (encoder_button.isReleased()) isReleased = true;  // is this redundant? depends if we want to have a long press
	return isReleased; // danger to share this variable with both buttons? 
}

bool HardwareManager::get_arcade_button() { // inconsistent naming
	led_button.loop();

	isReleased = false;
	if (led_button.isReleased()) isReleased = true; // released or pressed?
	return isReleased; 
}

// RESET- not really used
void HardwareManager::reset(){
	encoder.clearCount();
    // turn off LEDs
    digitalWrite(EPD_BUTTON_LED, LOW); 
}