#include "InputManager.h"

#define USE_MOCK_INPUT  // Enable mock mode

#ifdef USE_MOCK_INPUT

void InputManager::begin() {
    Serial.println("\n[Mock] InputManager initialized.");
}

void InputManager::update() {
    if (Serial.available()) {
        char ch = Serial.read();
        if (ch == 'a') rotationDelta--;     // Rotate counter-clockwise
        if (ch == 'd') rotationDelta++;     // Rotate clockwise
        if (ch == 'b') buttonPressed = true; // Simulate button press
    }
}

int InputManager::getRotation() {
    int delta = rotationDelta;
    rotationDelta = 0;
    return delta;
}

bool InputManager::wasButtonPressed() {
    bool pressed = buttonPressed;
    buttonPressed = false;
    return pressed;
}

#else

const int ENCODER_PIN_A = 34;
const int ENCODER_PIN_B = 35;
const int BUTTON_PIN    = 32;

volatile int encoderPosition = 0;
volatile bool buttonFlag = false;

void IRAM_ATTR encoderISR() {
    int a = digitalRead(ENCODER_PIN_A);
    int b = digitalRead(ENCODER_PIN_B);
    encoderPosition += (a == b) ? 1 : -1;
}

void IRAM_ATTR buttonISR() {
    buttonFlag = true;
}

void InputManager::begin() {
    pinMode(ENCODER_PIN_A, INPUT_PULLUP);
    pinMode(ENCODER_PIN_B, INPUT_PULLUP);
    pinMode(BUTTON_PIN,    INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), encoderISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN),    buttonISR, FALLING);
}

void InputManager::update() {
    noInterrupts();
    rotationDelta = encoderPosition;
    encoderPosition = 0;
    buttonPressed = buttonFlag;
    buttonFlag = false;
    interrupts();
}

int InputManager::getRotation() {
    int delta = rotationDelta;
    rotationDelta = 0;
    return delta;
}

bool InputManager::wasButtonPressed() {
    bool pressed = buttonPressed;
    buttonPressed = false;
    return pressed;
}

#endif
