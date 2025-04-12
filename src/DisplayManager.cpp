
#include "DisplayManager.h"

void DisplayManager::begin() {
    DEV_Module_Init();
    EPD_2IN9_V2_Init();
    EPD_2IN9_V2_Clear();
    DEV_Delay_ms(500);

    UWORD ImageSize = (EPD_2IN9_V2_WIDTH / 8) * EPD_2IN9_V2_HEIGHT;
    BlackImage = (UBYTE *)malloc(ImageSize);
    Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 270, WHITE);
}

void DisplayManager::drawLifeCounter(int lifeP1, int lifeP2, int lifeP3, int lifeP4, int currentTurnPlayerID, int myPlayerID) {
    Paint_Clear(WHITE);

    Paint_DrawString_EN(10, 5, "LIFESTREAM", &Font16, WHITE, BLACK);

    char buffer[30];

    sprintf(buffer, "Planeswalker 1: %d %s", lifeP1, (currentTurnPlayerID == 0) ? "<-" : "");
    Paint_DrawString_EN(10, 30, buffer, &Font12, WHITE, BLACK);

    sprintf(buffer, "  Planeswalker 2: %d %s", lifeP2, (currentTurnPlayerID == 1) ? "<-" : "");
    Paint_DrawString_EN(10, 45, buffer, &Font12, WHITE, BLACK);

    sprintf(buffer, "    Planeswalker 3: %d %s", lifeP3, (currentTurnPlayerID == 2) ? "<-" : "");
    Paint_DrawString_EN(10, 60, buffer, &Font12, WHITE, BLACK);

    sprintf(buffer, "      Planeswalker 4: %d %s", lifeP4, (currentTurnPlayerID == 3) ? "<-" : "");
    Paint_DrawString_EN(10, 75, buffer, &Font12, WHITE, BLACK);

    sprintf(buffer, "You are Planeswalker %d", myPlayerID + 1);
    Paint_DrawString_EN(10, 100, buffer, &Font16, WHITE, BLACK);

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
