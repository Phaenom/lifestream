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
	encoder.attachHalfQuad( DT, CLK );
	encoder.setCount( 20*2 ); // replace with lifecount?
	
	// encoder button input
	//encoder_button 
	
	//ESP32Encoder::useInternalWeakPullResistors = puType::up;
	//encoder.setFilter(1023);
}

// Update function to read inputs each loop
long InputManager::update() {
	// Future implementation
	long newPosition = encoder.getCount() / 2;
	
	return newPosition;

// Do I want to report encoder count or delta count to game state?
// risks either way?

}

void InputManager::set_mode(){
	
}

void InputManager::reset(){
	encoder.clearCount();
}
