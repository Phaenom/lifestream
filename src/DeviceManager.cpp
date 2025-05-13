#include "DeviceManager.h"

void DeviceManager::begin() {
    Serial.println("[DeviceManager] Initialized.");
}

void DeviceManager::assumeHostRole() {
    host = true;
    playerId = 0;
    Serial.println("[DeviceManager] Host role assumed. Player ID set to 0.");
}

void DeviceManager::assumePlayerRole(uint8_t id) {
    host = false;
    playerId = id;
    Serial.printf("[DeviceManager] Player role assumed. Player ID set to %u.\n", id);
}

bool DeviceManager::isHost() const {
    return host;
}

uint8_t DeviceManager::getPlayerId() const {
    return playerId;
}
