#include "testing/SimulationInputManager.h"

void SimulationInputManager::begin() {
    Serial.println("\n[SimInput] Simulation input initialized.");
}

void SimulationInputManager::update() {
    if (Serial.available()) {
        char input = Serial.read();
        unsigned long now = millis();

        if (input == 'a') {
            rotationDelta--;
        } else if (input == 'd') {
            rotationDelta++;
        } else if (input == 'b') {
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
    }
}

int SimulationInputManager::getRotation() const {
    int delta = rotationDelta;
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