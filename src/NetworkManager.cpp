#include "Config.h"
// #include "NetworkManager.h"
// #include "DisplayManager.h"
extern DisplayManager display;

NetworkManager network;

// Global broadcast address used to send ESP-NOW packets to all nearby devices.
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Broadcast address for ESP-NOW

// Low-level ESP-NOW initialization
void NetworkManager::setupESPNow() {
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Initialization Failed");
        ESP.restart();  // Reset device on failure to recover cleanly
    }
}

// ESP-NOW receive callback using the modern ESP-IDF-style API.
// When data is received, it forwards it to the network's onReceive handler.
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    extern NetworkManager network;
    network.onDataReceive(mac, incomingData, len);
}

// Initializes WiFi and ESP-NOW, and begins host discovery window.
void NetworkManager::begin() {
    WiFi.mode(WIFI_STA);  // ESP-NOW only works in station (STA) mode
    setupESPNow();        // Initialize ESP-NOW layer
    esp_now_register_recv_cb(OnDataRecv); // Set up callback to receive packets
    discoveryStartTime = millis();        // Start a timer for host discovery timeout
}

// Main loop to manage network state.
// If role is not yet determined, check for host presence within a timeout.
// If host, send game state and announcements at regular intervals.
void NetworkManager::updateRole() {
    if (role != ROLE_UNDEFINED) return;  // Role already assigned

    // Wait a few seconds for host announcements
    unsigned long now = millis();
    if (!hostDetected && now - discoveryStartTime > 5000) {
        becomeHost();
    } else if (hostDetected && now - discoveryStartTime > 1000) {
        becomeClient();
    }
}

// Promote device to Host role.
// Assign self Player ID 0 and add a broadcast peer for ESP-NOW sends.
void NetworkManager::becomeHost() {
    Serial.println("\nBecoming HOST");
    role = ROLE_HOST;
    myPlayerID = 0; // Convention: host is always Player 0

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add broadcast peer");
    }
}

// Promote device to Client role.
// For now, assigns a static Player ID of 1 (should eventually be dynamic).
void NetworkManager::becomeClient() {
    Serial.println("Becoming CLIENT");
    role = ROLE_CLIENT;
    myPlayerID = 1;
}

// Host-only: Broadcast full game state to all clients.
void NetworkManager::sendGameState() {
    GameData data = {};
    data.messageType = MSG_TYPE_GAME_STATE;
    data.playerCount = playerCount;
    data.currentTurn = currentTurn;
    data.senderID = myPlayerID;

    for (int i = 0; i < 4; ++i) {
        data.lifeTotal[i] = gameState.getPlayerState(i).life;
    }

    esp_now_send(broadcastAddress, (uint8_t*)&data, sizeof(data));
}


// Client-only: Send new life total to host.
void NetworkManager::sendLifeUpdate(uint8_t life) {
    if (role == ROLE_CLIENT) {
        LifeChange change = {MSG_TYPE_LIFE_CHANGE, myPlayerID, life};
        esp_now_send(broadcastAddress, (uint8_t *)&change, sizeof(change));
    }
}

// Client-only: Send a request to pass the turn to the next player.
void NetworkManager::sendTurnChange() {
    if (role == ROLE_CLIENT) {
        TurnChange change = {MSG_TYPE_TURN_CHANGE, myPlayerID};
        esp_now_send(broadcastAddress, (uint8_t *)&change, sizeof(change));
    }
}

// Called whenever a message is received over ESP-NOW.
// Determines message type and takes appropriate action based on current role.
void NetworkManager::onDataReceive(const uint8_t *mac, const uint8_t *incomingData, int len) {
    if (len < 1) return;
    uint8_t messageType = incomingData[0];

    switch (messageType) {
        case MSG_TYPE_HOST_ANNOUNCE:
            if (role == ROLE_UNDEFINED) {
                Serial.println("[Network] Host Announce Received!");
            }
            hostDetected = true;

            memcpy(hostMac, mac, 6);    // Store the host MAC

            break;

        case MSG_TYPE_GAME_STATE:
            if (role == ROLE_CLIENT && len >= sizeof(GameData)) {
                memcpy(&pendingGameState, incomingData, sizeof(GameData));
                hasPendingGameState = true;
                Serial.printf("[Network] Game state buffered: Turn=%d, Life=%d/%d/%d/%d\n",
                            pendingGameState.currentTurn,
                            pendingGameState.lifeTotal[0], pendingGameState.lifeTotal[1],
                            pendingGameState.lifeTotal[2], pendingGameState.lifeTotal[3]);
            }
            break;

        case MSG_TYPE_LIFE_CHANGE:
            // Host receives a life total update from a client
            if (role == ROLE_HOST && len == sizeof(LifeChange)) {
                LifeChange *change = (LifeChange *)incomingData;
                lifeTotals[change->senderID] = change->newLifeTotal;
                sendGameState(); // Sync updated state with all clients
            }
            break;

        case MSG_TYPE_TURN_CHANGE:
            // Host receives a turn-change request and advances the turn
            if (role == ROLE_HOST && len == sizeof(TurnChange)) {
                currentTurn = (currentTurn + 1) % playerCount;
                sendGameState();
            }
            break;
    }
}

// Return the current device role (host, client, or undefined)
DeviceRole NetworkManager::getRole() {
    return role;
}

// Return the current player ID assigned to this device
uint8_t NetworkManager::getPlayerID() {
    return myPlayerID;
}

