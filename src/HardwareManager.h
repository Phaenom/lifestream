
#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

// #include <ESP32Encoder.h>
// #include <ezButton.h>
//#include <Button.h>
//#include "pitches.h" // how to..l.?

#include <ESP32Encoder.h>
#include <Arduino.h>
#include <ezButton.h>
//# include "Config.h"

/*
  HardwareManager Class (Stub)
  Designed to handle output to LEDs and Buzzer.
  Designed to handle inputs from Rotary Encoder and Buttons.
*/

class HardwareManager {

public:
    HardwareManager(); // Constructor

    // class parameters

    // ENCODER STUFF
    ESP32Encoder encoder;
    int64_t oldPosition;
    int64_t newPosition;
    int64_t delta;

    // BUTTON STUFF
    ezButton encoder_button; 	// ENCODER BUTTON STUFF
    ezButton led_button; // LED BUTTON STUFF

    bool led_state; // LED state
    bool isPressed;
    bool isReleased;
    bool isLongPress; // what to use this for?
    long pressTime;
    long pressDuration;

    int mode; // game manager / network manager handles?


    // SETUP FUNCTIONS
    void begin();
    void update(bool new_state); // Update function to read inputs each loop

    // Update function to read inputs each loop
    long update_encoder();
    //bool update_button();

    bool get_encoder_button(); // rename button names?
    bool get_arcade_button();

    //void set_mode();
    void reset();

};

extern HardwareManager hardware;

#endif