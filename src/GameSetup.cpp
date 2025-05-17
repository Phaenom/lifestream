#include "GameSetup.h"
#include <Arduino.h>

GameSetup gameSetup;

void GameSetup::begin() {
    Serial.println("\n[GameSetup] initialized.");

    // TODO: Replace with interactive setup using Display + Input
    startingLife = 20;
    playerCount = 4;
    Serial.println("");
    Serial.printf("[GameSetup] Default setup — Players: %d, Starting Life: %d\n", playerCount, startingLife);
    configured = true;  // Simulate configuration for testing
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
    Serial.printf("[GameSetup] Received config: %d players, %d starting life\n", playerCount, startingLife);
}

bool GameSetup::isConfigured() const {
    Serial.printf("[GameSetup] isConfigured() called — %s\n", configured ? "true" : "false");
    return configured;
}