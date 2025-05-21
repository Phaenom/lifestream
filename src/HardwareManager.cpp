#include "HardwareManager.h"
#include "Config.h"

#include "IInputManager.h"

#include "NetworkManager.h"
#include "GameState.h"
# include "DeviceManager.h"

//#include <ESP32Encoder.h> // https://github.com/madhephaestus/ESP32Encoder.git 

HardwareManager hardware; // Global instance of HardwareManager

HardwareManager::HardwareManager():
	led_button(PIN_ARCADE_BUTTON),
	encoder_button(PIN_ENCODER_BUTTON),
	encoder() 
	{
	// Constructor
	// change = 0;
	// oldPosition = 0;
	// newPosition = 0;
	// pressDuration = 0;
	// mode = 0; // future

	// ROTARY ENCODER SETUP
	encoder.attachHalfQuad( PIN_ENCODER_DT, PIN_ENCODER_CLK ); 	//attachFullQuad option available
	encoder.clearCount(); // reset encoder count to 0
	//encoder.attachFullQuad( EPD_DT, EPD_CLK ); 	//attachFullQuad option available

	oldPosition = 0; // initial position
	newPosition = 0;
	delta = 0;

	mode = 0;
	isPressed = false;
	// isLongPress = false;
	// pressTime = 0;
}

void HardwareManager::begin(){
	// THIS SHOULDNT DO ANYTHING SINCE I AM USING DELTA
	encoder.setCount(0); // set initial count x
	oldPosition = encoder.getCount()/ 2; // set old position to initial count divided by 2
	
	// BUITTON SETUP
	led_button.setDebounceTime(50); // Set debounce time to 50ms
	encoder_button.setDebounceTime(50); // Set debounce time to 50ms

    // future - create buzzer object
    // future - create LED object

    // LEDs
    pinMode(PIN_ARCADE_LED, OUTPUT);   // set ESP32 pin to output mode
    // pinMode(EPD_ONBOARD_LED, OUTPUT);   // set ESP32 pin to output mode

    // start OFF
    led_state = LOW; // Set initial LED state to LOW (off)
    digitalWrite(PIN_ARCADE_LED, led_state); 
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
void HardwareManager::update() {

	extern GameState gameState;
	extern NetworkManager network;
	extern DeviceManager device;

	// FUTURE-- pull functions out and add conditional that checks if in game setup mode, then remap

	// int myID = device.getPlayerID();
	// - myRole = device.getRole();

	// ENCODER BUTTON
	encoder_button.loop(); // update button state- have to call!
	if (encoder_button.isReleased()) {
		mode = !mode; // toggle between 0 and 1
		// mode = (mode + 1) % max_mode; // future for multiple modes
	}

	// ROTATY ENCODER
	delta = hardware.update_encoder(); // update encoder position
	if (delta != 0 ) {
		if ( mode = 0) { // LIFE 
			gameState.adjustLife(device.getPlayerID(), delta); // adjust life
			int newLife = gameState.getLife(device.getPlayerID());

			if (device.getRole() == ROLE_HOST) {
				network.sendGameState(); // HOST
			}

			if (device.getRole() == ROLE_CLIENT) {
				network.sendLifeChangeRequest(device.getPlayerID(), newLife); // CLIENT
			}

		} else if (mode = 1) { // POISON
			gameState.adjustPoison(device.getPlayerID(), delta); // adjust poison
			uint8_t newPoison = gameState.getPlayerState(device.getPlayerID()).poison;

			if (device.getRole() == ROLE_HOST) {
				network.sendGameState(); // HOST
			}

			if (device.getRole() == ROLE_CLIENT) {
				network.sendPoisonChangeRequest(device.getPlayerID(), newPoison); // CLIENT
			}
		}
	}
	
	// ARCADE BUTTON
	led_button.loop(); // update button state- have to call!
	led_state = LOW;
	// bool arc_state = hardware.get_arcade_button(); // update arcade button state
	if (led_button.isReleased()) {
		gameState.nextTurn(); 

		if (device.getRole() == ROLE_HOST) {
			network.sendGameState();

		} else if (device.getRole() == ROLE_CLIENT) {
			network.sendTurnChange();
		}
	}

	if (gameState.getCurrentTurnPlayer() == device.getPlayerID() ) {
		led_state = HIGH; // Set LED state to HIGH (on)
	} else {
		led_state = LOW; // Set LED state to LOW (off)
	}
	digitalWrite(PIN_ARCADE_LED, led_state); // Update the button LED state


    // buzzer code here
}

// BUTTON FUNCTIONS DEPRECATED- or move logic above into them
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
    digitalWrite(PIN_ARCADE_LED, LOW); 
}