#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <Arduino.h>

class DeviceManager {
public:
    // Initialize device role state
    void begin();

    // Assign this device as the host
    void assumeHostRole();
    // Assign this device as a player with a specific ID
    void assumePlayerRole(uint8_t id);

    // Returns true if this device is the host
    bool isHost() const;
    // Returns the assigned player ID
    uint8_t getPlayerId() const;

    // Manually set host status (used during role transition)
    void setIsHost(bool value);

private:
    bool host = false;                // True if device is acting as host
    uint8_t playerId = 0;             // Assigned player ID (range: 0–3)
    unsigned long lastUpdateTime = 0; // Last activity timestamp (reserved for future use)
};

// Global instance of the device manager
extern DeviceManager device;

#endif // DEVICEMANAGER_H
