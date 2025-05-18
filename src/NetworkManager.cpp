#include "NetworkManager.h"
#include "DeviceManager.h"
#include "GameState.h"
#include "GameSetup.h"

//extern DisplayManager display;

NetworkManager network;

uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Initialize ESP-NOW at a low level
void NetworkManager::setupESPNow() {
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Initialization Failed");
        ESP.restart();  // Reset device on failure to recover cleanly
    }
}

// ESP-NOW receive callback; forwards data to NetworkManager
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    extern NetworkManager network;
    network.onDataReceive(mac, incomingData, len);
}

// Initializes WiFi, ESP-NOW, and starts host discovery window
void NetworkManager::begin() {
    WiFi.mode(WIFI_STA);  // ESP-NOW only works in station (STA) mode
    setupESPNow();        // Initialize ESP-NOW layer
    esp_now_register_recv_cb(OnDataRecv); // Set up callback to receive packets
    discoveryStartTime = millis();        // Start a timer for host discovery timeout
}

// Determine device role; become host if no host detected after timeout
void NetworkManager::updateRole() {
    if (device.role != ROLE_UNDEFINED) return;  // Role already assigned

    // Wait for host announcements or timeout to become host
    unsigned long now = millis();
    if (!hostDetected && now - discoveryStartTime > 5000) {
        device.becomeHost();
    } else if (hostDetected && now - discoveryStartTime > 1000) {
        device.becomeClient();
    }
}

// Host: Broadcast full game state to all clients
void NetworkManager::sendGameState() {
    GameData data = {};
    data.messageType = MSG_TYPE_GAME_STATE;
    data.playerCount = playerCount;
    data.currentTurn = currentTurn;
    data.senderID = device.myPlayerID;

    for (int i = 0; i < 4; ++i) {
        data.lifeTotal[i] = gameState.getPlayerState(i).life;
        data.poisonTotal[i] = gameState.getPlayerState(i).poison;
    }

    esp_now_send(broadcastAddress, (uint8_t*)&data, sizeof(data));
}

// Client: Send new life total to host
void NetworkManager::sendLifeUpdate(uint8_t life) {
    if (device.role == ROLE_CLIENT) {
        LifeChange change = {MSG_TYPE_LIFE_CHANGE, device.myPlayerID, life};
        esp_now_send(broadcastAddress, (uint8_t *)&change, sizeof(change));
    }
}

// Client: Request to pass the turn to next player
void NetworkManager::sendTurnChange() {
    if (device.role == ROLE_CLIENT) {
        TurnChange change = {MSG_TYPE_TURN_CHANGE, device.myPlayerID};
        esp_now_send(broadcastAddress, (uint8_t *)&change, sizeof(change));
    }
}

// Handle incoming ESP-NOW data; route by message type and role
void NetworkManager::onDataReceive(const uint8_t *mac, const uint8_t *incomingData, int len) {
    if (len < 1) return;
    uint8_t messageType = incomingData[0];

    switch (messageType) {
        case MSG_TYPE_HOST_ANNOUNCE:
            if (device.role == ROLE_UNDEFINED) {
                Serial.println("[Network] Host Announce Received!");
            }
            hostDetected = true;

            memcpy(hostMac, mac, 6);    // Store the host MAC

            break;

        case MSG_TYPE_GAME_STATE:
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
            if (device.role == ROLE_HOST && len == sizeof(LifeChange)) {
                LifeChange* change = (LifeChange*)incomingData;
                Serial.printf("[Network] Host received life change: P%d → %d\n",
                            change->senderID, change->newLifeTotal);

                gameState.adjustLife(change->senderID, 
                    change->newLifeTotal - gameState.getPlayerState(change->senderID).life);
                sendGameState();
            }
            break;
        }

        case MSG_TYPE_TURN_CHANGE:
            // Host: advance turn on turn-change request
            if (device.role == ROLE_HOST && len == sizeof(TurnChange)) {
                currentTurn = (currentTurn + 1) % playerCount;
                sendGameState();
            }
            break;
        case MSG_TYPE_POISON_CHANGE: {
            if (device.role == ROLE_HOST && len == sizeof(PoisonChange)) {
                PoisonChange* change = (PoisonChange*)incomingData;
                Serial.printf("[Network] Host received poison change: P%d → %d\n",
                            change->senderID, change->newPoison);
                int current = gameState.getPlayerState(change->senderID).poison;
                int delta = change->newPoison - current;
                gameState.adjustPoison(change->senderID, delta);
                sendGameState();
            }
            break;
        }

    }
}

void NetworkManager::heartbeat() {
    static unsigned long lastAnnounce = 0;
    static unsigned long lastHeartbeat = 0;
    unsigned long now = millis();

    // Host: broadcast presence every second
    if (device.role == ROLE_HOST && now - lastAnnounce > 1000) {
        HostAnnounce announce = { MSG_TYPE_HOST_ANNOUNCE };
        esp_now_send(broadcastAddress, (uint8_t*)&announce, sizeof(announce));
        lastAnnounce = now;
    }

    // Print heartbeat info every 5 seconds
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

const uint8_t* NetworkManager::getHostMAC() const {
    return (device.role == ROLE_CLIENT) ? hostMac : nullptr;
}

void NetworkManager::markReady() {
    readyToApplyGameState = true;
}

void NetworkManager::applyPendingGameState() {
    if (!readyToApplyGameState || !hasPendingGameState) return;

    playerCount = pendingGameState.playerCount;
    currentTurn = pendingGameState.currentTurn;
    for (int i = 0; i < 4; i++) {
        lifeTotals[i] = pendingGameState.lifeTotal[i];
    }

    Serial.println("[Network] Applying pending game state...");

    Serial.printf("[Debug] &display: %p\n", (void*)&display);

    Serial.printf("[Debug] Drawing state: Turn=%d, Life=%d/%d/%d/%d\n",
                  currentTurn, lifeTotals[0], lifeTotals[1], lifeTotals[2], lifeTotals[3]);

    for (int i = 0; i < 4; ++i) {
        PlayerState state;
        state.life = pendingGameState.lifeTotal[i];
        state.poison = pendingGameState.poisonTotal[i];
        state.eliminated = (lifeTotals[i] <= 0);
        state.isTurn = (i == currentTurn);
        display.renderPlayerState(i, state);
    }

    display.flush();

    hasPendingGameState = false;
}

void NetworkManager::sendLifeChangeRequest(uint8_t playerId, uint8_t newLifeTotal) {
    LifeChange change;
    change.messageType = MSG_TYPE_LIFE_CHANGE;
    change.senderID = playerId;
    change.newLifeTotal = newLifeTotal;

    esp_err_t res = esp_now_send(hostMac, (uint8_t*)&change, sizeof(change));
    Serial.printf("[Network] Sent life update to host: P%d → %d (res=%d)\n", playerId, newLifeTotal, res);
}

void NetworkManager::sendPoisonChangeRequest(uint8_t playerId, uint8_t newPoison) {
    PoisonChange change;
    change.messageType = MSG_TYPE_POISON_CHANGE;
    change.senderID = playerId;
    change.newPoison = newPoison;

    esp_err_t res = esp_now_send(hostMac, (uint8_t*)&change, sizeof(change));
    Serial.printf("[Network] Sent poison update: P%d → %d (res=%d)\n", playerId, newPoison, res);
}
