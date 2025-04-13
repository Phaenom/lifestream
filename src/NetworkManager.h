
#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// Enum to define possible roles for the device
enum DeviceRole {
    ROLE_UNDEFINED, // Role not yet determined
    ROLE_HOST,      // This device acts as Host
    ROLE_CLIENT     // This device acts as Client
};

// Message Types used for ESP-NOW communication
#define MSG_TYPE_HOST_ANNOUNCE  0x01  // Broadcast by Host
#define MSG_TYPE_GAME_STATE     0x02  // Host sends full game state
#define MSG_TYPE_LIFE_CHANGE    0x03  // Client requests life change
#define MSG_TYPE_TURN_CHANGE    0x04  // Client requests turn change

// Structure sent from Host to all Clients containing full game state
typedef struct GameData {
    uint8_t messageType;
    uint8_t playerCount;
    uint8_t currentTurn;
    uint8_t lifeTotals[4];
    uint8_t senderID;
} GameData;

// Life total change request sent by Client to Host
typedef struct LifeChange {
    uint8_t messageType;
    uint8_t senderID;
    uint8_t newLifeTotal;
} LifeChange;

// Turn pass request sent by Client to Host
typedef struct TurnChange {
    uint8_t messageType;
    uint8_t senderID;
} TurnChange;

// Host announcing itself for discovery
typedef struct HostAnnounce {
    uint8_t messageType;
} HostAnnounce;

class NetworkArbiter {
public:
    void begin();                     // Initialize networking and start discovery
    void update();                    // Called regularly from loop()
    DeviceRole getRole();             // Returns the current device role
    uint8_t getPlayerID();            // Returns assigned player ID

    void sendLifeUpdate(uint8_t life);  // Client requests life change
    void sendTurnChange();              // Client requests turn change
    void sendGameState();               // Host sends full GameData to Clients

    void onReceive(const uint8_t *mac, const uint8_t *incomingData, int len);  // Handle received messages

    // Public Game State Variables shared between Host and Clients
    uint8_t playerCount = 4;
    uint8_t currentTurn = 0;
    uint8_t lifeTotals[4] = {20, 20, 20, 20};

private:
    DeviceRole role = ROLE_UNDEFINED;  // Current device role
    uint8_t myPlayerID = 0;            // Device's assigned player ID

    void setupESPNow();        // Initialize ESP-NOW
    void startAsHost();        // Set this device as Host
    void startAsClient();      // Set this device as Client

    bool hostDetected = false;         // Flag to indicate if Host was detected during discovery
    unsigned long discoveryStartTime = 0;  // Time when discovery started
};

#endif
