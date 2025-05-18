#ifndef BATTERYMANAGER_H
#define BATTERYMANAGER_H

#include <Arduino.h>

class BatteryManager {
public:
    void begin();  // Initialize module
};

extern BatteryManager battery; // External declaration of BatteryManager instance

#endif // BATTERYMANAGER_H
