#include "DisplayManager.h"

// Defines screen coordinates for life, poison, and turn marker per player
struct PlayerDisplayRegion {
    int lifeX, lifeY;
    int poisonX, poisonY;
    int turnX, turnY;
};

const PlayerDisplayRegion PLAYER_LAYOUT[4] = {
    {10, 40, 10, 80, 100, 10},     // Player 1
    {130, 40, 130, 80, 220, 10},   // Player 2
    {10, 120, 10, 160, 100, 100},  // Player 3
    {130, 120, 130, 160, 220, 100} // Player 4
};

DisplayManager::DisplayManager() {}

void DisplayManager::begin() {
    EPD_2IN9_V2_Init();
    EPD_2IN9_V2_Clear();

    frameBuffer = (UBYTE*)malloc(EPD_2IN9_V2_WIDTH * EPD_2IN9_V2_HEIGHT / 8);
    Paint_NewImage(frameBuffer, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 0, WHITE);
    Paint_SelectImage(frameBuffer);
    Paint_Clear(WHITE);
}

/**
 * Draws a blinking circle to indicate current player's turn.
 */
void DisplayManager::drawTurnMarker(int x, int y) {
    if (toggle) {
        Paint_DrawCircle(x, y, 5, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    } else {
        Paint_DrawCircle(x, y, 5, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    }
    toggle = !toggle;
    EPD_2IN9_V2_Display(frameBuffer);
}

void DisplayManager::updateTurnIndicator(uint8_t playerId, bool isTurn) {
    if (playerId >= 4) return;
    const auto& region = PLAYER_LAYOUT[playerId];
    if (isTurn) toggle = true;
    drawTurnMarker(region.turnX, region.turnY);
}

void DisplayManager::drawLife(uint8_t playerId, int life) {
    if (playerId >= 4) return;
    const auto& region = PLAYER_LAYOUT[playerId];

    Paint_ClearWindows(region.lifeX, region.lifeY - 24, region.lifeX + 100, region.lifeY + 6, WHITE);
    if (life <= 0) {
        Paint_DrawString_EN(region.lifeX, region.lifeY - 24, "ELIMINATED", &Font24, BLACK, WHITE);
    } else {
        char buf[10];
        snprintf(buf, sizeof(buf), "%d", life);
        Paint_DrawString_EN(region.lifeX, region.lifeY - 24, buf, &Font24, BLACK, WHITE);
    }
    EPD_2IN9_V2_Display(frameBuffer);
}

void DisplayManager::drawPoison(uint8_t playerId, int poison) {
    if (playerId >= 4) return;
    const auto& region = PLAYER_LAYOUT[playerId];

    Paint_ClearWindows(region.poisonX, region.poisonY - 12, region.poisonX + 30, region.poisonY + 8, WHITE);
    char buf[10];
    snprintf(buf, sizeof(buf), "%d", poison);
    Paint_DrawString_EN(region.poisonX, region.poisonY - 12, buf, &Font12, BLACK, WHITE);
    EPD_2IN9_V2_Display(frameBuffer);
}

/**
 * Renders the full state for a single player (life, poison, turn).
 */
void DisplayManager::renderPlayerState(uint8_t playerId, const PlayerState& state) {
    if (playerId >= 4) return;

    drawLife(playerId, state.eliminated ? 0 : state.life);
    drawPoison(playerId, state.poison);
    updateTurnIndicator(playerId, state.isTurn);
}