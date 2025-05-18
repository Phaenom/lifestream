// InputManager.cpp
// This source file implements the InputManager class, which handles rotary encoder input and button press detection.
// It manages encoder rotation counting via interrupts and distinguishes between short and long button presses.

#include <Arduino.h>
#include "InputManager.h"
#include "Config.h"

// Pin connected to rotary encoder output A
const int ENCODER_PIN_A = 34;
// Pin connected to rotary encoder output B
const int ENCODER_PIN_B = 35;
// Pin connected to the push button
const int BUTTON_PIN    = 32;

// Volatile variable to store the encoder position updated in ISR
volatile int encoderPosition = 0;

// Interrupt Service Routine to handle encoder rotation changes
// Increments or decrements encoderPosition based on the state of encoder pins
void IRAM_ATTR encoderISR() {
    int a = digitalRead(ENCODER_PIN_A);
    int b = digitalRead(ENCODER_PIN_B);
    encoderPosition += (a == b) ? 1 : -1;
}

void InputManager::begin() {
    // Configure encoder pins as input with pull-up resistors
    pinMode(ENCODER_PIN_A, INPUT_PULLUP);
    pinMode(ENCODER_PIN_B, INPUT_PULLUP);
    // Configure button pin as input with pull-up resistor
    pinMode(BUTTON_PIN,    INPUT_PULLUP);

    // Attach interrupt to encoder pin A to detect changes and call encoderISR
    attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), encoderISR, CHANGE);

    Serial.println("[InputManager] Hardware input initialized (encoder + button)");
}

void InputManager::update() {
    noInterrupts();
    rotationDelta = encoderPosition;
    encoderPosition = 0;
    interrupts();

    //Serial.printf("[InputManager] Rotation delta: %d\n", rotationDelta);

    // Static variable to remember last button state across calls
    static bool lastButton = HIGH;
    // Read current button state
    bool currentButton = digitalRead(BUTTON_PIN);
    // Get current time in milliseconds
    unsigned long now = millis();

    // Detect button press (transition from HIGH to LOW)
    if (currentButton == LOW && lastButton == HIGH) {
        buttonPressTime = now;  // Record time when button was pressed
        buttonHeld = true;
        Serial.println("[InputManager] Button pressed.");
    }

    // Detect button release (transition from LOW to HIGH)
    if (currentButton == HIGH && lastButton == LOW) {
        unsigned long pressTime = now - buttonPressTime;  // Calculate press duration
        if (pressTime >= 1000) {
            longPressDetected = true;  // Long press if held >= 1000 ms
            Serial.printf("[InputManager] Button released after %lu ms (long press)\n", pressTime);
        } else {
            shortPressDetected = true;                   // Otherwise, short press
            Serial.printf("[InputManager] Button released after %lu ms (short press)\n", pressTime);
        }
        buttonHeld = false;
    }

    // Update last button state for next update cycle
    lastButton = currentButton;
}

// Returns the amount of rotation detected since the last call and resets the delta
int InputManager::getRotation() const {
    #if SIMULATION_ENABLED
        return simInput.getRotation();
    #else
        return hardware.update_encoder();
    #endif
}


// Returns true if a short button press was detected since the last call, then clears the flag
bool InputManager::wasButtonShortPressed() const {
    bool result = shortPressDetected;
    if (result) Serial.println("[InputManager] Detected short press event");
    shortPressDetected = false;
    return result;
}

// Returns true if a long button press was detected since the last call, then clears the flag
bool InputManager::wasButtonLongPressed() const {
    bool result = longPressDetected;
    if (result) Serial.println("[InputManager] Detected long press event");
    longPressDetected = false;
    return result;
}