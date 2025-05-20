#include "NetworkManager.h"
#include "DeviceManager.h"
#include "GameState.h"
#include "GameSetup.h"

// Global instance of the NetworkManager
NetworkManager network;

// Special MAC address used for broadcasting messages to all peers
uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Internal: Initialize ESP-NOW subsystem, reset device on failure
void NetworkManager::setupESPNow() {
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Initialization Failed");
        ESP.restart();  // Force reboot to attempt clean re-initialization
    }
}

// Global receive handler; delegates handling to NetworkManager instance
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    extern NetworkManager network;
    network.onDataReceive(mac, incomingData, len);
}

// Entry point for network initialization: sets Wi-Fi mode, initializes ESP-NOW, and starts discovery
void NetworkManager::begin() {
    WiFi.mode(WIFI_STA);                      // Station mode is required for ESP-NOW
    setupESPNow();                            // Initialize ESP-NOW
    esp_now_register_recv_cb(OnDataRecv);     // Set up packet receive handler
    discoveryStartTime = millis();            // Start discovery timeout timer
}

// Evaluate whether to become host or client based on discovery window status
void NetworkManager::updateRole() {
    if (device.role != ROLE_UNDEFINED) return;

    unsigned long now = millis();
    if (!hostDetected && now - discoveryStartTime > 5000) {
        // No host seen after 5s → become host
        device.becomeHost();
    } else if (hostDetected && now - discoveryStartTime > 1000) {
        // Host seen → become client after brief delay
        device.becomeClient();
    }
}

// HOST: Broadcast full game state to all connected devices
void NetworkManager::sendGameState() {
    GameData data = {};
    data.messageType = MSG_TYPE_GAME_STATE;
    data.playerCount = playerCount;
    data.currentTurn = currentTurn;
    data.senderID = device.myPlayerID;

    // Copy life/poison state for each player
    for (int i = 0; i < 4; ++i) {
        data.lifeTotal[i] = gameState.getPlayerState(i).life;
        data.poisonTotal[i] = gameState.getPlayerState(i).poison;
    }

    esp_now_send(broadcastAddress, (uint8_t*)&data, sizeof(data));
}

// CLIENT: Notify host of updated life total
void NetworkManager::sendLifeUpdate(uint8_t life) {
    if (device.role == ROLE_CLIENT) {
        LifeChange change = {MSG_TYPE_LIFE_CHANGE, device.myPlayerID, life};
        esp_now_send(broadcastAddress, (uint8_t *)&change, sizeof(change));
    }
}

// CLIENT: Request host to advance turn to next player
void NetworkManager::sendTurnChange() {
    if (device.role == ROLE_CLIENT) {
        TurnChange change = {MSG_TYPE_TURN_CHANGE, device.myPlayerID};
        esp_now_send(broadcastAddress, (uint8_t *)&change, sizeof(change));
    }
}

// Core ESP-NOW packet handler. Routes by message type and device role.
void NetworkManager::onDataReceive(const uint8_t *mac, const uint8_t *incomingData, int len) {
    if (len < 1) return;
    uint8_t messageType = incomingData[0];

    switch (messageType) {
        case MSG_TYPE_HOST_ANNOUNCE:
            // Passive discovery: record host presence and MAC
            if (device.role == ROLE_UNDEFINED) {
                Serial.println("[Network] Host Announce Received!");
            }
            hostDetected = true;
            memcpy(hostMac, mac, 6);
            break;

        case MSG_TYPE_GAME_STATE:
            // CLIENT: Buffer full game state to apply after markReady
            if (device.role == ROLE_CLIENT && len >= sizeof(GameData)) {
                memcpy(&pendingGameState, incomingData, sizeof(GameData));
                hasPendingGameState = true;
                Serial.printf("[Network] Game state buffered: Turn=%d, Life=%d/%d/%d/%d\n",
                            pendingGameState.currentTurn,
                            pendingGameState.lifeTotal[0], pendingGameState.lifeTotal[1],
                            pendingGameState.lifeTotal[2], pendingGameState.lifeTotal[3]);
            }
            break;

        case MSG_TYPE_LIFE_CHANGE: {
            // HOST: Apply life update and rebroadcast full game state
            if (device.role == ROLE_HOST && len == sizeof(LifeChange)) {
                LifeChange* change = (LifeChange*)incomingData;
                Serial.printf("[Network] Host received life change: P%d → %d\n",
                            change->senderID, change->newLifeTotal);

                int current = gameState.getPlayerState(change->senderID).life;
                gameState.adjustLife(change->senderID, change->newLifeTotal - current);
                sendGameState();
            }
            break;
        }

        case MSG_TYPE_TURN_CHANGE:
            // HOST: Advance turn on valid client request
            if (device.role == ROLE_HOST && len == sizeof(TurnChange)) {
                currentTurn = (currentTurn + 1) % playerCount;
                sendGameState();
            }
            break;

        case MSG_TYPE_POISON_CHANGE: {
            // HOST: Apply poison update and rebroadcast full game state
            if (device.role == ROLE_HOST && len == sizeof(PoisonChange)) {
                PoisonChange* change = (PoisonChange*)incomingData;
                Serial.printf("[Network] Host received poison change: P%d → %d\n",
                            change->senderID, change->newPoison);
                int current = gameState.getPlayerState(change->senderID).poison;
                gameState.adjustPoison(change->senderID, change->newPoison - current);
                sendGameState();
            }
            break;
        }
    }
}

