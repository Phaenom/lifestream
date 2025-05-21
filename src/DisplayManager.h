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

    void begin();                                           // Initialize display system
    void renderPlayerState(uint8_t playerId, const PlayerState& state);  // Draw full info for one player
    void renderAllPlayerStates(const class GameState& state);            // Draw all players
    void drawDeviceRole();                                  // Draw host/client role indicator
    void flush();                                           // Push any pending updates to the screen
    const UBYTE* getDisplayBuffer() const;                  // Return pointer to display buffer

private:
    void drawLife(uint8_t playerId, int life);              // Draw life total or "ELIMINATED"
    void drawPoison(uint8_t playerId, int poison);          // Draw poison counter
    void updateTurnIndicator(uint8_t playerId, bool isTurn); // Draw blinking indicator if player's turn
    void drawTurnMarker(int x, int y);                      // Draw the circular turn marker
    void clearTurnMarker(int x, int y);                     // Erase turn marker
    void drawLogo(int x, int y);                            // Draw MTG logo at given coordinates
    UBYTE* displayBuffer = nullptr;                         // Display memory buffer
    bool toggle = false;                                    // Blink state toggle
};

// Global DisplayManager instance
extern DisplayManager display;

#endif // DISPLAY_MANAGER_H