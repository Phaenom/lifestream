// DisplayManager.cpp
// Responsible for rendering player state to the e-paper display
// Includes life totals, poison counters, and turn indicators

#include <algorithm>
#include "DisplayManager.h"
#include "Config.h"

// Defines screen regions for each player's life, poison, and turn marker display.
struct PlayerDisplayRegion {
    int lifeX, lifeY;
    int poisonX, poisonY;
    int turnX, turnY;
};

const PlayerDisplayRegion PLAYER_LAYOUT[4] = {
    {40, 25, 40, 40, 5, 10},     // Player 1 (top left)
    {150, 25, 150, 40, 245, 10}, // Player 2 (top right, adjusted from 180 to 150)
    {40, 90, 40, 105, 5, 75},    // Player 3 (bottom left)
    {150, 90, 150, 105, 245, 75} // Player 4 (bottom right, adjusted from 180 to 150)
};

DisplayManager::DisplayManager() {}

// Initializes the e-paper display, clears the screen, and prepares a drawing buffer.
void DisplayManager::begin() {
    EPD_2IN9_V2_Init();
    EPD_2IN9_V2_Clear();

    // Allocate display buffer and initialize drawing context
    frameBuffer = (UBYTE*)malloc(EPD_2IN9_V2_WIDTH * EPD_2IN9_V2_HEIGHT / 8);
    Paint_NewImage(frameBuffer, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 0, WHITE);
    Paint_SelectImage(frameBuffer);
    Paint_Clear(WHITE);
    Serial.println("[Display] Display initialized and cleared");
}

/**
 * Draws a blinking circle at the specified (x, y) to indicate the active player's turn.
 * Alternates fill color each call to create a blinking effect.
 */
void DisplayManager::drawTurnMarker(int x, int y) {
    static int lastX = -1, lastY = -1;
    static bool lastState = false;
    if (x == lastX && y == lastY && toggle == lastState) return;
    lastX = x;
    lastY = y;
    lastState = toggle;

    x = std::max(5, std::min(x, EPD_2IN9_V2_WIDTH - 5));
    y = std::max(5, std::min(y, EPD_2IN9_V2_HEIGHT - 5));
    if (toggle) {
        Paint_DrawCircle(x, y, 5, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    } else {
        Paint_DrawCircle(x, y, 5, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    }
    Serial.printf("[Display] Drawing turn marker at (%d, %d), state: %s\n", x, y, toggle ? "BLACK" : "WHITE");
    toggle = !toggle;
    EPD_2IN9_V2_Display(frameBuffer);
}

// Updates the turn indicator for the specified player.
// If isTurn is true, activates the blinking turn marker.
void DisplayManager::updateTurnIndicator(uint8_t playerId, bool isTurn) {
    if (playerId >= 4) return;
    const auto& region = PLAYER_LAYOUT[playerId];
    if (isTurn) toggle = true;
    drawTurnMarker(region.turnX, region.turnY);
}

// Renders the life total for the specified player in their assigned screen region.
// Displays "ELIMINATED" if life is zero or below.
void DisplayManager::drawLife(uint8_t playerId, int life) {
    static int lastLife[4] = {-1, -1, -1, -1};
    if (lastLife[playerId] == life) return;
    lastLife[playerId] = life;

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
    int x = region.lifeX - textWidth / 2;
    x = std::max(0, std::min(x, EPD_2IN9_V2_WIDTH - textWidth));
    int y = std::max(0, std::min(region.lifeY - 12, 128 - 16));
    int x2 = std::min(x + 100, EPD_2IN9_V2_WIDTH);
    int y2 = std::min(y + 20, EPD_2IN9_V2_HEIGHT);
    Paint_ClearWindows(x, y, x2, y2, WHITE);
    Paint_DrawString_EN(x, y, str, &Font12, BLACK, WHITE);
    Serial.printf("[Display] Drawing life for P%d: %s\n", playerId + 1, str);
    EPD_2IN9_V2_Display(frameBuffer);
}

// Renders the poison counter for the specified player in their assigned screen region.
void DisplayManager::drawPoison(uint8_t playerId, int poison) {
    static int lastPoison[4] = {-1, -1, -1, -1};
    if (lastPoison[playerId] == poison) return;
    lastPoison[playerId] = poison;

    if (playerId >= 4) return;
    const auto& region = PLAYER_LAYOUT[playerId];
    char buf[10];
    snprintf(buf, sizeof(buf), "%d", poison);
    int textWidth = strlen(buf) * Font12.Width;
    int x = region.poisonX - textWidth / 2;
    x = std::max(0, std::min(x, EPD_2IN9_V2_WIDTH - textWidth));
    int y = std::max(0, std::min(region.poisonY - 12, 128 - 16));
    int x2 = std::min(x + 30, EPD_2IN9_V2_WIDTH);
    int y2 = std::min(y + 20, EPD_2IN9_V2_HEIGHT);
    Paint_ClearWindows(x, y, x2, y2, WHITE);
    Paint_DrawString_EN(x, y, buf, &Font12, BLACK, WHITE);
    Serial.printf("[Display] Drawing poison for P%d: %s\n", playerId + 1, buf);
    EPD_2IN9_V2_Display(frameBuffer);
}

/**
 * Renders the full state (life, poison, turn marker) for the given player ID.
 * Combines calls to drawLife, drawPoison, and updateTurnIndicator.
 */
void DisplayManager::renderPlayerState(uint8_t playerId, const PlayerState& state) {
    if (playerId >= 4) return;

    static unsigned long lastUpdateTime[4] = {0, 0, 0, 0};

    unsigned long now = millis();
    if (now - lastUpdateTime[playerId] < 1000) return;  // Skip update if less than 1000ms since last for this player
    lastUpdateTime[playerId] = now;

    Serial.printf("[Display] Rendering state for P%d — Life: %d, Poison: %d, Turn: %d\n",
                  playerId + 1,
                  state.eliminated ? 0 : state.life,
                  state.poison,
                  state.isTurn);

    static int cachedLife[4] = {-1, -1, -1, -1};
    static int cachedPoison[4] = {-1, -1, -1, -1};
    static bool cachedTurn[4] = {false, false, false, false};

    int currentLife = state.eliminated ? 0 : state.life;

    if (cachedLife[playerId] != currentLife) {
        cachedLife[playerId] = currentLife;
        drawLife(playerId, currentLife);
    }

    if (cachedPoison[playerId] != state.poison) {
        cachedPoison[playerId] = state.poison;
        drawPoison(playerId, state.poison);
    }

    if (cachedTurn[playerId] != state.isTurn) {
        cachedTurn[playerId] = state.isTurn;
        updateTurnIndicator(playerId, state.isTurn);
    }
}