// DisplayManager.cpp
// This source file implements the DisplayManager class responsible for controlling the ePaper display output.
// It manages drawing life totals, poison counters, and turn indicators on the ePaper display.

#include "DisplayManager.h"

/**
 * Constructor initializes toggle flag and frameBuffer pointer.
 */
DisplayManager::DisplayManager() : toggle(false), frameBuffer(nullptr) {}

/**
 * Initializes the ePaper display hardware and prepares the frame buffer.
 * Allocates memory for the frame buffer and clears the display.
 */
void DisplayManager::begin() {
    Serial.println("\n[Init] DisplayManager initialized.");

    // Initialize the ePaper display hardware
    EPD_2IN9_V2_Init();
    EPD_2IN9_V2_Clear();

    // Allocate and initialize frame buffer for drawing
    frameBuffer = (UBYTE *)malloc(EPD_2IN9_V2_WIDTH * EPD_2IN9_V2_HEIGHT / 8);
    Paint_NewImage(frameBuffer, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 0, WHITE);
    Paint_SelectImage(frameBuffer);
    Paint_Clear(WHITE);
}

/**
 * Draws a blinking turn indicator circle at the specified (x, y) coordinates.
 * Uses the toggle flag to alternate between drawing a black filled circle and erasing it.
 * 
 * @param x The x-coordinate of the turn marker center.
 * @param y The y-coordinate of the turn marker center.
 */
void DisplayManager::drawTurnMarker(int x, int y) {
    // Draw filled circle in black or white depending on toggle state
    if (toggle) {
        Paint_DrawCircle(x, y, 5, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    } else {
        Paint_DrawCircle(x, y, 5, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    }
    // Flip toggle to create blinking effect on subsequent calls
    toggle = !toggle;

    // Update the ePaper display with the current frame buffer contents
    EPD_2IN9_V2_Display(frameBuffer);
}

/**
 * Updates the turn indicator by redrawing the turn marker at the specified location.
 * 
 * @param x The x-coordinate of the turn marker center.
 * @param y The y-coordinate of the turn marker center.
 */
void DisplayManager::updateTurnIndicator(int x, int y) {
    drawTurnMarker(x, y);
}

/**
 * Draws the life total or "ELIMINATED" text at the specified (x, y) coordinates.
 * Clears the area before drawing to avoid overlapping previous text.
 * 
 * @param x The x-coordinate where the life total or text should be drawn.
 * @param y The y-coordinate baseline for the text.
 * @param life The current life total; if zero or below, "ELIMINATED" is shown instead.
 */
void DisplayManager::drawLife(int x, int y, int life) {
    // Clear the area where life total or eliminated text will be drawn
    Paint_ClearWindows(x, y - 24, x + 100, y + 6, WHITE);

    if (life <= 0) {
        // Draw "ELIMINATED" text if life is zero or below
        Paint_DrawString_EN(x, y - 24, "ELIMINATED", &Font24, BLACK, WHITE);
    } else {
        // Draw the numeric life total
        char buf[10];
        sprintf(buf, "%d", life);
        Paint_DrawString_EN(x, y - 24, buf, &Font24, BLACK, WHITE);
    }

    // Refresh the display to show updated life total or eliminated text
    EPD_2IN9_V2_Display(frameBuffer);
}

/**
 * Draws the poison counter at the specified (x, y) coordinates using Font12.
 * Clears the area before drawing to prevent overlap.
 * 
 * @param x The x-coordinate where the poison count should be drawn.
 * @param y The y-coordinate baseline for the poison count text.
 * @param poison The current poison counter value.
 */
void DisplayManager::drawPoison(int x, int y, int poison) {
    // Clear the area where poison counter will be drawn
    Paint_ClearWindows(x, y - 12, x + 30, y + 8, WHITE);

    // Draw the numeric poison counter
    char buf[10];
    sprintf(buf, "%d", poison);
    Paint_DrawString_EN(x, y - 12, buf, &Font12, BLACK, WHITE);

    // Refresh the display to show updated poison counter
    EPD_2IN9_V2_Display(frameBuffer);
}