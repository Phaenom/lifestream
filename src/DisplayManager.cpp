//#include <algorithm>
#include "DisplayManager.h"
#include "GameState.h"
#include "DeviceManager.h"
#include "NetworkManager.h"
#include "Config.h"

DisplayManager display;

// Defines screen regions for each player's life, poison, and turn marker display.
struct PlayerDisplayRegion {
    int lifeX, lifeY;
    int poisonX, poisonY;
    int turnX, turnY;
};

// PLAYER_LAYOUT specifies display regions for each player.
// Each entry: {lifeX, lifeY, poisonX, poisonY, turnX, turnY}
const PlayerDisplayRegion PLAYER_LAYOUT[4] = {
    {40, 30,  45, 49,  10, 30},     // Player 1
    {198, 30, 203, 49, 168, 30},    // Player 2
    {40, 90,  45, 109,  10, 90},    // Player 3
    {198, 90, 203, 109, 168, 90}    // Player 4
};

DisplayManager::DisplayManager() {}

// Initializes the e-paper display, clears the screen, and prepares a drawing buffer.
void DisplayManager::begin() {
    // Initialize hardware interfaces
    DEV_Module_Init();
    EPD_2IN9_V2_Init();
    EPD_2IN9_V2_Clear();

    // Allocate display buffer and initialize drawing context
    displayBuffer = (UBYTE*)malloc((EPD_2IN9_V2_WIDTH * EPD_2IN9_V2_HEIGHT) / 8);
    Paint_NewImage(displayBuffer, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, ROTATE_270, WHITE);
    Paint_SelectImage(displayBuffer);
    Paint_Clear(WHITE);

    // Bounding boxes for player quadrants with 2-pixel margin for safe padding
    Paint_DrawRectangle(2, 2, 146, 62, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);     // Player 1
    Paint_DrawRectangle(150, 2, 294, 62, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);   // Player 2
    Paint_DrawRectangle(2, 66, 146, 126, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);   // Player 3
    Paint_DrawRectangle(150, 66, 294, 126, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY); // Player 4

    // Fix for ROTATE_270 coordinate system
    const int logoX = (EPD_2IN9_V2_HEIGHT - MTG_LOGO_SYMBOL_WIDTH) / 2;
    const int logoY = (EPD_2IN9_V2_WIDTH - MTG_LOGO_SYMBOL_HEIGHT) / 2;
    Paint_SetRotate(ROTATE_270);
    Paint_ClearWindows(logoX, logoY, logoX + MTG_LOGO_SYMBOL_WIDTH, logoY + MTG_LOGO_SYMBOL_HEIGHT, WHITE);

    // Draw centered title above the logo
    const char* title = "LIFESTREAM";
    int titleWidth = strlen(title) * Font16.Width;
    int titleX = (EPD_2IN9_V2_HEIGHT - titleWidth) / 2;
    int titleY = 0; // Align with top of screen
    Paint_DrawString_EN(titleX, titleY, title, &Font16, BLACK, WHITE);
    drawLogo(logoX, logoY);

    // Draw player labels in each quadrant
    Paint_DrawString_EN(0, 0, "P1", &Font12, BLACK, WHITE);             // Top-left
    Paint_DrawString_EN(280, 0, "P2", &Font12, BLACK, WHITE);           // Top-right
    Paint_DrawString_EN(0, 115, "P3", &Font12, BLACK, WHITE);           // Bottom-left
    Paint_DrawString_EN(280, 115, "P4", &Font12, BLACK, WHITE);         // Bottom-right

    EPD_2IN9_V2_Display(displayBuffer);

    DEV_Delay_ms(10);
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

    x = std::max(5, std::min(x, EPD_2IN9_V2_HEIGHT - 5));
    y = std::max(5, std::min(y, EPD_2IN9_V2_WIDTH - 5));
    if (toggle) {
        Paint_DrawCircle(x, y, 5, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    } else {
        Paint_DrawCircle(x, y, 5, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    }
    LOGF("[Display] Drawing turn marker at (%d, %d), state: %s\n", x, y, toggle ? "BLACK" : "WHITE");
    toggle = !toggle;
    EPD_2IN9_V2_Display(displayBuffer);
}

/**
 * Clears the turn marker at the specified (x, y) by drawing a white-filled circle.
 */
void DisplayManager::clearTurnMarker(int x, int y) {
    Paint_DrawCircle(x, y, 5, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    EPD_2IN9_V2_Display_Partial(displayBuffer);
}

// Updates the turn indicator for the specified player.
// Temporarily disabled: logic is commented out for future reactivation.
void DisplayManager::updateTurnIndicator(uint8_t playerId, bool isTurn) {
    // Turn marker display is temporarily disabled.
    // if (playerId >= 4) return;
    // const auto& region = PLAYER_LAYOUT[playerId];
    // if (isTurn) {
    //     toggle = true;
    //     drawTurnMarker(region.turnX, region.turnY);
    // } else {
    //     clearTurnMarker(region.turnX, region.turnY);
    // }
}

// Renders the life total for the specified player in their assigned screen region.
// Displays "ELIMINATED" if life is zero or below.
void DisplayManager::drawLife(uint8_t playerId, int life) {
    static int lastLife[4] = {-1, -1, -1, -1};
    if (playerId >= 4) return;
    if (lastLife[playerId] == life) return;
    lastLife[playerId] = life;

    const auto& region = PLAYER_LAYOUT[playerId];

    const char* str = nullptr;
    char buf[16] = {0};
    if (life <= 0) {
        str = "ELIMINATED";
    } else {
        snprintf(buf, sizeof(buf), "%d [+]", life);
        str = buf;
    }

    if (&Font12 == nullptr || str == nullptr) {
        LOGLN("[Display] Font12 or str is null — skipping draw");
        return;
    }

    int textWidth = strlen(str) * Font12.Width;
    int x = region.lifeX - textWidth / 2;
    x = std::max(0, std::min(x, EPD_2IN9_V2_HEIGHT - textWidth));
    int y = std::max(0, std::min(region.lifeY - 8, 128 - 16));

    if (displayBuffer == nullptr) {
        LOGLN("[Display] displayBuffer is null — skipping partial draw");
        return;
    }

    int x2 = std::min(x + 100, EPD_2IN9_V2_HEIGHT);
    int y2 = std::min(y + 20, EPD_2IN9_V2_HEIGHT);
    Paint_ClearWindows(x, y, x2, y2, WHITE);
    Paint_DrawString_EN(x, y, str, &Font12, WHITE, BLACK);

    EPD_2IN9_V2_Display_Partial(displayBuffer);
}

// Renders the poison counter for the specified player in their assigned screen region.
void DisplayManager::drawPoison(uint8_t playerId, int poison) {
    static int lastPoison[4] = {-1, -1, -1, -1};
    if (playerId >= 4) return;
    if (lastPoison[playerId] == poison) return;
    lastPoison[playerId] = poison;

    const auto& region = PLAYER_LAYOUT[playerId];
    char buf[16];
    snprintf(buf, sizeof(buf), "%d [!]", poison);
    int textWidth = strlen(buf) * Font12.Width;
    int x = region.poisonX - textWidth / 2;
    x = std::max(0, std::min(x, EPD_2IN9_V2_HEIGHT - textWidth));
    int y = std::max(0, std::min(region.poisonY - 8, 128 - 16));
    int x2 = std::min(x + 30, EPD_2IN9_V2_HEIGHT);
    int y2 = std::min(y + 20, EPD_2IN9_V2_HEIGHT);
    Paint_ClearWindows(x, y, x2, y2, WHITE);
    Paint_DrawString_EN(x, y, buf, &Font12, WHITE, BLACK);

    EPD_2IN9_V2_Display_Partial(displayBuffer);
}

/**
 * Renders the full state (life, poison, turn marker) for the given player ID.
 * Combines calls to drawLife, drawPoison, and updateTurnIndicator.
 */
void DisplayManager::renderPlayerState(uint8_t playerId, const PlayerState& state) {
    LOGF("[Display] Rendering P%d: life=%d, poison=%d, turn=%d\n",
                  playerId, state.life, state.poison, state.isTurn);

    if (displayBuffer == nullptr) {
        LOGLN("[Display] displayBuffer is null!");
        return;
    }

    if (playerId >= 4) return;

    static int cachedLife[4] = {-1, -1, -1, -1};
    static int cachedPoison[4] = {-1, -1, -1, -1};
    static bool cachedTurn[4] = {false, false, false, false};

    int currentLife = state.eliminated ? 0 : state.life;

    if (cachedLife[playerId] != currentLife) {
        LOGLN("[Display] → drawLife()");
        cachedLife[playerId] = currentLife;
        drawLife(playerId, currentLife);
    }

    if (cachedPoison[playerId] != state.poison) {
        LOGLN("[Display] → drawPoison()");
        cachedPoison[playerId] = state.poison;
        drawPoison(playerId, state.poison);
    }

    if (cachedTurn[playerId] != state.isTurn) {
        LOGLN("[Display] → updateTurnIndicator()");
        updateTurnIndicator(playerId, state.isTurn);
        cachedTurn[playerId] = state.isTurn;
    }
}

void DisplayManager::renderAllPlayerStates(const GameState& state) {
    for (int i = 0; i < 4; ++i) {
        renderPlayerState(i, state.getPlayerState(i));
    }
}

void DisplayManager::drawLogo(int x, int y) {
    for (int j = 0; j < MTG_LOGO_SYMBOL_HEIGHT; j++) {
        for (int i = 0; i < MTG_LOGO_SYMBOL_WIDTH; i++) {
            int byteIndex = (i + j * MTG_LOGO_SYMBOL_WIDTH) / 8;
            int bitIndex = 7 - (i % 8);

            if (mtg_logo_symbol[byteIndex] & (1 << bitIndex)) {
                // Set pixel in buffer
                Paint_DrawPoint(x + i, y + j, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
            }
        }
    }
}

void DisplayManager::drawDeviceRole() {
    const char* roleText = "";
    switch (device.getRole()) {
        case ROLE_HOST:
            roleText = "HOST";
            break;
        case ROLE_CLIENT:
            roleText = "CLIENT";
            break;
        default:
            roleText = "UNASSIGNED";
            break;
    }

    // Account for 270° screen rotation (296 x 128 screen)
    int textLength = strlen(roleText);
    int textWidth = textLength * 6;       // Adjust if your font differs
    int screenWidth = 296;
    int screenHeight = 128;

    int x = (screenWidth - textWidth) / 2;  // Centered horizontally
    int y = screenHeight - 12;              // 12px font height → y = 116

    Paint_DrawString_EN(x, y, roleText, &Font12, BLACK, WHITE);
}

void DisplayManager::flush() {
    if (displayBuffer == nullptr) {
        LOGLN("[Display] flush() skipped — displayBuffer is null");
        return;
    }

    EPD_2IN9_V2_Display(displayBuffer);
}

const UBYTE* DisplayManager::getDisplayBuffer() const {
    return displayBuffer;
}