#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// =================================================================================
//
// ENUM DEFINITIONS
//
// =================================================================================

// Define possible roles this device can take in the networked game.
enum DeviceRole {
    ROLE_UNDEFINED, // Role not yet determined
    ROLE_HOST,      // This device acts as Host
    ROLE_CLIENT     // This device acts as Client
};

// =================================================================================
//
// MESSAGE TYPE DEFINITIONS
//
// =================================================================================

#define MSG_TYPE_HOST_ANNOUNCE  0x01
#define MSG_TYPE_GAME_STATE     0x02
#define MSG_TYPE_LIFE_CHANGE    0x03  // Client requests life change
#define MSG_TYPE_TURN_CHANGE    0x04  // Client requests turn change

// =================================================================================
//
// STRUCTURES USED FOR ESP-NOW COMMUNICATION
//
// =================================================================================

// Sent by the host to all clients to broadcast the current game state
typedef struct GameData{
    uint8_t messageType;
    uint8_t playerCount;
    uint8_t currentTurn;
    uint8_t lifeTotal[4];
    uint8_t senderID;
} GameData;

// Life total change request sent by Client to Host
typedef struct LifeChange{
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

// =================================================================================
//
// NETWORK MANAGER CLASS
//
// =================================================================================

class NetworkManager {
public:
    void begin();                           // Initialize networking and start discovery
    void update();                          // Called regularly from loop()
    DeviceRole getRole();                   // Returns the current device role
    uint8_t getPlayerID();                  // Returns assigned player ID

    void sendLifeUpdate(uint8_t life);      // Client requests life change
    void sendTurnChange();                  // Client requests turn change
    void sendGameState();                   // Host sends full GameData to Clients

    void onDataReceive(const uint8_t *mac, const uint8_t *incomingData, int len);  // Handle received messages

    // Public Game State Variables shared between Host and Clients
    uint8_t playerCount = 4;
    uint8_t currentTurn = 0;
    uint8_t lifeTotals[4] = {20, 20, 20, 20};
    DeviceRole role = ROLE_UNDEFINED;       // Current device role

private:
    uint8_t myPlayerID = 0;                 // Device's assigned player ID

    void setupESPNow();                     // Initialize ESP-NOW
    void startAsHost();                     // Set this device as Host
    void startAsClient();                   // Set this device as Client

    bool hostDetected = false;              // Flag to indicate if Host was detected during discovery
    unsigned long discoveryStartTime = 0;   // Time when discovery started
};

extern NetworkManager network;  // <-- external declaration

#endif


// =================================================================================
//
// NETWORK MANAGER IMPLEMENTATION
//      SECONDARY VERSION (NOT WORKING) LEFT FOR REFERENCE
//
// =================================================================================

/* #ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#pragma once

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "DisplayManager.h"
#include "esp_wifi.h"

constexpr uint8_t PACKET_TYPE_SETUP         = 0;
constexpr uint8_t PACKET_TYPE_PLAYER_UPDATE = 1;
constexpr uint8_t PACKET_TYPE_TURN_REQUEST  = 2;
constexpr uint8_t PACKET_TYPE_JOIN_REQUEST  = 3;
constexpr uint8_t PACKET_TYPE_PLAYER_ASSIGN = 4;

#pragma pack(push, 1)
struct GameSyncPacket {
    uint8_t playerId;
    uint8_t type;         // 0 = GameSetup, 1 = PlayerState update
    uint8_t playerCount;
    uint8_t life;
    uint8_t poison;
    uint8_t isTurn;
    uint8_t eliminated;
};
#pragma pack(pop)

class NetworkManager {
public:
    void begin();
    void sendGameState(uint8_t playerId, const PlayerState& state);
    void sendGameSetup(uint8_t playerCount, uint8_t startingLife);
    void sendTurnAdvanceRequest(uint8_t playerId);
    bool hasHost() const;
    bool hasReceivedGameParams() const;
    void sendJoinRequest();
    static void sendGameSetupTo(const uint8_t* dest, uint8_t playerCount, uint8_t startingLife);


private:
    static void onDataReceived(const uint8_t* mac, const uint8_t* data, int len);
    static void sendPlayerAssign(const uint8_t* dest, uint8_t playerId);
    static uint8_t nextAvailableId();
};

#endif */