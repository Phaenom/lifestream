#include "OutputManager.h"
#include "Config.h"


void OutputManager::begin(){
	
    // future - create buzzer object
    // future - create LED object

    // LEDs
    pinMode(EPD_BUTTON_LED, OUTPUT);   // set ESP32 pin to output mode
    pinMode(EPD_ONBOARD_LED, OUTPUT);   // set ESP32 pin to output mode

    // BLINK ON STARTUP
    digitalWrite(EPD_BUTTON_LED, LOW); 
    digitalWrite(EPD_ONBOARD_LED, LOW);
    delay(200); // wait for 0.2 seconds
    digitalWrite(EPD_BUTTON_LED, HIGH); 
    digitalWrite(EPD_ONBOARD_LED, HIGH);

    // revert to OFF
    led_state = LOW; // Set initial LED state to LOW (off)
    digitalWrite(EPD_BUTTON_LED, led_state); 
    digitalWrite(EPD_ONBOARD_LED, led_state);


    // BUZZ ON STARTUP
    // tone(EPD_BUZZ, 440, 100); // Set the buzzer to a frequency of 440Hz for 100ms
    // tone(EPD_BUZZ, 1000, 100); // Set the buzzer to a frequency of 1000Hz for 100ms
    // tone(EPD_BUZZ, 440, 100); // Set the buzzer to a frequency of 440Hz for 100ms
}

// Update function to read inputs each loop
void OutputManager::update(bool new_state) {
	// Future implementation
    if (new_state != led_state) {
        digitalWrite(EPD_BUTTON_LED, new_state); // Update the button LED state
        digitalWrite(EPD_ONBOARD_LED, new_state); // Update the onboard LED state
        led_state = new_state; // Update the stored LED state
    }   
    // if (led_state == HIGH) {
    //     tone(EPD_BUZZ, 440, 100); // Set the buzzer to a frequency of 440Hz for 100ms
    // }    
}

void OutputManager::reset(){
    // turn off LEDs
    digitalWrite(EPD_BUTTON_LED, LOW); 
    digitalWrite(EPD_ONBOARD_LED, LOW);
    //tone(EPD_BUZZ, 100, 100); // Set the buzzer to a frequency of 440Hz for 100ms

}
