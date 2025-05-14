#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#ifdef WOKWI
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <SPI.h>
#else
#include "DEV_Config.h"
#include "EPD_2in9_V2.h"
#include "GUI_Paint.h"
#include "Fonts.h"
#endif

#ifdef WOKWI
class GxEPD2_BW_Base; // Forward declaration
#endif

// Struct representing a single player's current game state
struct PlayerState {
    int life = 20;
    int poison = 0;
    bool isTurn = false;
    bool eliminated = false;
};

class DisplayManager {
public:
    DisplayManager();

    void begin();                                      // Initialize display system
    void renderPlayerState(uint8_t playerId, const PlayerState& state); // Draw full info for one player

private:
    void drawLife(uint8_t playerId, int life);         // Render life total or "ELIMINATED"
    void drawPoison(uint8_t playerId, int poison);     // Render poison counter
    void updateTurnIndicator(uint8_t playerId, bool isTurn); // Draw blinking turn indicator
    void drawTurnMarker(int x, int y);                 // Render blinking circle

    bool toggle = false;                               // Blink state toggle
#ifndef WOKWI
    UBYTE* frameBuffer = nullptr;                      // Display buffer
#endif

#ifdef WOKWI
    GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> display;
#endif
};

#endif // DISPLAY_MANAGER_H