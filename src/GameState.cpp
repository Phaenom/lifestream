#include "GameState.h"
#include "NetworkManager.h"

extern DisplayManager display;
extern NetworkManager network;

void GameState::begin(int id, int life) {
    localPlayerId = id;
    startingLife = life;
    currentTurnPlayer = 0;
    playerCount = 4;

    for (int i = 0; i < 4; ++i) {
        players[i].life = life;
        players[i].poison = 0;
        players[i].isTurn = false;
        players[i].eliminated = false;
    }

    display.renderPlayerState(localPlayerId, players[localPlayerId]);
    Serial.printf("[GameState] Initialized as player %d with starting life %d\n", id, life);
}

void GameState::reset() {
    players[localPlayerId].life = startingLife;
    players[localPlayerId].poison = 0;
    players[localPlayerId].eliminated = false;
    players[localPlayerId].isTurn = false;

    display.renderPlayerState(localPlayerId, players[localPlayerId]);
    Serial.println("[GameState] Local player state reset to starting values.");
}

void GameState::adjustLife(int delta) {
    PlayerState& self = players[localPlayerId];
    if (self.eliminated) return;
    self.life += delta;
    if (self.life < 0) self.life = 0;
    checkElimination();
    display.renderPlayerState(localPlayerId, self);
    Serial.printf("[GameState] Life adjusted by %d. New life: %d\n", delta, self.life);
    Serial.printf("[GameState] Adjusting life for player %d by %+d\n", currentTurnPlayer, delta);
}

void GameState::adjustPoison(int delta) {
    PlayerState& self = players[localPlayerId];
    if (self.eliminated) return;
    self.poison += delta;
    if (self.poison < 0) self.poison = 0;
    checkElimination();
    display.renderPlayerState(localPlayerId, self);
    Serial.printf("[GameState] Poison adjusted by %d. New poison: %d\n", delta, self.poison);
}

void GameState::setTurn(bool active) {
    players[localPlayerId].isTurn = active;
    display.renderPlayerState(localPlayerId, players[localPlayerId]);
    Serial.printf("[GameState] Local turn state set to: %s\n", active ? "true" : "false");
}

void GameState::checkElimination() {
    PlayerState& self = players[localPlayerId];
    if (self.life <= 0 || self.poison >= 10) {
        self.eliminated = true;
        Serial.println("[GameState] Player eliminated.");
    }
}

const PlayerState& GameState::getPlayerState(uint8_t id) const {
    return players[id];
}
void GameState::updateRemotePlayer(uint8_t id, const PlayerState& state) {
    if (id == localPlayerId || id >= 4) return;
    players[id] = state;
    display.renderPlayerState(id, players[id]);
    Serial.printf("[GameState] Remote player %d state updated: life=%d, poison=%d, turn=%s, eliminated=%s\n",
                  id, state.life, state.poison,
                  state.isTurn ? "true" : "false",
                  state.eliminated ? "true" : "false");
}

void GameState::nextTurn() {
    players[currentTurnPlayer].isTurn = false;
    display.renderPlayerState(currentTurnPlayer, players[currentTurnPlayer]);

    currentTurnPlayer = (currentTurnPlayer + 1) % playerCount;
    players[currentTurnPlayer].isTurn = true;
    display.renderPlayerState(currentTurnPlayer, players[currentTurnPlayer]);
    network.sendGameState(currentTurnPlayer, players[currentTurnPlayer]);
    Serial.printf("[GameState] Turn passed from Player %d to Player %d\n", 
                  (currentTurnPlayer + playerCount - 1) % playerCount, currentTurnPlayer);
}

int GameState::getCurrentTurnPlayer() const {
    return currentTurnPlayer;
}

int GameState::getPlayerCount() const {
    return playerCount;
}