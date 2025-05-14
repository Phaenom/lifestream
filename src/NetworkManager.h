#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>
#include <esp_now.h>
#include "DisplayManager.h"

constexpr uint8_t PACKET_TYPE_SETUP         = 0;
constexpr uint8_t PACKET_TYPE_PLAYER_UPDATE = 1;
constexpr uint8_t PACKET_TYPE_TURN_REQUEST  = 2;

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
#ifdef WOKWI
    bool hasReceivedGameParams() const { return true; }
#endif
    void begin();
    void sendGameState(uint8_t playerId, const PlayerState& state);
    void sendGameSetup(uint8_t playerCount, uint8_t startingLife);
    void sendTurnAdvanceRequest(uint8_t playerId);
    bool hasHost() const;
};

#endif