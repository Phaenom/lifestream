#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <Arduino.h>
#include "IInputManager.h"

/**
 * InputManager handles rotary encoder input and button press detection.
 * It tracks rotation changes and distinguishes between short and long button presses.
 */
class InputManager : public IInputManager {
public:
    /**
     * Initializes the input manager, setting up necessary hardware or state.
     * Typically called once during system startup.
     */
    void begin() override;

    /**
     * Updates the input manager state by reading inputs.
     * Should be called regularly in the main loop.
     */
    void update() override;

    /**
     * Returns the amount of rotation detected since the last update.
     * Positive or negative values indicate direction.
     */
    int getRotation() const override;

    /**
     * Returns true if a short button press was detected since the last update.
     */
    bool wasButtonShortPressed() const override;

    /**
     * Returns true if a long button press was detected since the last update.
     */
    bool wasButtonLongPressed() const override;

private:
    mutable int rotationDelta = 0;             // Tracks the change in rotation since last update
    bool buttonHeld = false;           // Indicates if the button is currently held down
    unsigned long buttonPressTime = 0; // Timestamp when the button was pressed
    mutable bool shortPressDetected = false;  // Flag set when a short press is detected
    mutable bool longPressDetected = false;   // Flag set when a long press is detected
};

//extern InputManager input;

#endif