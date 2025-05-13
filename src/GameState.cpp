#include "GameState.h"

extern DisplayManager display;
extern NetworkManager network;

void GameState::begin(int id, int life) {
    localPlayerId = id;
    startingLife = life;
    currentTurnPlayer = 0;
    playerCount = 4;

    for (int i = 0; i < 4; ++i) {
        players[i] = { .life = life, .poison = 0, .isTurn = false, .eliminated = false };
    }

    display.renderPlayerState(localPlayerId, players[localPlayerId]);
}

void GameState::reset() {
    players[localPlayerId].life = startingLife;
    players[localPlayerId].poison = 0;
    players[localPlayerId].eliminated = false;
    players[localPlayerId].isTurn = false;

    display.renderPlayerState(localPlayerId, players[localPlayerId]);
}

void GameState::adjustLife(int delta) {
    PlayerState& self = players[localPlayerId];
    if (self.eliminated) return;
    self.life += delta;
    if (self.life < 0) self.life = 0;
    checkElimination();
    display.renderPlayerState(localPlayerId, self);
}

void GameState::adjustPoison(int delta) {
    PlayerState& self = players[localPlayerId];
    if (self.eliminated) return;
    self.poison += delta;
    if (self.poison < 0) self.poison = 0;
    checkElimination();
    display.renderPlayerState(localPlayerId, self);
}

void GameState::setTurn(bool active) {
    players[localPlayerId].isTurn = active;
    display.renderPlayerState(localPlayerId, players[localPlayerId]);
}

void GameState::checkElimination() {
    PlayerState& self = players[localPlayerId];
    if (self.life <= 0 || self.poison >= 10) {
        self.eliminated = true;
    }
}

const PlayerState& GameState::getPlayerState(uint8_t id) const {
    return players[id];
}
void GameState::updateRemotePlayer(uint8_t id, const PlayerState& state) {
    if (id == localPlayerId || id >= 4) return;
    players[id] = state;
    display.renderPlayerState(id, players[id]);
}

void GameState::nextTurn() {
    players[currentTurnPlayer].isTurn = false;
    display.renderPlayerState(currentTurnPlayer, players[currentTurnPlayer]);

    currentTurnPlayer = (currentTurnPlayer + 1) % playerCount;
    players[currentTurnPlayer].isTurn = true;
    display.renderPlayerState(currentTurnPlayer, players[currentTurnPlayer]);
    network.sendGameState(currentTurnPlayer, players[currentTurnPlayer]);
}

int GameState::getCurrentTurnPlayer() const {
    return currentTurnPlayer;
}

int GameState::getPlayerCount() const {
    return playerCount;
}