#include <Arduino.h>
#include "GameSetup.h"
#include "Config.h"

// Global GameSetup instance
GameSetup gameSetup;

void GameSetup::begin() {
    LOGLN("\n[GameSetup] initialized.");

    // TODO: Replace with interactive setup using Display + Input
    startingLife = 20;
    playerCount = 4;

    LOGF("[GameSetup] Default setup — Players: %d, Starting Life: %d\n", playerCount, startingLife);
    configured = true;  // Mark as configured for testing
}

int GameSetup::getStartingLife() const {
    return startingLife;
}

int GameSetup::getPlayerCount() const {
    return playerCount;
}

void GameSetup::setFromNetwork(int players, int life) {
    playerCount = players;
    startingLife = life;
    configured = true;

    LOGF("[GameSetup] Received config: %d players, %d starting life\n", playerCount, startingLife);
}

bool GameSetup::isConfigured() const {
    LOGF("[GameSetup] isConfigured() called — %s\n", configured ? "true" : "false");
    return configured;
}