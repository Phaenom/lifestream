#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <Arduino.h>
#include "DisplayManager.h"

class GameState {
public:
    void begin(int localPlayerId, int startingLife);     // Init with player ID and life
    void reset();                                        // Reset local player only
    void adjustLife(int delta);                          // Local life
    void adjustPoison(int delta);                        // Local poison
    void setTurn(bool active);                           // Local turn state
    const PlayerState& getPlayerState(uint8_t id) const; // Get state of any player
    void nextTurn();

    int getCurrentTurnPlayer() const;
    int getPlayerCount() const;

private:
    PlayerState players[4];      // All players' states
    int localPlayerId = 0;
    int startingLife = 20;
    int currentTurnPlayer = 0;
    int playerCount = 4;

    void checkElimination();     // Local only
};

#endif