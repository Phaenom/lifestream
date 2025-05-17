// EEPROMManager manages persistent storage of settings
#include "EEPROMManager.h"

EEPROMManager eeprom;

void EEPROMManager::begin() {
    Serial.println("\n[Init] EEPROMManager initialized.");
}
