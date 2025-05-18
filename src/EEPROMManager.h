#ifndef EEPROMMANAGER_H
#define EEPROMMANAGER_H

#include <Arduino.h>

class EEPROMManager {
public:
    void begin();  // Initialize module
};

extern EEPROMManager eeprom; // External declaration of EEPROMManager instance

#endif // EEPROMMANAGER_H
