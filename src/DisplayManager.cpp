#include <algorithm>
#include "DisplayManager.h"
#include "Config.h"

// Defines screen coordinates for life, poison, and turn marker per player
struct PlayerDisplayRegion {
    int lifeX, lifeY;
    int poisonX, poisonY;
    int turnX, turnY;
};

const PlayerDisplayRegion PLAYER_LAYOUT[4] = {
    {10, 40, 10, 80, 100, 10},     // Player 1
    {130, 40, 130, 80, 220, 10},   // Player 2
    {10, 110, 10, 100, 100, 100},  // Player 3
    {130, 110, 130, 100, 220, 100} // Player 4
};

DisplayManager::DisplayManager() {}

void DisplayManager::begin() {
#ifndef SIMULATION_MODE
    EPD_2IN9_V2_Init();
    EPD_2IN9_V2_Clear();

    frameBuffer = (UBYTE*)malloc(EPD_2IN9_V2_WIDTH * EPD_2IN9_V2_HEIGHT / 8);
    Paint_NewImage(frameBuffer, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 0, WHITE);
    Paint_SelectImage(frameBuffer);
    Paint_Clear(WHITE);
#else
    Serial.println("[DisplayManager] Simulation mode active, display disabled.");
#endif
}

/**
 * Draws a blinking circle to indicate current player's turn.
 */
void DisplayManager::drawTurnMarker(int x, int y) {
#ifndef SIMULATION_MODE
    if (toggle) {
        Paint_DrawCircle(x, y, 5, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    } else {
        Paint_DrawCircle(x, y, 5, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    }
    toggle = !toggle;
    EPD_2IN9_V2_Display(frameBuffer);
#endif
}

void DisplayManager::updateTurnIndicator(uint8_t playerId, bool isTurn) {
#ifndef SIMULATION_MODE
    if (playerId >= 4) return;
    const auto& region = PLAYER_LAYOUT[playerId];
    if (isTurn) toggle = true;
    drawTurnMarker(region.turnX, region.turnY);
#endif
}

void DisplayManager::drawLife(uint8_t playerId, int life) {
#ifndef SIMULATION_MODE
    if (playerId >= 4) return;
    const auto& region = PLAYER_LAYOUT[playerId];
    const char* str;
    char buf[10];
    if (life <= 0) {
        str = "ELIMINATED";
    } else {
        snprintf(buf, sizeof(buf), "%d", life);
        str = buf;
    }
    int textWidth = strlen(str) * Font12.Width;
    int x = std::min(region.lifeX, 296 - textWidth);
    int y = std::min(region.lifeY - 12, 128 - 16);
    Paint_ClearWindows(x, y, x + 100, y + 20, WHITE);
    Paint_DrawString_EN(x, y, str, &Font12, BLACK, WHITE);
    EPD_2IN9_V2_Display(frameBuffer);
#endif
}

void DisplayManager::drawPoison(uint8_t playerId, int poison) {
#ifndef SIMULATION_MODE
    if (playerId >= 4) return;
    const auto& region = PLAYER_LAYOUT[playerId];
    char buf[10];
    snprintf(buf, sizeof(buf), "%d", poison);
    int textWidth = strlen(buf) * Font12.Width;
    int x = std::min(region.poisonX, 296 - textWidth);
    int y = std::min(region.poisonY - 12, 128 - 16);
    Paint_ClearWindows(x, y, x + 30, y + 20, WHITE);
    Paint_DrawString_EN(x, y, buf, &Font12, BLACK, WHITE);
    EPD_2IN9_V2_Display(frameBuffer);
#endif
}

/**
 * Renders the full state for a single player (life, poison, turn).
 */
void DisplayManager::renderPlayerState(uint8_t playerId, const PlayerState& state) {
#ifndef SIMULATION_MODE
    if (playerId >= 4) return;

    drawLife(playerId, state.eliminated ? 0 : state.life);
    drawPoison(playerId, state.poison);
    updateTurnIndicator(playerId, state.isTurn);
#endif
}