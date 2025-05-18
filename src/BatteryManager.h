#ifndef BATTERYMANAGER_H
#define BATTERYMANAGER_H

#include <Arduino.h>

// Maybe: https://www.sparkfun.com/lithium-ion-battery-1500mah-iec62133-certified.html

class BatteryManager {
public:
    void begin();  // Initialize module
};

extern BatteryManager battery; // External declaration of BatteryManager instance

#endif // BATTERYMANAGER_H
