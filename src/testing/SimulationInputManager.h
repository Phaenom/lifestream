#ifndef SIMULATION_INPUT_MANAGER_H
#define SIMULATION_INPUT_MANAGER_H

#include <Arduino.h>
#include "IInputManager.h"

class SimulationInputManager : public IInputManager {
public:
    void begin() override;
    void update() override;
    int getRotation() override;
    bool wasButtonShortPressed() override;
    bool wasButtonLongPressed() override;

private:
    int rotationDelta = 0;
    unsigned long buttonPressTime = 0;
    bool buttonHeld = false;
    bool shortPressDetected = false;
    bool longPressDetected = false;
};

#endif