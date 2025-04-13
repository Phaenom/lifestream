
#include <Arduino.h>
#include "src/NetworkManager.h"
#include "src/DisplayManager.h"
#include "src/InputManager.h"  // Placeholder for future input handling

NetworkArbiter network;
DisplayManager display;
InputManager input;  // Future use for button/encoder

// Life totals for all players
int life1 = 20, life2 = 20, life3 = 20, life4 = 20;
int currentTurnPlayerID = 0;
int myPlayerID = 0;

// Track last known states for detecting changes
DeviceRole lastRole = ROLE_UNDEFINED;
uint8_t lastPlayerID = 255;
int lastLife1 = -1, lastLife2 = -1, lastLife3 = -1, lastLife4 = -1;
int lastTurnPlayerID = -1;

void setup() {
    Serial.begin(115200);

    network.begin();
    display.begin();
    input.begin();  // Future expansion for input handling
}

void loop() {
    // Always update network logic first
    network.update();

    // Handle input (if implemented later)
    input.update();

    // For now, directly mirror network state to display variables
    life1 = network.lifeTotals[0];
    life2 = network.lifeTotals[1];
    life3 = network.lifeTotals[2];
    life4 = network.lifeTotals[3];
    currentTurnPlayerID = network.currentTurn;
    myPlayerID = network.getPlayerID();

    // Detect any state change
    bool changed = false;

    if (network.getRole() != lastRole || myPlayerID != lastPlayerID) changed = true;
    if (life1 != lastLife1 || life2 != lastLife2 || life3 != lastLife3 || life4 != lastLife4) changed = true;
    if (currentTurnPlayerID != lastTurnPlayerID) changed = true;

    // Update display only if something changed
    if (changed) {
        display.drawLifeCounter(life1, life2, life3, life4, currentTurnPlayerID, myPlayerID, network.getRole());

        lastRole = network.getRole();
        lastPlayerID = myPlayerID;
        lastLife1 = life1;
        lastLife2 = life2;
        lastLife3 = life3;
        lastLife4 = life4;
        lastTurnPlayerID = currentTurnPlayerID;
    }

    // Small delay for ESP-NOW stability and display efficiency
    delay(100);
}
