// EEPROMManager manages persistent storage of settings
#include "EEPROMManager.h"
#include "Config.h"

EEPROMManager eeprom;

void EEPROMManager::begin() {
    LOGLN("\n[Init] EEPROMManager initialized.");
}
