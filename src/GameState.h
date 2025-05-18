#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <Arduino.h>
#include "DisplayManager.h"

class GameState {
public:
    void begin(int localPlayerId, int startingLife);         // Initialize with player ID and starting life
    void reset();                                            // Reset local player's state
    void resetAll();                                         // Reset all players (used in simulation mode)
    void adjustLife(uint8_t playerId, int delta);            // Change life total for a player
    void adjustPoison(uint8_t playerId, int delta);          // Change poison counter for a player
    void setTurn(bool active);                               // Set local player's turn state
    const PlayerState& getPlayerState(uint8_t id) const;     // Access a specific player's state
    void nextTurn();                                         // Advance to the next player's turn

    int getCurrentTurnPlayer() const;                        // Get the ID of the current turn player
    int getPlayerCount() const;                              // Return the number of players
    void updateRemotePlayer(uint8_t id, const PlayerState& state); // Sync state from network
    bool lifeChanged(uint8_t playerId, int newValue);        // Check if life has changed
    int getLife(uint8_t playerId) const;                     // Get a player's life total

private:
    PlayerState players[4];      // All player states
    int localPlayerId = 0;       // This device's assigned player ID
    int startingLife = 20;       // Configured starting life
    int currentTurnPlayer = 0;   // Player whose turn it is
    int playerCount = 4;         // Total number of players

    void checkElimination();     // Check for any player at 0 or less life
};

extern GameState gameState;      // Global GameState instance

#endif