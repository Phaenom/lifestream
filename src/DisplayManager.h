#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include "DEV_Config.h"
#include "utility/EPD_2in9_V2.h"
#include "GUI_Paint.h"
#include "Fonts.h"

/**
 * DisplayManager class handles all display-related operations for the ePaper screen.
 * It manages drawing life and poison counters, as well as a blinking turn indicator.
 */
class DisplayManager {
public:
    DisplayManager();

    /**
     * Initialize the display hardware and prepare it for drawing.
     */
    void begin();                                 // Initialize the display

    /**
     * Draw the life counter at the specified (x, y) coordinates.
     * Displays the life value or "ELIMINATED" if life is zero or less.
     * @param x X coordinate on the display
     * @param y Y coordinate on the display
     * @param life The current life value to display
     */
    void drawLife(int x, int y, int life);        // Draw life or "ELIMINATED"

    /**
     * Draw the poison counter at the specified (x, y) coordinates.
     * @param x X coordinate on the display
     * @param y Y coordinate on the display
     * @param poison The current poison count to display
     */
    void drawPoison(int x, int y, int poison);    // Draw poison counter

    /**
     * Update the turn indicator at the specified (x, y) coordinates.
     * This indicator blinks to show the active player's turn.
     * @param x X coordinate on the display
     * @param y Y coordinate on the display
     */
    void updateTurnIndicator(int x, int y);       // Blinking turn marker

private:
    /**
     * Draw the turn marker at the given position.
     * Used internally to toggle the blinking state.
     * @param x X coordinate on the display
     * @param y Y coordinate on the display
     */
    void drawTurnMarker(int x, int y);            // Internal helper for blinking

    bool toggle;                                  // Tracks toggle state for blinking effect
    UBYTE* frameBuffer;                           // Frame buffer storing ePaper display data
};

#endif // DISPLAY_MANAGER_H