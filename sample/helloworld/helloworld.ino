/* Includes ------------------------------------------------------------------*/
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"

/* Entry point ----------------------------------------------------------------*/
void setup()
{
    //  utput a message to the serial terminal (for debugging)
    printf("EPD_2IN9_V2_test Demo\r\n");

    // Initialize low-level hardware interfaces (GPIOs, SPI, etc.)
    DEV_Module_Init();

    // Initialize the e-Paper display and clear its contents
    printf("e-Paper Init and Clear...\r\n");
    EPD_2IN9_V2_Init();        // Initialize the e-paper hardware
    EPD_2IN9_V2_Clear();       // Clear the screen (sets it to white)
    DEV_Delay_ms(500);         // Short delay for stability

    // Create a pointer for the display image buffer
    UBYTE *BlackImage;

    // Calculate image buffer size in bytes
    // Each byte stores 8 pixels (monochrome), so width in bytes = width / 8
    // Multiply by height to get total size
    UWORD Imagesize = ((EPD_2IN9_V2_WIDTH % 8 == 0) ? 
                      (EPD_2IN9_V2_WIDTH / 8) : 
                      (EPD_2IN9_V2_WIDTH / 8 + 1)) * 
                      EPD_2IN9_V2_HEIGHT;

    // Dynamically allocate memory for the image buffer
    if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        // If allocation fails, print error and halt
        printf("Failed to allocate memory...\r\n");
        while (1);  // Infinite loop to stop program
    }

    // Initialize the painting module with the image buffer
    // Parameters: buffer, width, height, rotation (0/90/180/270), background color
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 270, WHITE);

    // Clear the buffer to all white (background color)
    Paint_Clear(WHITE);

    // Draw a string on the image buffer
    // X = 10, Y = 60, text = "Hello, World!", font = Font16, text color = BLACK, background = WHITE
    Paint_DrawString_EN(10, 60, "Hello, World!", &Font16, WHITE, BLACK);

    // Send the prepared buffer to the e-paper display
    EPD_2IN9_V2_Display(BlackImage);

    // Wait a bit before sleep (not strictly required)
    DEV_Delay_ms(2000);

    // Put the e-paper display into deep sleep mode to save power
    EPD_2IN9_V2_Sleep();

    // Free the image buffer memory (good practice in embedded apps)
    free(BlackImage);
}

/* The main loop -------------------------------------------------------------*/
void loop()
{
  //
}
