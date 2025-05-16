#include "NetworkManager.h"
#include "GameSetup.h"
#include "GameState.h"
#include "DeviceManager.h"

extern GameSetup gameSetup;
extern GameState gameState;
extern DeviceManager device;

void NetworkManager::begin() {
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed");
        return;
    }
    esp_now_register_recv_cb(onDataReceived);
    Serial.println("[NetworkManager] ESP-NOW initialized and callback registered.");
}

void NetworkManager::onDataReceived(const uint8_t *mac, const uint8_t *data, int len) {
    Serial.printf("[NetworkManager] Packet received from MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    if (len < sizeof(GameSyncPacket)) return;

    GameSyncPacket packet;
    memcpy(&packet, data, sizeof(GameSyncPacket));
    Serial.printf("[NetworkManager] Packet type: %d, Target player: %d\n", packet.type, packet.playerId);

    if (packet.playerId >= 4) return;

    if (packet.type == PACKET_TYPE_SETUP) {
        Serial.printf("[NetworkManager] Received setup packet: playerCount=%d, life=%d\n", packet.playerCount, packet.life);
        device.setIsHost(false); // This device is now a client
        gameSetup.setFromNetwork(packet.playerCount, packet.life);
        gameState.begin(device.getPlayerId(), packet.life);
    } else if (packet.type == PACKET_TYPE_PLAYER_UPDATE) {
        Serial.printf("[NetworkManager] Received player update packet: playerId=%d, life=%d, poison=%d, isTurn=%d, eliminated=%d\n",
                      packet.playerId, packet.life, packet.poison, packet.isTurn, packet.eliminated);
        PlayerState newState;
        newState.life = packet.life;
        newState.poison = packet.poison;
        newState.isTurn = packet.isTurn;
        newState.eliminated = packet.eliminated;

        gameState.updateRemotePlayer(packet.playerId, newState);

    } else if (packet.type == PACKET_TYPE_TURN_REQUEST) {
        Serial.printf("[NetworkManager] Received turn request packet: playerId=%d\n", packet.playerId);
        if (device.isHost() && packet.playerId == gameState.getCurrentTurnPlayer()) {
            gameState.nextTurn();
        }
    } else if (packet.type == PACKET_TYPE_JOIN_REQUEST) {
        Serial.println("[NetworkManager] Received join request");
        if (device.isHost()) {
            uint8_t newId = nextAvailableId();
            if (newId < 4) {
                sendPlayerAssign(mac, newId);
            } else {
                Serial.println("[NetworkManager] No player slots available");
            }
        }
    } else if (packet.type == PACKET_TYPE_PLAYER_ASSIGN) {
        Serial.printf("[NetworkManager] Assigned playerId=%d\n", packet.playerId);
        device.assumePlayerRole(packet.playerId);
        gameState.begin(packet.playerId, gameSetup.getStartingLife());
    }
}

void NetworkManager::sendGameSetup(uint8_t playerCount, uint8_t startingLife) {
    GameSyncPacket packet = {};
    packet.type = PACKET_TYPE_SETUP;
    packet.playerCount = playerCount;
    packet.life = startingLife;

    Serial.printf("[NetworkManager] Sending setup packet: playerCount=%d, life=%d\n", playerCount, startingLife);
    esp_err_t result = esp_now_send(nullptr, (uint8_t*)&packet, sizeof(packet));
    Serial.printf("[NetworkManager] sendGameSetup result: %d\n", result);
}

void NetworkManager::sendGameState(uint8_t playerId, const PlayerState& state) {
    GameSyncPacket packet;
    packet.playerId = playerId;
    packet.type = PACKET_TYPE_PLAYER_UPDATE;
    packet.life = state.life;
    packet.poison = state.poison;
    packet.isTurn = state.isTurn;
    packet.eliminated = state.eliminated;

    Serial.printf("[NetworkManager] Sending game state: playerId=%d, life=%d, poison=%d, turn=%d, eliminated=%d\n",
                  playerId, state.life, state.poison, state.isTurn, state.eliminated);
    esp_err_t result = esp_now_send(nullptr, (uint8_t*)&packet, sizeof(packet));
    Serial.printf("[NetworkManager] sendGameState result: %d\n", result);
}

void NetworkManager::sendTurnAdvanceRequest(uint8_t playerId) {
    GameSyncPacket packet = {};
    packet.type = PACKET_TYPE_TURN_REQUEST;
    packet.playerId = playerId;

    Serial.printf("[NetworkManager] Sending turn request from playerId=%d\n", playerId);
    esp_err_t result = esp_now_send(nullptr, (uint8_t*)&packet, sizeof(packet));
    Serial.printf("[NetworkManager] sendTurnAdvanceRequest result: %d\n", result);
}

void NetworkManager::sendJoinRequest() {
    GameSyncPacket packet = {};
    packet.type = PACKET_TYPE_JOIN_REQUEST;
    Serial.println("[NetworkManager] Sending join request");
    esp_now_send(nullptr, (uint8_t*)&packet, sizeof(packet));
}

void NetworkManager::sendPlayerAssign(const uint8_t* dest, uint8_t playerId) {
    GameSyncPacket packet = {};
    packet.type = PACKET_TYPE_PLAYER_ASSIGN;
    packet.playerId = playerId;
    Serial.printf("[NetworkManager] Assigning player ID %d\n", playerId);
    esp_now_send(dest, (uint8_t*)&packet, sizeof(packet));
}

uint8_t NetworkManager::nextAvailableId() {
    static bool assigned[4] = {false};  // crude static tracker
    for (uint8_t i = 1; i < 4; ++i) {
        if (!assigned[i]) {
            assigned[i] = true;
            return i;
        }
    }
    return 255;  // invalid ID if all are taken
}

bool NetworkManager::hasHost() const {
    // TODO: Implement real peer discovery logic
    return false;
}

bool NetworkManager::hasReceivedGameParams() const {
    return gameSetup.isConfigured();
}