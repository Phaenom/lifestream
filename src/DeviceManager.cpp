#include "DeviceManager.h"
#include "NetworkManager.h"

// Global DeviceManager instance
DeviceManager device;

void DeviceManager::begin() {
    Serial.println("[DeviceManager] Initialized.");
}

/* bool DeviceManager::isHost() const {
    return host;
}
 */

// Promote device to Host role; assign Player ID 0 and add broadcast peer
void DeviceManager::becomeHost() {
    Serial.println("\nBecoming HOST");
    role = ROLE_HOST;
    myPlayerID = 0; // Convention: host is always Player 0

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add broadcast peer");
    }
}

// Promote device to Client role; assign Player ID 1 (static for now)
void DeviceManager::becomeClient() {
    Serial.println("Becoming CLIENT");
    role = ROLE_CLIENT;
    myPlayerID = 1;

    // Add host as peer if MAC known
    if (network.hostMac[0] != 0) {
        esp_now_peer_info_t peerInfo = {};
        memcpy(peerInfo.peer_addr, network.hostMac, 6);
        peerInfo.channel = 0;
        peerInfo.encrypt = false;

        if (!esp_now_is_peer_exist(network.hostMac)) {
            esp_err_t res = esp_now_add_peer(&peerInfo);
            Serial.printf("[Network] Client added host peer (res=%d)\n", res);
        }
    } else {
        Serial.println("[Network] Warning: hostMac not set when becoming client");
    }
}

// Return current device role
DeviceRole DeviceManager::getRole() const {
    return role;
}

// Return current player ID
uint8_t DeviceManager::getPlayerID() const {
    return myPlayerID;
}

const char* DeviceManager::roleToString(DeviceRole role) {
    switch (role) {
        case ROLE_HOST: return "🧠 Host";
        case ROLE_CLIENT: return "🎮 Client";
        case ROLE_UNDEFINED: return "❓ Undefined";
        default: return "🚫 Unknown";
    }
}