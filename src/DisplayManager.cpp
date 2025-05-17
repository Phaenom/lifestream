#include "DisplayManager.h"

void DisplayManager::begin() {
    // Initialize hardware interfaces
    DEV_Module_Init();

    // Initialize ePaper display
    EPD_2IN9_V2_Init();
    EPD_2IN9_V2_Clear();
    DEV_Delay_ms(500);

    // Allocate memory for image buffer
    UWORD ImageSize = (EPD_2IN9_V2_WIDTH / 8) * EPD_2IN9_V2_HEIGHT;
    BlackImage = (UBYTE *)malloc(ImageSize);
    
    // Initialize new image
    Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 270, WHITE);
}

void DisplayManager::drawLifeCounter(int lifeP1, int lifeP2, int lifeP3, int lifeP4, int currentTurnPlayerID, int myPlayerID, DeviceRole role) {
    Paint_Clear(WHITE);

    // Draw MTG Symbol logo at top center
    drawLogo(250, -5); 

    Paint_DrawString_EN(10, 5, "LIFESTREAM", &Font16, WHITE, BLACK);

    char buffer[40];

    // Display life totals for all players with turn indicator
    sprintf(buffer, "Planeswalker 1: %d %s", lifeP1, (currentTurnPlayerID == 0) ? "<-" : "");
    Paint_DrawString_EN(10, 30, buffer, &Font12, WHITE, BLACK);

    sprintf(buffer, "  Planeswalker 2: %d %s", lifeP2, (currentTurnPlayerID == 1) ? "<-" : "");
    Paint_DrawString_EN(10, 45, buffer, &Font12, WHITE, BLACK);

    sprintf(buffer, "    Planeswalker 3: %d %s", lifeP3, (currentTurnPlayerID == 2) ? "<-" : "");
    Paint_DrawString_EN(10, 60, buffer, &Font12, WHITE, BLACK);

    sprintf(buffer, "      Planeswalker 4: %d %s", lifeP4, (currentTurnPlayerID == 3) ? "<-" : "");
    Paint_DrawString_EN(10, 75, buffer, &Font12, WHITE, BLACK);

    // Display player role (HOST / CLIENT)
    const char* roleStr = "UNKNOWN";
    if (role == ROLE_HOST) {
        roleStr = "HOST";
    } else if (role == ROLE_CLIENT) {
        roleStr = "CLIENT";
    }

    sprintf(buffer, "You are Planeswalker %d (%s)", myPlayerID + 1, roleStr);
    Paint_DrawString_EN(5, 100, buffer, &Font12, WHITE, BLACK);

    EPD_2IN9_V2_Display(BlackImage);
    DEV_Delay_ms(500);
}

void DisplayManager::drawPoisonCounter(int psnP1, int psnP2, int psnP3, int psnP4, int currentTurnPlayerID, int myPlayerID, DeviceRole role) {
    Paint_Clear(WHITE);

    // Draw MTG Symbol logo at top center
    //EPD_2IN9_V2_WIDTH / 8) * EPD_2IN9_V2_HEIGHT

    // drawPoison(10, 100); 

    Paint_DrawString_EN(60, 5, "POISONSTREAM", &Font16, WHITE, BLACK);
    Paint_DrawLine(0, 25, 295, 25, BLACK, DOT_PIXEL_2X2,LINE_STYLE_SOLID); // horiztonal line at top

    char buffer[40];

    // POISON COUNTS
    // sprintf(buffer, "Planeswalker Poison");
    // Paint_DrawString_EN(60, 30, buffer, &Font12, WHITE, BLACK);
    
    sprintf(buffer, "P1: %d", psnP1);
    Paint_DrawString_EN(60, 30, buffer, &Font12, WHITE, BLACK);

    sprintf(buffer, "P2: %d", psnP2);
    Paint_DrawString_EN(60, 45, buffer, &Font12, WHITE, BLACK);

    sprintf(buffer, "P3: %d", psnP3);
    Paint_DrawString_EN(60, 60, buffer, &Font12, WHITE, BLACK);

    sprintf(buffer, "P4: %d", psnP4);
    Paint_DrawString_EN(60, 75, buffer, &Font12, WHITE, BLACK);

    // OWN POISON
    sprintf(buffer, "%d", psnP1);
    Paint_DrawString_EN(240, 60, buffer, &Font24, WHITE, BLACK);


    // TURN STUFF 
    sprintf(buffer, "P1 %s", (currentTurnPlayerID == 0) ? "<-" : "");
    Paint_DrawString_EN(0, 30, buffer, &Font12, WHITE, BLACK);
    
    sprintf(buffer, "P2 %s", (currentTurnPlayerID == 1) ? "<-" : "");
    Paint_DrawString_EN(0, 45, buffer, &Font12, WHITE, BLACK);
    
    sprintf(buffer, "P3 %s", (currentTurnPlayerID == 2) ? "<-" : "");
    Paint_DrawString_EN(0, 60, buffer, &Font12, WHITE, BLACK);

    sprintf(buffer, "P4 %s", (currentTurnPlayerID == 3) ? "<-" : "");
    Paint_DrawString_EN(0, 75, buffer, &Font12, WHITE, BLACK);


    // Paint_DrawVerticalLine(10, 75, 2, BLACK);
    // Paint_DrawHorizontalLine(10, 75, 2, BLACK);
    // Y = 128 -1 = 127
    // X = 296 -1 = 295
    // x y x y
    Paint_DrawLine(45, 25, 45, 127, BLACK, DOT_PIXEL_1X1,LINE_STYLE_SOLID); // vertical line for turn order
    Paint_DrawLine(200, 25, 200, 127, BLACK, DOT_PIXEL_1X1,LINE_STYLE_SOLID); // vertical line for turn order
//EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT

    // Display player role (HOST / CLIENT)
    const char* roleStr = "UNKNOWN";
    if (role == ROLE_HOST) {
        roleStr = "HOST";
    } else if (role == ROLE_CLIENT) {
        roleStr = "CLIENT";
    }

    sprintf(buffer, "ID: P %d (%s)", myPlayerID + 1, roleStr);
    Paint_DrawString_EN(60, 110, buffer, &Font12, WHITE, BLACK);

    EPD_2IN9_V2_Display(BlackImage);
    DEV_Delay_ms(500);
}


void DisplayManager::sleep() {
    EPD_2IN9_V2_Sleep();

#if defined(DEV_Module_Exit)
    DEV_Module_Exit();
#else
    SPI.end();
#endif

    if (BlackImage != NULL) {
        free(BlackImage);
        BlackImage = NULL;
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

void DisplayManager::drawPoison(int x, int y) {
    for (int j = 0; j < PSN_SYMBOL_HEIGHT; j++) {
        for (int i = 0; i < PSN_SYMBOL_WIDTH; i++) {
            int byteIndex = (i + j * PSN_SYMBOL_WIDTH) / 8;
            int bitIndex = 7 - (i % 8);

            if (psn_symbol[byteIndex] & (1 << bitIndex)) {
                // Set pixel in buffer
                Paint_DrawPoint(x + i, y + j, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
            }
        }
    }
}
