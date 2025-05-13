#ifndef GAME_SETUP_H
#define GAME_SETUP_H
class GameSetup {
public:
    void begin();                   // Launches game setup (e.g., via input)
    int getStartingLife() const;   // Returns configured starting life
    int getPlayerCount() const;    // Returns configured number of players
    void setFromNetwork(int playerCount, int startingLife);

private:
    int startingLife = 20;         // Default starting life
    int playerCount = 4;           // Default number of players
};

#endif