// Periodic logic for host announcements and heartbeat debug info
void NetworkManager::heartbeat() {
    static unsigned long lastAnnounce = 0;
    static unsigned long lastHeartbeat = 0;
    unsigned long now = millis();

    // HOST: Send beacon to signal presence
    if (device.role == ROLE_HOST && now - lastAnnounce > 1000) {
        HostAnnounce announce = { MSG_TYPE_HOST_ANNOUNCE };
        esp_now_send(broadcastAddress, (uint8_t*)&announce, sizeof(announce));
        lastAnnounce = now;
    }

    // Log basic network stats every 5 seconds
    if (now - lastHeartbeat > 5000) {
        uint8_t localMac[6];
        esp_read_mac(localMac, ESP_MAC_WIFI_STA);

        Serial.println("\n[Loop] Network Heartbeat");
        Serial.printf(" - Player ID : %d\n", device.getPlayerID());
        Serial.printf(" - Role      : %s\n", device.roleToString(device.getRole()));
        Serial.printf(" - MAC Addr  : %02X:%02X:%02X:%02X:%02X:%02X\n",
                      localMac[0], localMac[1], localMac[2],
                      localMac[3], localMac[4], localMac[5]);

        esp_now_peer_num_t peerCount;
        esp_now_get_peer_num(&peerCount);
        Serial.printf(" - ESP-NOW Peers: %d\n", peerCount.total_num);

        if (device.role == ROLE_CLIENT && hostMac[0] != 0) {
            Serial.printf(" - Host MAC  : %02X:%02X:%02X:%02X:%02X:%02X\n",
                          hostMac[0], hostMac[1], hostMac[2],
                          hostMac[3], hostMac[4], hostMac[5]);
        }

        lastHeartbeat = now;
    }
}

// Return pointer to host MAC (client only), or nullptr
const uint8_t* NetworkManager::getHostMAC() const {
    return (device.role == ROLE_CLIENT) ? hostMac : nullptr;
}

// Flag network to apply buffered game state (used in update loop)
void NetworkManager::markReady() {
    readyToApplyGameState = true;
}

// CLIENT: Apply buffered game state to local system and update display
void NetworkManager::applyPendingGameState() {
    if (!readyToApplyGameState || !hasPendingGameState) return;

    playerCount = pendingGameState.playerCount;
    currentTurn = pendingGameState.currentTurn;

    Serial.println("[Network] Applying pending game state...");
    Serial.printf("[Debug] Drawing state: Turn=%d, Life=%d/%d/%d/%d\n",
                  currentTurn, pendingGameState.lifeTotal[0],
                  pendingGameState.lifeTotal[1], pendingGameState.lifeTotal[2], pendingGameState.lifeTotal[3]);

    // Render each player's state on the display
    for (int i = 0; i < 4; ++i) {
        PlayerState state;
        state.life = pendingGameState.lifeTotal[i];
        state.poison = pendingGameState.poisonTotal[i];
        state.eliminated = (state.life <= 0);
        state.isTurn = (i == currentTurn);
        display.renderPlayerState(i, state);
    }

    display.flush();
    hasPendingGameState = false;
}

// CLIENT: Explicit life change message directed to known host MAC
void NetworkManager::sendLifeChangeRequest(uint8_t playerId, uint8_t newLifeTotal) {
    LifeChange change = {MSG_TYPE_LIFE_CHANGE, playerId, newLifeTotal};
    esp_err_t res = esp_now_send(hostMac, (uint8_t*)&change, sizeof(change));
    Serial.printf("[Network] Sent life update to host: P%d → %d (res=%d)\n", playerId, newLifeTotal, res);
}

// CLIENT: Explicit poison change message directed to known host MAC
void NetworkManager::sendPoisonChangeRequest(uint8_t playerId, uint8_t newPoison) {
    PoisonChange change = {MSG_TYPE_POISON_CHANGE, playerId, newPoison};
    esp_err_t res = esp_now_send(hostMac, (uint8_t*)&change, sizeof(change));
    Serial.printf("[Network] Sent poison update: P%d → %d (res=%d)\n", playerId, newPoison, res);
}
