
#ifndef CONFIG_H
#define CONFIG_H

// Pin definitions
#define EPD_CS_PIN 5
#define EPD_DC_PIN 17
#define EPD_RST_PIN 16
#define EPD_BUSY_PIN 4


// epd = ???
#define EPD_DT 26 // rotary encoder
//#define EPD_CLK 13 // rotary encoder
#define EPD_CLK 17 // rotary encoder

// #define EPD_BUTTON_ENCODER 27 // encoder button
#define EPD_BUTTON_ENCODER 21 // encoder button

#define EPD_BUZZ 18 // piezo buzzer

// #define EPD_BUTTON 21 // end turn button
//#define EPD_BUTTON 25 // end turn button
#define EPD_BUTTON 19 // end turn button
#define EPD_BUTTON_LED 14 // button's LED

#define EPD_ONBOARD_LED 2 // onboard LED



//#define EPD_LED 99 // button's LED <-replace with real pin

#endif
