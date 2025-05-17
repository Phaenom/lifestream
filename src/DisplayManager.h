
#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <SPI.h>
#include "DEV_Config.h" //?
#include "EPD.h"
#include "GUI_Paint.h"
#include "assets/mtg_logo_symbol.h"
#include "assets/psn_symbol.h"
#include <Arduino.h>
#include "NetworkManager.h"

class DisplayManager {
public:
    // Initialize the display hardware
    void begin();

    // Draw life totals for all players and handle turn indicator
    void drawLifeCounter(int lifeP1, int lifeP2, int lifeP3, int lifeP4, int currentTurnPlayerID, int myPlayerID, DeviceRole role);
    void drawPoisonCounter(int psnP1, int psnP2, int psnP3, int psnP4, int currentTurnPlayerID, int myPlayerID, DeviceRole role);

    // Put display into low-power sleep mode
    void sleep();

private:
    UBYTE *BlackImage; // Buffer for display image

    // Draws the MTG logo at specified (x, y) coordinates
    void drawLogo(int x, int y);
    void drawPoison(int x, int y);

};

#endif