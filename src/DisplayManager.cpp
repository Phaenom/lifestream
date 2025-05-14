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
    {40, 25, 40, 40, 5, 10},     // Player 1 (top left)
    {180, 25, 180, 40, 245, 10}, // Player 2 (top right)
    {40, 90, 40, 105, 5, 75},    // Player 3 (bottom left)
    {180, 90, 180, 105, 245, 75} // Player 4 (bottom right)
};

#ifdef WOKWI
DisplayManager::DisplayManager()
    : display(GxEPD2_290(5, 17, 16, 4)) {
}
#else
DisplayManager::DisplayManager() {}
#endif

void DisplayManager::begin() {
//     #ifdef WOKWI
//     display.init(115200);
//     display.setRotation(1);
//     display.setFont(&FreeMonoBold9pt7b);
//     display.setTextColor(GxEPD_BLACK);
//     display.setFullWindow();

//     display.firstPage();
//     do {
//         for (int id = 0; id < 4; ++id) {
//             const auto& region = PLAYER_LAYOUT[id];

//             // Full life string
//             String lifeVal = (20 - id * 5 <= 0 ? "ELIM" : String(20 - id * 5));
//             String lifeLine = "P" + String(id + 1) + ": " + lifeVal;
//             int lifeTextWidth = lifeLine.length() * 6;
//             int lifeX = region.lifeX - lifeTextWidth / 2;
//             display.setCursor(lifeX, region.lifeY);
//             display.print(lifeLine);

//             // Full poison string
//             String poisonLine = "P" + String(id + 1) + " PSN: " + String(id * 2);
//             int poisonTextWidth = poisonLine.length() * 6;
//             int poisonX = region.poisonX - poisonTextWidth / 2;
//             display.setCursor(poisonX, region.poisonY);
//             display.print(poisonLine);

//             // Turn marker
//             display.setCursor(region.turnX, region.turnY);
//             display.print(id == 0 ? "*" : " ");
//         }
//     } while (display.nextPage());
// #else
//     EPD_2IN9_V2_Init();
//     EPD_2IN9_V2_Clear();

//     frameBuffer = (UBYTE*)malloc(EPD_2IN9_V2_WIDTH * EPD_2IN9_V2_HEIGHT / 8);
//     Paint_NewImage(frameBuffer, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 0, WHITE);
//     Paint_SelectImage(frameBuffer);
//     Paint_Clear(WHITE);
// #endif

#ifdef WOKWI
  display.init(115200);
  display.setRotation(1);
  display.setFullWindow();
  display.setTextColor(GxEPD_BLACK);
  display.setFont();

  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.drawRect(10, 10, 100, 50, GxEPD_BLACK);  // Visual check
    display.setCursor(20, 40);
    display.print("HELLO WOKWI");
  } while (display.nextPage());
#endif

}

/**
 * Draws a blinking circle to indicate current player's turn.
 */
void DisplayManager::drawTurnMarker(int x, int y) {
    #ifdef WOKWI
    static unsigned long lastUpdate = 0;
    unsigned long now = millis();
    if (now - lastUpdate >= 1000) {
        display.firstPage();
        do {
            display.setCursor(x, y);
            display.print(toggle ? "*" : " ");
        } while (display.nextPage());
        toggle = !toggle;
        lastUpdate = now;
    }
#else
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
#ifdef WOKWI
    if (playerId >= 4) return;
    const auto& region = PLAYER_LAYOUT[playerId];
    if (isTurn) toggle = true;
    drawTurnMarker(region.turnX, region.turnY);
#else
    if (playerId >= 4) return;
    const auto& region = PLAYER_LAYOUT[playerId];
    if (isTurn) toggle = true;
    drawTurnMarker(region.turnX, region.turnY);
#endif
}

void DisplayManager::drawLife(uint8_t playerId, int life) {
#ifdef WOKWI
    if (playerId >= 4) return;
    static unsigned long lastLifeUpdate[4] = {0};
    unsigned long now = millis();
    if (now - lastLifeUpdate[playerId] >= 1000) {
        const auto& region = PLAYER_LAYOUT[playerId];
        String lifeStr = (life <= 0 ? "ELIM" : String(life));
        int textWidth = lifeStr.length() * 6;  // Approximate width
        int x = region.lifeX - textWidth / 2;
        display.firstPage();
        do {
            display.setCursor(x, region.lifeY);
            display.printf("P%d: %s", playerId + 1, lifeStr.c_str());
        } while (display.nextPage());
        lastLifeUpdate[playerId] = now;
    }
#else
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
#ifdef WOKWI
    if (playerId >= 4) return;
    static unsigned long lastPoisonUpdate[4] = {0};
    unsigned long now = millis();
    if (now - lastPoisonUpdate[playerId] >= 1000) {
        const auto& region = PLAYER_LAYOUT[playerId];
        String poisonStr = String(poison);
        int textWidth = poisonStr.length() * 6;  // Approximate width
        int x = region.poisonX - textWidth / 2;
        display.firstPage();
        do {
            display.setCursor(x, region.poisonY);
            display.printf("P%d PSN: %d", playerId + 1, poison);
        } while (display.nextPage());
        lastPoisonUpdate[playerId] = now;
    }
#else
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
    #ifdef WOKWI
    if (playerId >= 4) return;
    static PlayerState lastRendered[4];

    if (memcmp(&lastRendered[playerId], &state, sizeof(PlayerState)) != 0) {
        drawLife(playerId, state.eliminated ? 0 : state.life);
        drawPoison(playerId, state.poison);
        updateTurnIndicator(playerId, state.isTurn);
        lastRendered[playerId] = state;
    }
#else
    if (playerId >= 4) return;

    drawLife(playerId, state.eliminated ? 0 : state.life);
    drawPoison(playerId, state.poison);
    updateTurnIndicator(playerId, state.isTurn);
#endif
}