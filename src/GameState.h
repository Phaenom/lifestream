
#ifndef GAME_STATE_H
#define GAME_STATE_H

/*
  GameState Class
  Responsible for storing and managing player life totals,
  player turn status, and basic game setup/reset functionality.
*/

// Player data structure
struct Player {
    int life;      // Player's current life total
    bool isTurn;   // Whether it's this player's turn
};

// GameState Class Definition
class GameState {
public:
    static const int MAX_PLAYERS = 4;  // Maximum number of players supported
    Player players[MAX_PLAYERS];       // Array to store player data
    int playerCount;                   // Total number of players
    int myPlayerID;                    // This device's player ID

    // Reset function to initialize the game state
    void reset(int playerCount, int startingLife) {
        this->playerCount = playerCount;
        myPlayerID = 0; // Default to player 0 for single device setup

        for (int i = 0; i < playerCount; i++) {
            players[i].life = startingLife;  // Set starting life
            players[i].isTurn = false;       // Clear turn status
        }

        players[0].isTurn = true;  // Assign first turn to player 0
    }
};

#endif
