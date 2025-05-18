#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <Arduino.h>

// =================================================================================
//
// ENUM DEFINITIONS
//
// =================================================================================

// Device roles in the networked game.
enum DeviceRole {
    ROLE_UNDEFINED, // Role not yet determined
    ROLE_HOST,      // Acts as Host
    ROLE_CLIENT     // Acts as Client
};

// =================================================================================
//
// DEVICE MANAGER CLASS
//
// =================================================================================

class DeviceManager {
public:
    // DeviceManager handles all role-related identity for this device.
    // This includes role, player ID, and host/client transitions.

    void begin();                         // Initialize device state

    void setAsHost();                     // Assign this device as host
    void setPlayerID(uint8_t id);         // Assign this device a player ID

    bool isHost() const;                  // True if acting as host
    uint8_t getPlayerID() const;          // Get assigned player ID

    DeviceRole getRole() const;           // Get current role
    static const char* roleToString(DeviceRole role);

    void becomeHost();                    // Host transition logic
    void becomeClient();                  // Client transition logic

    DeviceRole role = ROLE_UNDEFINED;       // Current device role
    uint8_t myPlayerID = 0;

    private:
                   // Assigned player ID
    unsigned long lastUpdateTime = 0; // Last activity timestamp (reserved for future use)
};

// Global instance of the device manager
extern DeviceManager device;

#endif // DEVICEMANAGER_H