const char* NetworkManager::roleToString(DeviceRole role) {
    switch (role) {
        case ROLE_HOST: return "🧠 Host";
        case ROLE_CLIENT: return "🎮 Client";
        case ROLE_UNDEFINED: return "❓ Undefined";
        default: return "🚫 Unknown";
    }
}

void NetworkManager::heartbeat() {
    static unsigned long lastAnnounce = 0;
    static unsigned long lastHeartbeat = 0;
    unsigned long now = millis();

    // Host-only: broadcast presence every second
    if (role == ROLE_HOST && now - lastAnnounce > 1000) {
        HostAnnounce announce = { MSG_TYPE_HOST_ANNOUNCE };
        esp_now_send(broadcastAddress, (uint8_t*)&announce, sizeof(announce));
        lastAnnounce = now;
    }

    // All devices: print heartbeat info every 5 seconds
    if (now - lastHeartbeat > 5000) {
        uint8_t localMac[6];
        esp_read_mac(localMac, ESP_MAC_WIFI_STA);

        Serial.println("\n[Loop] Network Heartbeat");
        Serial.printf(" - Player ID : %d\n", getPlayerID());
        Serial.printf(" - Role      : %s\n", roleToString(getRole()));
        Serial.printf(" - MAC Addr  : %02X:%02X:%02X:%02X:%02X:%02X\n",
                      localMac[0], localMac[1], localMac[2],
                      localMac[3], localMac[4], localMac[5]);

        esp_now_peer_num_t peerCount;
        esp_now_get_peer_num(&peerCount);
        Serial.printf(" - ESP-NOW Peers: %d\n", peerCount.total_num);

        if (role == ROLE_CLIENT && hostMac[0] != 0) {
            Serial.printf(" - Host MAC  : %02X:%02X:%02X:%02X:%02X:%02X\n",
                          hostMac[0], hostMac[1], hostMac[2],
                          hostMac[3], hostMac[4], hostMac[5]);
        }

        lastHeartbeat = now;
    }
}

const uint8_t* NetworkManager::getHostMAC() const {
    return (role == ROLE_CLIENT) ? hostMac : nullptr;
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

    // ✅ Debug pointer check before using display
    Serial.printf("[Debug] &display: %p\n", (void*)&display);

    // ✅ Optional: log values being rendered
    Serial.printf("[Debug] Drawing state: Turn=%d, Life=%d/%d/%d/%d\n",
                  currentTurn, lifeTotals[0], lifeTotals[1], lifeTotals[2], lifeTotals[3]);

    for (int i = 0; i < 4; ++i) {
        PlayerState state;
        state.life = lifeTotals[i];
        state.poison = 0;
        state.eliminated = (lifeTotals[i] <= 0);
        state.isTurn = (i == currentTurn);
        display.renderPlayerState(i, state);
    }

    display.flush();  // or display.display() depending on driver

    hasPendingGameState = false;
}

// =================================================================================
//
// NETWORK MANAGER IMPLEMENTATION
//      SECONDARY VERSION (NOT WORKING) LEFT FOR REFERENCE
//
// =================================================================================

/* #include "NetworkManager.h"
#include "GameSetup.h"
#include "GameState.h"
#include "DeviceManager.h"

extern GameSetup gameSetup;
extern GameState gameState;
extern DeviceManager device;

void NetworkManager::begin() {
    WiFi.mode(WIFI_STA); // REQUIRED for ESP-NOW to work correctly
    
    esp_wifi_set_promiscuous(true);
    wifi_second_chan_t second;
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE); // force channel 1
    esp_wifi_set_promiscuous(false);
    
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed");
        return;
    }

    esp_now_register_recv_cb(onDataReceived);
    Serial.println("[NetworkManager] ESP-NOW initialized and callback registered.");

    // Host discovery window
    Serial.println("[NetworkManager] Waiting for host broadcast...");
    unsigned long start = millis();
    while (millis() - start < 3000) {
        if (gameSetup.isConfigured()) {
            Serial.println("[NetworkManager] Host detected. Acting as client.");
            return;
        }
        delay(50);
}

    Serial.println("[NetworkManager] No host found. Assuming host role.");
    device.assumeHostRole();
    gameSetup.begin();
    sendGameSetup(gameSetup.getPlayerCount(), gameSetup.getStartingLife());
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
        Serial.printf("[NetworkManager] Game state initialized for player %d with life %d\n", device.getPlayerId(), packet.life);
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
                sendGameSetupTo(mac, gameSetup.getPlayerCount(), gameSetup.getStartingLife());
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
    if (!device.isHost()) {
        Serial.println("[NetworkManager] Sending join request");
        esp_now_send(nullptr, (uint8_t*)&packet, sizeof(packet));
    } else {
        Serial.println("[NetworkManager] Not sending join request (this device is host)");
    }
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

void NetworkManager::sendGameSetupTo(const uint8_t* dest, uint8_t playerCount, uint8_t startingLife) {
    GameSyncPacket packet = {};
    packet.type = PACKET_TYPE_SETUP;
    packet.playerCount = playerCount;
    packet.life = startingLife;

    Serial.printf("[NetworkManager] Sending setup to peer: playerCount=%d, life=%d\n", playerCount, startingLife);
    esp_err_t result = esp_now_send(dest, (uint8_t*)&packet, sizeof(packet));
    Serial.printf("[NetworkManager] sendGameSetupTo result: %d\n", result);
}
 */