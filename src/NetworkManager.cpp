#include "NetworkManager.h"
#include "GameSetup.h"
#include "GameState.h"
#include "DeviceManager.h"

extern GameSetup gameSetup;
extern GameState gameState;
extern DeviceManager device;

constexpr uint8_t PACKET_TYPE_SETUP = 0;
constexpr uint8_t PACKET_TYPE_PLAYER_UPDATE = 1;
constexpr uint8_t PACKET_TYPE_TURN_REQUEST = 2;

void onDataReceived(const uint8_t *mac, const uint8_t *data, int len) {
    if (len < sizeof(GameSyncPacket)) return;

    GameSyncPacket packet;
    memcpy(&packet, data, sizeof(GameSyncPacket));

    if (packet.playerId >= 4) return;

    if (packet.type == PACKET_TYPE_SETUP) {
        Serial.printf("Received setup packet: playerCount=%d, life=%d\n", packet.playerCount, packet.life);
        gameSetup.setFromNetwork(packet.playerCount, packet.life);
        gameState.begin(device.getPlayerId(), packet.life);

    } else if (packet.type == PACKET_TYPE_PLAYER_UPDATE) {
        Serial.printf("Received player update packet: playerId=%d, life=%d, poison=%d, isTurn=%d, eliminated=%d\n",
                      packet.playerId, packet.life, packet.poison, packet.isTurn, packet.eliminated);
        PlayerState newState;
        newState.life = packet.life;
        newState.poison = packet.poison;
        newState.isTurn = packet.isTurn;
        newState.eliminated = packet.eliminated;

        gameState.updateRemotePlayer(packet.playerId, newState);

    } else if (packet.type == PACKET_TYPE_TURN_REQUEST) {
        Serial.printf("Received turn request packet: playerId=%d\n", packet.playerId);
        if (device.isHost() && packet.playerId == gameState.getCurrentTurnPlayer()) {
            gameState.nextTurn();
        }
    }
}

void NetworkManager::begin() {
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed");
        return;
    }
    esp_now_register_recv_cb(onDataReceived);
}

void NetworkManager::sendGameSetup(uint8_t playerCount, uint8_t startingLife) {
    GameSyncPacket packet = {};
    packet.type = PACKET_TYPE_SETUP;
    packet.playerCount = playerCount;
    packet.life = startingLife;

    esp_err_t result = esp_now_send(nullptr, (uint8_t*)&packet, sizeof(packet));
    Serial.printf("sendGameSetup result: %d\n", result);
}

void NetworkManager::sendGameState(uint8_t playerId, const PlayerState& state) {
    GameSyncPacket packet;
    packet.playerId = playerId;
    packet.type = PACKET_TYPE_PLAYER_UPDATE;
    packet.life = state.life;
    packet.poison = state.poison;
    packet.isTurn = state.isTurn;
    packet.eliminated = state.eliminated;

    esp_err_t result = esp_now_send(nullptr, (uint8_t*)&packet, sizeof(packet));
    Serial.printf("sendGameState result: %d\n", result);
}

void NetworkManager::sendTurnAdvanceRequest(uint8_t playerId) {
    GameSyncPacket packet = {};
    packet.type = PACKET_TYPE_TURN_REQUEST;
    packet.playerId = playerId;

    esp_err_t result = esp_now_send(nullptr, (uint8_t*)&packet, sizeof(packet));
    Serial.printf("sendTurnAdvanceRequest result: %d\n", result);
}

bool NetworkManager::hasHost() const {
    // Placeholder logic: always return false to simulate no host present
    // Replace with actual peer discovery logic in the future
    return false;
}