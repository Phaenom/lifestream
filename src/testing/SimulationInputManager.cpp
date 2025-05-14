#include "GameState.h"
#include "testing/SimulationInputManager.h"

void SimulationInputManager::begin() {
    Serial.println("\n[SimInput] Simulation input initialized.");
    Serial.printf("[InputManager] Update cycle at %lu ms\n", millis());
}

void SimulationInputManager::update() {
    if (Serial.available()) {
        char input = Serial.read();
        Serial.printf("[SimInput] Received char: 0x%02X (%c)\n", input, input);
        input = tolower(input);
        unsigned long now = millis();

        if (input == 'a') {
            simulateRotation(-1);
        } else if (input == 'd') {
            simulateRotation(1);
        }
#ifdef WOKWI
        else if (input == 'b') {
            simulateButtonPress(true);  // Simulate press
        } else if (input == 'r') {
            simulateButtonPress(false); // Simulate release
        }
#else
        else if (input == 'b') {
            buttonPressTime = now;
            buttonHeld = true;
        } else if (input == 'r') {
            if (buttonHeld) {
                unsigned long duration = now - buttonPressTime;
                if (duration >= 1000) longPressDetected = true;
                else shortPressDetected = true;
                buttonHeld = false;
            }
        }
#endif
    }

    // After processing input, if rotationDelta is nonzero, adjust game state
    extern GameState gameState;
    if (rotationDelta != 0) {
        Serial.printf("[SimInput] Adjusting life by %d\n", rotationDelta);
        gameState.adjustLife(rotationDelta);
        rotationDelta = 0;  // reset manually since getRotation() isn't called here
    }
}

int SimulationInputManager::getRotation() const {
    int delta = rotationDelta;
    // Serial.printf("[SimInput] getRotation: returning %d\n", delta);
    rotationDelta = 0;
    return delta;
}

bool SimulationInputManager::wasButtonShortPressed() const {
    bool result = shortPressDetected;
    shortPressDetected = false;
    return result;
}

bool SimulationInputManager::wasButtonLongPressed() const {
    bool result = longPressDetected;
    longPressDetected = false;
    return result;
}

#ifdef WOKWI
void SimulationInputManager::simulateRotation(int delta) {
    rotationDelta += delta;
    Serial.printf("[SimInput] simulateRotation called: delta=%d, new value=%d\n", delta, rotationDelta);
}

void SimulationInputManager::simulateButtonPress(bool held) {
    unsigned long now = millis();

    if (held && !buttonHeld) {
        // Begin button press
        buttonPressTime = now;
        buttonHeld = true;
    } else if (!held && buttonHeld) {
        // Release button and classify press type
        unsigned long duration = now - buttonPressTime;
        if (duration >= 1000) longPressDetected = true;
        else shortPressDetected = true;
        buttonHeld = false;
    }
}
#endif