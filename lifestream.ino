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
#include "src/GameState.h"
#include "src/Config.h"
#include "src/assets/mtg_logo_symbol.h"

// Instantiate managers
DisplayManager display;  // Responsible for all ePaper display handling
GameState game;          // Stores player life totals and turn information

InputManager hardware; // Responsible for all buttons, buzzers, encoders, etc

// Variables to track previous state (for detecting changes)
int previousLife[4] = { -1, -1, -1, -1 };  // Store previous life totals of each player
int previousTurnPlayerID = -1;             // Store previous turn ownerc:\Users\Phaen\Documents\Workspace\lifestream\src\DisplayManager.cpp

// Arduino setup() runs once when the device starts
void setup() {
    Serial.begin(115200);
    Serial.println("LifeStream Setup Starting");

    // Initialize ePaper Display hardware
    display.begin();

    // Initialize the game with 4 players, each starting with 20 lifec:\Users\Phaen\Documents\Workspace\lifestream\src\DisplayManager.h
    game.reset(4, 20);

	// Initialize the hardware inputs
	hardware.begin(); // add start HP if that makes sense for encoder

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
}

// Arduino loop() runs repeatedly after setup()
void loop() {
    bool needsRedraw = false;  // Flag to track if screen update is required

	previousLife[0] = hardware.update();
	
    // Check if any player's life total has changed
    for (int i = 0; i < game.playerCount; i++) {
        if (previousLife[i] != game.players[i].life) {
            previousLife[i] = game.players[i].life;  // Update tracked value
            needsRedraw = true;                      // Trigger redraw
        }
    }

    // Check if the turn has changed
    int currentTurnPlayerID = 0;  // Static for now - will later handle real turn progression
    if (previousTurnPlayerID != currentTurnPlayerID) {
        previousTurnPlayerID = currentTurnPlayerID;  // Update tracked value
        needsRedraw = true;                          // Trigger redraw
    }

    // Only update the display if life totals or turn ownership changed
    if (needsRedraw) {
        display.begin();  // Wake up ePaper display

        display.drawLifeCounter(
            game.players[0].life,
            game.players[1].life,
            game.players[2].life,
            game.players[3].life,
            currentTurnPlayerID,  // Who's turn is it
            game.myPlayerID       // Your player ID
        );

        display.sleep();  // Put display back to sleep after update
    }

    delay(100); // Small delay to prevent tight loop execution
}