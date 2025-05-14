#ifndef SIMULATION_INPUT_MANAGER_H
#define SIMULATION_INPUT_MANAGER_H

#include <Arduino.h>
#include "IInputManager.h"

class SimulationInputManager : public IInputManager {
public:
    void begin() override;
    void update() override;
    int getRotation() const override;
    bool wasButtonShortPressed() const override;
    bool wasButtonLongPressed() const override;

#ifdef WOKWI
    void simulateRotation(int delta) override;
    void simulateButtonPress(bool held);
#endif

private:
    mutable int rotationDelta = 0;
    unsigned long buttonPressTime = 0;
    bool buttonHeld = false;
    mutable bool shortPressDetected = false;
    mutable bool longPressDetected = false;
};

#endif