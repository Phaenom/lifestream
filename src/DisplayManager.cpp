#include "DisplayManager.h"
#include "GameState.h"
#include "DeviceManager.h"
#include "NetworkManager.h"
#include "Config.h"
#include "HardwareManager.h"

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
    {74, 30,  74, 49,  10, 30},     // Player 1
    {222, 30, 222, 49, 168, 30},    // Player 2
    {74, 90,  74, 109,  10, 90},    // Player 3
    {222, 90, 222, 109, 168, 90}    // Player 4
};

DisplayManager::DisplayManager() {}

// Initializes the e-paper display, clears the screen, and prepares a drawing buffer.
void DisplayManager::begin() {
    DEV_Module_Init();
    EPD_2IN9_V2_Init();
    EPD_2IN9_V2_Clear();

    displayBuffer = (UBYTE*)malloc((EPD_2IN9_V2_WIDTH * EPD_2IN9_V2_HEIGHT) / 8);
    Paint_NewImage(displayBuffer, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, ROTATE_270, WHITE);
    Paint_SelectImage(displayBuffer);
    Paint_Clear(WHITE);

    Paint_DrawRectangle(2, 2, 146, 62, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(150, 2, 294, 62, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(2, 66, 146, 126, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(150, 66, 294, 126, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

    const int logoX = (EPD_2IN9_V2_HEIGHT - MTG_LOGO_SYMBOL_WIDTH) / 2;
    const int logoY = (EPD_2IN9_V2_WIDTH - MTG_LOGO_SYMBOL_HEIGHT) / 2;
    Paint_SetRotate(ROTATE_270);
    Paint_ClearWindows(logoX, logoY, logoX + MTG_LOGO_SYMBOL_WIDTH, logoY + MTG_LOGO_SYMBOL_HEIGHT, WHITE);

    const char* title = "LIFESTREAM";
    int titleWidth = strlen(title) * Font16.Width;
    int titleX = (EPD_2IN9_V2_HEIGHT - titleWidth) / 2;
    int titleY = 0;
    Paint_DrawString_EN(titleX, titleY, title, &Font16, BLACK, WHITE);
    drawLogo(logoX, logoY);

    Paint_DrawString_EN(0, 0, "P1", &Font12, BLACK, WHITE);
    Paint_DrawString_EN(280, 0, "P2", &Font12, BLACK, WHITE);
    Paint_DrawString_EN(0, 115, "P3", &Font12, BLACK, WHITE);
    Paint_DrawString_EN(280, 115, "P4", &Font12, BLACK, WHITE);

    EPD_2IN9_V2_Display(displayBuffer);
    DEV_Delay_ms(10);
}

void DisplayManager::drawLife(uint8_t playerId, int life, bool isLocalPlayer) {
    if (playerId >= 4) return;

    const auto& region = PLAYER_LAYOUT[playerId];
    char buf[24] = {0};
    const char* lifeStr = nullptr;

    if (life <= 0) {
        lifeStr = "ELIMINATED";
    } else {
        snprintf(buf, sizeof(buf), "%d [+]", life);
        lifeStr = buf;
    }

    int textWidth = strlen(lifeStr) * Font12.Width;
    int x = std::max(0, region.lifeX - textWidth / 2);
    int y = std::max(0, std::min(region.lifeY - 8, 128 - 16));
    int x2 = std::min(x + textWidth + 16, EPD_2IN9_V2_HEIGHT);
    int y2 = y + 20;

    Paint_ClearWindows(x, y, x2, y2, WHITE);
    Paint_DrawString_EN(x, y, lifeStr, &Font12, WHITE, BLACK);

    if (isLocalPlayer && hardware.getMode() == MODE_LIFE && life > 0) {
        Paint_DrawString_EN(x - 16, y, "->", &Font12, WHITE, BLACK);
    }

    EPD_2IN9_V2_Display_Partial(displayBuffer);
}

void DisplayManager::drawPoison(uint8_t playerId, int poison, bool isLocalPlayer) {
    if (playerId >= 4) return;

    const auto& region = PLAYER_LAYOUT[playerId];
    char buf[24];
    snprintf(buf, sizeof(buf), "%d [!]", poison);

    int textWidth = strlen(buf) * Font12.Width;
    int x = std::max(0, region.poisonX - textWidth / 2);
    int y = std::max(0, std::min(region.poisonY - 8, 128 - 16));
    int x2 = std::min(x + textWidth + 16, EPD_2IN9_V2_HEIGHT);
    int y2 = y + 20;

    Paint_ClearWindows(x, y, x2, y2, WHITE);
    Paint_DrawString_EN(x, y, buf, &Font12, WHITE, BLACK);

    if (isLocalPlayer && hardware.getMode() == MODE_POISON) {
        Paint_DrawString_EN(x - 16, y, "->", &Font12, WHITE, BLACK);
    }

    EPD_2IN9_V2_Display_Partial(displayBuffer);
}

void DisplayManager::renderPlayerState(uint8_t playerId, const PlayerState& state, bool isPoisonMode, bool isLocalPlayer) {
    static int cachedLife[4] = {-1, -1, -1, -1};
    static int cachedPoison[4] = {-1, -1, -1, -1};

    if (playerId >= 4) return;

    int currentLife = state.eliminated ? 0 : state.life;

    if (cachedLife[playerId] != currentLife || (!isPoisonMode && isLocalPlayer)) {
        cachedLife[playerId] = currentLife;
        drawLife(playerId, currentLife, isLocalPlayer && !isPoisonMode);
    }

    if (cachedPoison[playerId] != state.poison || (isPoisonMode && isLocalPlayer)) {
        cachedPoison[playerId] = state.poison;
        drawPoison(playerId, state.poison, isLocalPlayer && isPoisonMode);
    }
}

void DisplayManager::renderAllPlayerStates(const GameState& state) {
    bool isPoisonMode = (hardware.getMode() == MODE_POISON);
    for (int i = 0; i < 4; ++i) {
        bool isLocal = (i == device.getPlayerID());
        renderPlayerState(i, state.getPlayerState(i), isPoisonMode, isLocal);
    }
}

void DisplayManager::drawLogo(int x, int y) {
    for (int j = 0; j < MTG_LOGO_SYMBOL_HEIGHT; j++) {
        for (int i = 0; i < MTG_LOGO_SYMBOL_WIDTH; i++) {
            int byteIndex = (i + j * MTG_LOGO_SYMBOL_WIDTH) / 8;
            int bitIndex = 7 - (i % 8);
            if (mtg_logo_symbol[byteIndex] & (1 << bitIndex)) {
                Paint_DrawPoint(x + i, y + j, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
            }
        }
    }
}

void DisplayManager::drawDeviceRole() {
    const char* roleText = "";
    switch (device.getRole()) {
        case ROLE_HOST: roleText = "HOST"; break;
        case ROLE_CLIENT: roleText = "CLIENT"; break;
        default: roleText = "UNASSIGNED"; break;
    }

    int textWidth = strlen(roleText) * Font12.Width;
    int x = (296 - textWidth) / 2;
    int y = 116;

    Paint_DrawString_EN(x, y, roleText, &Font12, BLACK, WHITE);
}

void DisplayManager::flush() {
    if (displayBuffer) {
        EPD_2IN9_V2_Display(displayBuffer);
    }
}

const UBYTE* DisplayManager::getDisplayBuffer() const {
    return displayBuffer;
}

void DisplayManager::refreshAll() {
    Paint_Clear(WHITE);

    // Redraw bounding boxes
    Paint_DrawRectangle(2, 2, 146, 62, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(150, 2, 294, 62, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(2, 66, 146, 126, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(150, 66, 294, 126, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

    // Redraw player labels
    Paint_DrawString_EN(0, 0, "P1", &Font12, BLACK, WHITE);
    Paint_DrawString_EN(280, 0, "P2", &Font12, BLACK, WHITE);
    Paint_DrawString_EN(0, 115, "P3", &Font12, BLACK, WHITE);
    Paint_DrawString_EN(280, 115, "P4", &Font12, BLACK, WHITE);

    // Redraw title and logo
    const int logoX = (EPD_2IN9_V2_HEIGHT - MTG_LOGO_SYMBOL_WIDTH) / 2;
    const int logoY = (EPD_2IN9_V2_WIDTH - MTG_LOGO_SYMBOL_HEIGHT) / 2;
    Paint_ClearWindows(logoX, logoY, logoX + MTG_LOGO_SYMBOL_WIDTH, logoY + MTG_LOGO_SYMBOL_HEIGHT, WHITE);
    const char* title = "LIFESTREAM";
    int titleWidth = strlen(title) * Font16.Width;
    int titleX = (EPD_2IN9_V2_HEIGHT - titleWidth) / 2;
    Paint_DrawString_EN(titleX, 0, title, &Font16, BLACK, WHITE);
    drawLogo(logoX, logoY);

    // Redraw all player states from current game state
    renderAllPlayerStates(gameState);

    // Perform full refresh
    flush();
}

