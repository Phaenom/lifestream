#include "InputManager.h"
#include "Config.h"

//#include <ESP32Encoder.h> // https://github.com/madhephaestus/ESP32Encoder.git 


//DisplayManager::begin

// InputManager::InputManager(){

	//button_arcade = 
// }

InputManager::InputManager() : 
	led_button(EPD_BUTTON), encoder_button(EPD_BUTTON_ENCODER) {
	// Constructor
	// change = 0;
	// oldPosition = 0;
	// newPosition = 0;
	// pressDuration = 0;
	// mode = 0; // future
	isPressed = false;
	// isLongPress = false;
	// pressTime = 0;
}
void InputManager::begin(){
	//(int DT, int CLK) {
		
	// ROTARY ENCODER SETUP
	encoder.attachHalfQuad( EPD_DT, EPD_CLK ); 	//attachFullQuad option available
	encoder.clearCount(); // reset encoder count to 0
	encoder.setCount( 20*2 ); // replace with lifecount?
	
	//oldPosition = encoder.getCount() / 2;
	//newPosition = oldPosition;
	//change = 0;

	//mode = 0;
	
	// encoder button input
	//encoder_button 
	
	//ESP32Encoder::useInternalWeakPullResistors = puType::up;
	//encoder.setFilter(1023);
	//ezButton led_button(EPD_BUTTON); // Set the pin for the button
	//Button led_button(EPD_BUTTON); // Set the pin for the button
	led_button.setDebounceTime(50); // Set debounce time to 50ms
	encoder_button.setDebounceTime(50); // Set debounce time to 50ms
	//led_button.setLongPressTime(2000); // Set long press time to 1000ms
	//led_button.setPinMode(INPUT_PULLUP); // Set pin mode to INPUT_PULLUP

	//encoder_button.setPinMode(INPUT_PULLUP); // Set pin mode to INPUT_PULLUP
	//encoder_button.setPin(EPD_BUTTON); // Set the pin for the button	
	

	// LED BUTTON SET UP

}

// Update function to read inputs each loop
long InputManager::update_encoder() {
	// Future implementation

	newPosition = encoder.getCount()/ 2;
	if (newPosition != oldPosition) {
		//change = newPosition - oldPosition;
		oldPosition = newPosition;

		Serial.print("encoder pos: ");
		Serial.println(newPosition); // Debug output to monitor life changes
	}
//	Serial.println(encoder.getCount()/2); // Debug output to monitor life changes

	// change = 0;
	// if (newPosition != oldPosition) {
	// 	change = newPosition - oldPosition;
	// 	oldPosition = newPosition;
	// }
	// Serial.print("encoder change: ");
	// Serial.println(change); // Debug output to monitor life changes

	//change = 1; // debug
	return newPosition;
	// return change;
	//return newPosition;

// Do I want to report encoder count or delta count to game state?
// risks either way?

}

// long InputManager::update_encoder() {
// 	return encoder.getCount()/2; // return encoder count divided by 2
// }


bool InputManager::update_button() {
	// Check if the button is pressed 
	//isPressed = encoder_button.isReleased();

	// updates button state
	led_button.loop();
	encoder_button.loop();

	// don't really need this. just use *.isReleased()
	// maybe for long presses?
	
	isPressed = false; 
	// if (led_button.isReleased() || encoder_button.isReleased()){
	if (led_button.isPressed() || encoder_button.isPressed()){		
		isPressed = true; 
	}
	return isPressed;
}

void InputManager::set_mode(){
	mode = 0; // future - to rotate between HP and PSN
}

void InputManager::reset(){
	encoder.clearCount();
}
