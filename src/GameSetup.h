#ifndef GAME_SETUP_H
#define GAME_SETUP_H

class GameSetup {
public:
    void begin();                   // Initialize game setup (default or interactive)
    int getStartingLife() const;   // Get configured starting life total
    int getPlayerCount() const;    // Get configured number of players
    void setFromNetwork(int playerCount, int startingLife); // Apply setup from network data
    bool isConfigured() const;     // Check if setup is completed

private:
    int startingLife = 20;         // Starting life total (default: 20)
    int playerCount = 4;           // Number of players (default: 4)
    bool configured = false;       // Whether setup has been completed
};

// Global GameSetup instance
extern GameSetup gameSetup;

#endif