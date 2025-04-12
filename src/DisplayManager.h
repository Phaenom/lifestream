
#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <SPI.h>
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"

class DisplayManager {
public:
    void begin();
    void drawLifeCounter(int lifeP1, int lifeP2, int lifeP3, int lifeP4, int currentTurnPlayerID, int myPlayerID);
    void sleep();

private:
    UBYTE *BlackImage;
};

#endif
