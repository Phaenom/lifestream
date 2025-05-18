#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <Arduino.h>
#include "IInputManager.h"
#include "Config.h"

/**
 * Handles rotary encoder input and button press detection.
 * Tracks rotation changes and distinguishes between short and long presses.
 */
class InputManager : public IInputManager {
public:
    /**
     * Initializes input hardware and state.
     * Should be called once during system startup.
     */
    void begin() override;

    /**
     * Polls hardware and updates input state.
     * Should be called frequently in the main loop.
     */
    void update() override;

    /**
     * Returns the net rotation delta since last read.
     * Positive or negative values indicate direction.
     */
    int getRotation() const override;

    /**
     * Returns true if a short button press occurred since the last read.
     */
    bool wasButtonShortPressed() const override;

    /**
     * Returns true if a long button press occurred since the last read.
     */
    bool wasButtonLongPressed() const override;

private:
    mutable int rotationDelta = 0;              // Accumulated rotation since last update
    bool buttonHeld = false;                    // True if button is currently held down
    unsigned long buttonPressTime = 0;          // Time when button was first pressed
    mutable bool shortPressDetected = false;    // Set if short press detected since last update
    mutable bool longPressDetected = false;     // Set if long press detected since last update
};

#endif