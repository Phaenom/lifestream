// BatteryManager monitors and reports battery voltage/status
#include "BatteryManager.h"

BatteryManager battery;

void BatteryManager::begin() {
    Serial.println("\n[Init] BatteryManager initialized.");
}
