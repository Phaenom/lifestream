#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include "Config/DEV_Config.h"
#include "e-Paper/EPD_2in9_V2.h"
#include "GUI/GUI_Paint.h"
#include "Fonts/Fonts.h"
#include "assets/mtg_logo_symbol.h"

// Represents a single player's current game state
struct PlayerState {
    int life = 20;
    int poison = 0;
    bool isTurn = false;
    bool eliminated = false;
};

class DisplayManager {
public:
    DisplayManager();

    void begin();  // Initialize display system

    // Full player info render with mode and local-player context
    void renderPlayerState(uint8_t playerId, const PlayerState& state, bool isPoisonMode, bool isLocalPlayer);

    void renderAllPlayerStates(const class GameState& state);   // Redraw all players
    void drawDeviceRole();                                     // Show HOST/CLIENT role text
    void flush();                                              // Force full screen redraw
    const UBYTE* getDisplayBuffer() const;                     // Get buffer pointer
    void refreshAll();  // Full screen redraw

private:
    void drawLife(uint8_t playerId, int life, bool isActive);   // Draw life value with optional indicator
    void drawPoison(uint8_t playerId, int poison, bool isActive); // Draw poison value with optional indicator
    void drawLogo(int x, int y);                                // Draw MTG symbol
    UBYTE* displayBuffer = nullptr;                             // Pixel buffer
};

extern DisplayManager display;

#endif // DISPLAY_MANAGER_H
