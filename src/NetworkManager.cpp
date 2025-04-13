#include "NetworkManager.h"

// Broadcast address for sending messages to all nearby ESP-NOW devices
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Receive callback function using new ESP-NOW API (esp_now_recv_info_t)
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
    extern NetworkArbiter network;
    network.onReceive(info->src_addr, incomingData, len);
}

// Initialize WiFi and ESP-NOW
void NetworkArbiter::begin() {
    WiFi.mode(WIFI_STA);  // ESP-NOW requires STA mode
    setupESPNow();        // Initialize ESP-NOW

    Serial.println("Searching for Host...");
    esp_now_register_recv_cb(OnDataRecv); // Register receive callback

    discoveryStartTime = millis();  // Record discovery start time
}

// Main update loop handling role logic and data exchange
void NetworkArbiter::update() {
    if (role == ROLE_UNDEFINED) {
        if (millis() - discoveryStartTime > 5000) {
            if (hostDetected) {
                startAsClient();
            } else {
                startAsHost();
            }
        }
    }

    if (role == ROLE_HOST) {
        static unsigned long lastAnnounce = 0;
        if (millis() - lastAnnounce > 1000) {
            HostAnnounce announce = {MSG_TYPE_HOST_ANNOUNCE};
            esp_now_send(broadcastAddress, (uint8_t *)&announce, sizeof(announce));
            sendGameState();
            lastAnnounce = millis();
        }
    }
}

// ESP-NOW initialization using modern API
void NetworkArbiter::setupESPNow() {
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Initialization Failed");
        ESP.restart();
    }
}

// Promote device to Host and add broadcast peer
void NetworkArbiter::startAsHost() {
    Serial.println("Becoming HOST");
    role = ROLE_HOST;
    myPlayerID = 0; // Host always has Player ID 0

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add broadcast peer");
    }
}

// Set this device as Client
void NetworkArbiter::startAsClient() {
    Serial.println("Becoming CLIENT");
    role = ROLE_CLIENT;
    myPlayerID = 1;  // Static ID for simplicity (can be improved)
}

// Host sends full game state to all Clients
void NetworkArbiter::sendGameState() {
    if (role == ROLE_HOST) {
        GameData data = {MSG_TYPE_GAME_STATE, playerCount, currentTurn,
                         {lifeTotals[0], lifeTotals[1], lifeTotals[2], lifeTotals[3]}, myPlayerID};
        esp_now_send(broadcastAddress, (uint8_t *)&data, sizeof(data));
    }
}

// Client requests life total update
void NetworkArbiter::sendLifeUpdate(uint8_t life) {
    if (role == ROLE_CLIENT) {
        LifeChange change = {MSG_TYPE_LIFE_CHANGE, myPlayerID, life};
        esp_now_send(broadcastAddress, (uint8_t *)&change, sizeof(change));
    }
}

// Client requests to pass turn
void NetworkArbiter::sendTurnChange() {
    if (role == ROLE_CLIENT) {
        TurnChange change = {MSG_TYPE_TURN_CHANGE, myPlayerID};
        esp_now_send(broadcastAddress, (uint8_t *)&change, sizeof(change));
    }
}

// Handle incoming ESP-NOW data based on message type
void NetworkArbiter::onReceive(const uint8_t *mac, const uint8_t *incomingData, int len) {
    if (len < 1) return;
    uint8_t messageType = incomingData[0];

    switch (messageType) {
        case MSG_TYPE_HOST_ANNOUNCE:
            hostDetected = true;
            break;

        case MSG_TYPE_GAME_STATE:
            if (role == ROLE_CLIENT && len == sizeof(GameData)) {
                GameData *data = (GameData *)incomingData;
                playerCount = data->playerCount;
                currentTurn = data->currentTurn;
                for (int i = 0; i < 4; i++) {
                    lifeTotals[i] = data->lifeTotals[i];
                }
            }
            break;

        case MSG_TYPE_LIFE_CHANGE:
            if (role == ROLE_HOST && len == sizeof(LifeChange)) {
                LifeChange *change = (LifeChange *)incomingData;
                lifeTotals[change->senderID] = change->newLifeTotal;
                sendGameState();
            }
            break;

        case MSG_TYPE_TURN_CHANGE:
            if (role == ROLE_HOST && len == sizeof(TurnChange)) {
                currentTurn = (currentTurn + 1) % playerCount;
                sendGameState();
            }
            break;
    }
}

// Return current role
DeviceRole NetworkArbiter::getRole() {
    return role;
}

// Return player ID
uint8_t NetworkArbiter::getPlayerID() {
    return myPlayerID;
}
