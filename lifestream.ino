/*
    LifeStream - ESP32 Multiplayer Life Counter
    A scalable, modular, and battery-friendly life counter system for tabletop games like Magic: The Gathering.
    Designed for ESP32 using a Waveshare 2.9" V2 ePaper Display with Rotary Encoder and Wi-Fi Multiplayer Sync via ESP-NOW.
    Modular Design for Scalability and Clean Code Architecture
*/

#include <Arduino.h>
#include "src/DisplayManager.h"
#include "src/InputManager.h"
#include "src/NetworkManager.h"
#include "src/OutputManager.h"
#include "src/GameState.h"
#include "src/Config.h"
#include "src/assets/mtg_logo_symbol.h"

// Instantiate managers
DisplayManager display;  // Responsible for all ePaper display handling
GameState game;          // Stores player life totals and turn information

OutputManager output;  // Responsible for all LED and buzzer output
InputManager hardware; // Responsible for all buttons, encoders, etc

// Variables to track previous state (for detecting changes)
int previousLife[4] = { -1, -1, -1, -1 };  // Store previous life totals of each player
int previousTurnPlayerID = -1;             // Store previous turn ownerc:\Users\Phaen\Documents\Workspace\lifestream\src\DisplayManager.cpp

int currentTurnPlayerID = 0;  // Static for now - will later handle real turn progression
int temp = 0;

// experimenting 
unsigned long previousMillis = 0;  
unsigned long interval = 3000;  // ms    
unsigned long testUpdate_interval = 1000; // ms
int test = 0;


// Arduino setup() runs once when the device starts
void setup() {
    Serial.begin(115200);
    Serial.println("LifeStream Setup Starting");

    // Initialize ePaper Display hardware
    display.begin();

    // Initialize the game with 4 players, each starting with 20 lifec:\Users\Phaen\Documents\Workspace\lifestream\src\DisplayManager.h
    game.reset(4, 20);

    // Force initial draw by syncing previous values
    for (int i = 0; i < game.playerCount; i++) {
        previousLife[i] = game.players[i].life;
    }
    previousTurnPlayerID = 0;  // Initial turn is with Planeswalker 1

    // Draw the initial screen with player life totals
    display.drawLifeCounter(
        game.players[0].life,
        game.players[1].life,
        game.players[2].life,
        game.players[3].life,
        previousTurnPlayerID, // Current turn player
        game.myPlayerID       // This device's player ID
    );

    // Put display to sleep after drawing (saves power)
    display.sleep();

    // Initialize the hardware inputs
	hardware.begin(); // add start HP if that makes sense for encoder
    output.begin(); // Initialize output hardware (LEDs, Buzzer, etc)

    previousMillis = millis(); // Update the last time the display was updated
    Serial.println("Setup complete");

}

// Arduino loop() runs repeatedly after setup()
void loop() {
    bool needsRedraw = false;  // Flag to track if screen update is required

    test+= hardware.update_encoder();
    if ( millis() - previousMillis > interval) {
        Serial.println("current encoder value: ");  // Debug message
        Serial.println(test);  // Debug message
        previousMillis = millis(); // Update the last time the display was updated
    } 
        // TEMPORARY code to test encoder
    // replace 0 with currentTurnPlayerID
    //game.players[0].life += hardware.update_encoder(); // Update player 1's life based on encoder input
    //game.players[0].life += hardware.update_encoder(); // Update player 1's life based on encoder input
    // Check if any player's life total has changed
    for (int i = 0; i < game.playerCount; i++) {
        if (previousLife[i] != game.players[i].life) {

            Serial.print("life change: ");
            Serial.println(game.players[0].life); // Debug output to monitor life changes
            
            previousLife[i] = game.players[i].life;  // Update tracked value
            needsRedraw = true;                      // Trigger redraw
        }
    }

    // check if turn end button is pressed
    // TEMP CODE REPLACE WITH LOGIC THAT IGNORES IF NOT YOUR TURN
    // CHANGE function name update button is confusing
    if (hardware.update_button()){
        temp += 1;        
        currentTurnPlayerID = temp;
        if (temp > 3){
            temp = 0;
        }
    } // Get the current turn player ID from the button input

    // Check if the turn has changed
    //int currentTurnPlayerID = 0;  // Static for now - will later handle real turn progression
    if (previousTurnPlayerID != currentTurnPlayerID) {
        previousTurnPlayerID = currentTurnPlayerID;  // Update tracked value
        needsRedraw = true;                          // Trigger redraw
    }

    // check if it is your turn and turn on/off LEDs
    // technically this send LED ON signal every loop- better way?
    if (currentTurnPlayerID == game.myPlayerID) {
        output.update(HIGH);  // Turn on LED or buzzer for your turn
    } else {
        output.update(LOW);  // Turn off LED or buzzer
    }

    // Only update the display if life totals or turn ownership changed
    // screen refresh rate locked to X ms
    if ( (needsRedraw) && (millis() - previousMillis > interval) ) { 
        Serial.println("health");
        Serial.println(game.players[0].life);  // Debug message

        // Serial.println("Updating display...");  // Debug message       
        // display.begin();  // Wake up ePaper display

        // display.drawLifeCounter(
        //     game.players[0].life,
        //     game.players[1].life,
        //     game.players[2].life,
        //     game.players[3].life,
        //     currentTurnPlayerID,  // Who's turn is it
        //     game.myPlayerID       // Your player ID
        // );

        // display.sleep();  // Put display back to sleep after update
        previousMillis = millis(); // Update the last time the display was updated

        // Serial.println("updated");  // Debug message       
    }
    


    delay(50); // Small delay to prevent tight loop execution
}