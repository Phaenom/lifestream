#include "InputManager.h"
#include "Config.h"

//#include <ESP32Encoder.h> // https://github.com/madhephaestus/ESP32Encoder.git 


//DisplayManager::begin

// InputManager::InputManager(){

	//button_arcade = 
// }
	
void InputManager::begin(){
	//(int DT, int CLK) {
		
	// rotary encoder input
	encoder.attachHalfQuad( EPD_DT, EPD_CLK ); 	//attachFullQuad option available
	encoder.clearCount(); // reset encoder count to 0
	//encoder.setCount( 20*2 ); // replace with lifecount?
	
	oldPosition = encoder.getCount() / 2;
	newPosition = oldPosition;
	change = 0;

	mode = 0;
	// encoder button input
	//encoder_button 
	
	//ESP32Encoder::useInternalWeakPullResistors = puType::up;
	//encoder.setFilter(1023);
}

// Update function to read inputs each loop
long InputManager::update() {
	// Future implementation

	newPosition = encoder.getCount()/ 2;
//	Serial.println(encoder.getCount()); // Debug output to monitor life changes
//	Serial.println(encoder.getCount()/2); // Debug output to monitor life changes

	change = 0;
	if (newPosition != oldPosition) {
		change = newPosition - oldPosition;
		oldPosition = newPosition;
	}
	Serial.println(change); // Debug output to monitor life changes

	change = 1; // debug
	
	return change;
	//return newPosition;

// Do I want to report encoder count or delta count to game state?
// risks either way?

}

void InputManager::set_mode(){
	mode = 0; // future
}

void InputManager::reset(){
	encoder.clearCount();
}
