#include "GameState.h"
#include "GameSetup.h"
#include "NetworkManager.h"

extern DisplayManager display;
extern NetworkManager network;

void GameState::begin(int id, int life) {
    Serial.println("[GameSetup] Host beginning game setup...");
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

void GameState::resetAll() {
    for (int i = 0; i < 4; ++i) {
        players[i].life = startingLife;
        players[i].poison = 0;
        players[i].eliminated = false;
        players[i].isTurn = false;
        display.renderPlayerState(i, players[i]);
    }
    currentTurnPlayer = 0;
    players[currentTurnPlayer].isTurn = true;
    display.renderPlayerState(currentTurnPlayer, players[currentTurnPlayer]);

    Serial.println("[GameState] All players reset to starting state.");
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
    //network.sendGameState(currentTurnPlayer, players[currentTurnPlayer]);
    Serial.printf("[GameState] Turn passed from Player %d to Player %d\n", 
                  (currentTurnPlayer + playerCount - 1) % playerCount, currentTurnPlayer);
}

int GameState::getCurrentTurnPlayer() const {
    return currentTurnPlayer;
}

int GameState::getPlayerCount() const {
    return playerCount;
}

void GameState::adjustPoison(uint8_t playerId, int delta) {
    if (playerId >= 4) return;

    PlayerState& p = players[playerId];
    if (p.eliminated) return;

    p.poison += delta;
    if (p.poison < 0) p.poison = 0;
    if (p.poison >= 10) p.eliminated = true;

    display.renderPlayerState(playerId, p);
    Serial.printf("[GameState] Adjusting poison for player %d by %+d\n", playerId, delta);
}

void GameState::adjustLife(uint8_t playerId, int delta) {
    if (playerId >= 4) return;

    PlayerState& p = players[playerId];
    if (p.eliminated) return;

    p.life += delta;
    if (p.life < 0) p.life = 0;

    if (p.life == 0 || p.poison >= 10) p.eliminated = true;

    display.renderPlayerState(playerId, p);
    Serial.printf("[GameState] Adjusting life for player %d by %+d. New life: %d\n", playerId, delta, p.life);
}