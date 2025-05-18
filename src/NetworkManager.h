#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// Global broadcast address for ESP-NOW
extern uint8_t broadcastAddress[6];

// =================================================================================
//
// MESSAGE TYPE DEFINITIONS
//
// =================================================================================

#define MSG_TYPE_HOST_ANNOUNCE  0x01
#define MSG_TYPE_GAME_STATE     0x02
#define MSG_TYPE_LIFE_CHANGE    0x03  // Client requests life change
#define MSG_TYPE_TURN_CHANGE    0x04  // Client requests turn change
#define MSG_TYPE_POISON_CHANGE  0x05

// =================================================================================
//
// ESP-NOW COMMUNICATION STRUCTURES
//
// =================================================================================

// Broadcast by Host to all Clients with current game state
typedef struct GameData {
    uint8_t messageType;
    uint8_t playerCount;
    uint8_t currentTurn;
    uint8_t lifeTotal[4];
    uint8_t poisonTotal[4];
    uint8_t senderID;
} GameData;

// Life total change request from Client to Host
typedef struct LifeChange {
    uint8_t messageType;
    uint8_t senderID;
    uint8_t newLifeTotal;
} LifeChange;

// Turn pass request from Client to Host
typedef struct TurnChange {
    uint8_t messageType;
    uint8_t senderID;
} TurnChange;

// Host announcement for discovery
typedef struct HostAnnounce {
    uint8_t messageType;
} HostAnnounce;

// Poison total change request from Client to Host
typedef struct PoisonChange {
    uint8_t messageType;    // = MSG_TYPE_POISON_CHANGE
    uint8_t senderID;
    uint8_t newPoison;
} PoisonChange;

// =================================================================================
//
// NETWORK MANAGER CLASS
//
// =================================================================================

class NetworkManager {
public:
    void begin();                           // Initialize networking and start discovery
    void updateRole();                      // Called regularly from loop()

    void sendLifeUpdate(uint8_t life);     // Client requests life change
    void sendTurnChange();                  // Client requests turn change
    void sendGameState();                   // Host sends full GameData to Clients

    void onDataReceive(const uint8_t *mac, const uint8_t *incomingData, int len);  // Handle received messages

    // Shared game state variables
    uint8_t playerCount = 4;
    uint8_t currentTurn = 0;
    uint8_t lifeTotals[4] = {20, 20, 20, 20};

    void heartbeat();                       // Called regularly in loop()
    const uint8_t* getHostMAC() const;

    // Pending game state handling
    GameData pendingGameState;
    bool hasPendingGameState = false;
    bool readyToApplyGameState = false;    // Wait until display is ready

    void markReady();                       // Called once display is initialized
    void applyPendingGameState();

    void sendLifeChangeRequest(uint8_t playerId, uint8_t newLifeTotal);
    void sendPoisonChangeRequest(uint8_t playerId, uint8_t newPoison);

    uint8_t hostMac[6] = {0};               // MAC address of detected Host

private:
    void setupESPNow();                     // Initialize ESP-NOW
    bool hostDetected = false;              // Indicates if Host was detected during discovery
    unsigned long discoveryStartTime = 0;  // Discovery start time
};

// Global instance of the device manager
extern NetworkManager network;

#endif