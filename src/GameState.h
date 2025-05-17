#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <Arduino.h>
#include "DisplayManager.h"

class GameState {
public:
    void begin(int localPlayerId, int startingLife);        // Init with player ID and life
    void reset();                                           // Reset local player only
    void resetAll();                                        // Resets all player states (for simulation mode)
    void adjustLife(uint8_t playerId, int delta);           // Adjust life for any player
    void adjustPoison(uint8_t playerId, int delta);         // Adjust poison for any player
    void setTurn(bool active);                              // Local turn state
    const PlayerState& getPlayerState(uint8_t id) const;    // Get state of any player
    void nextTurn();

    int getCurrentTurnPlayer() const;
    int getPlayerCount() const;
    void updateRemotePlayer(uint8_t id, const PlayerState& state);

private:
    PlayerState players[4];      // All players' states
    int localPlayerId = 0;
    int startingLife = 20;
    int currentTurnPlayer = 0;
    int playerCount = 4;

    void checkElimination();     // Local only
};

extern GameState gameState; // External declaration of GameState instance

#endif