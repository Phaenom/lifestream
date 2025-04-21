#include "InputManager.h"
#include "DisplayManager.h"
#include <Arduino.h>

// Initializes input system (simulated here)
void InputManager::begin() {
    serial.println("InputManager initialized with test mocks.");
}

int InputManager::getEncoderDelta() {
    if (Serial.available()) {
        char input = Serial.read();
        if (input == '+') return 1; // simulate the encoder turned clockwise
        if (input == '-') return -1; // simulate the encoder turned counter-clockwise
    }
    return 0; // No input
}

// Simulates a button press using the Enter key
bool InputManager::buttonPressed() {
    if (Serial.available()) {
        char input = Serial.read();
        return input == '\n'; //enter key
    }
    return false;
}

// Prompts user to select number of players (2–4)
uint8_t InputManager::selectNumberOfPlayers() {
    uint8_t count = 2;
    DisplayManager::showSelectedPlayers(count);

    while (true) {
        int delta = getEncoderDelta();
        if (delta != 0) {
            count = constrain(count + delta, 2, 4);
            DisplayManager::showSelectedPlayers(count);
        }
        if (buttonPressed()) return count;
    }
}

// Prompts user to select a starting life total (20 or 40)
int InputManager::selectStartingLife() {
    int options[] = {20, 40};
    int index = 0;
    DisplayManager::showSelectedLife(options[index]);

    while (true) {
        int delta = getEncoderDelta();
        if (delta != 0) {
            index = (index + delta + 2) % 2;
            DisplayManager::showSelectedLife(options[index]);
        }
        if (buttonPressed()) return options[index];
    }
}

// Prompts user to select a game type from a list
String InputManager::selectGameType() {
    String options[] = {"Standard", "Commander", "Custom"};
    int index = 0;
    DisplayManager::showSelectedGameType(options[index]);

    while (true) {
        int delta = getEncoderDelta();
        if (delta != 0) {
            index = (index + delta + 3) % 3;
            DisplayManager::showSelectedGameType(options[index]);
        }
        if (buttonPressed()) return options[index];
    }
}

// Prompts user to confirm all selected settings
bool InputManager::confirmGameSettings(const GameSettings& settings) {
    bool confirm = false;
    DisplayManager::showConfirmScreen(settings);

    while (true) {
        int delta = getEncoderDelta();
        if (delta != 0) {
            confirm = !confirm;
            DisplayManager::highlightConfirmation(confirm);
        }
        if (buttonPressed()) return confirm;
    }
}