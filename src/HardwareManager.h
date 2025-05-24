#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

/* 
| Button  | Action | Press Type               | Result |
| ------- | ------ | ------------------------ | ------ |
| Encoder | Short  | Toggle life/poison       |        |
| Encoder | Long   | Reset local device state |        |
| Arcade  | Short  | Advance game turn        |        |
| Arcade  | Long   | Reset game state         |        |
 */

#include <ESP32Encoder.h>
#include <Arduino.h>
#include <ezButton.h>
#include "IInputManager.h"

// Input modes for rotary encoder
enum InputMode {
    MODE_LIFE = 0,
    MODE_POISON = 1
};

class HardwareManager : public IInputManager {
public:
    HardwareManager();

    void begin() override;
    void update() override;

    long update_encoder() override;
    bool isEncoderButtonReleased() override;
    bool isArcadeButtonReleased() override;

    void reset();
    InputMode getMode() const { return mode; }

private:
    // Rotary encoder
    ESP32Encoder encoder;
    int64_t oldPosition = 0;
    int64_t newPosition = 0;
    int64_t delta = 0;

    // Buttons
    ezButton encoderButton;
    ezButton arcadeButton;

    // Long press
    const unsigned long LONG_PRESS_THRESHOLD = 1000; // 1000ms = 1s
    unsigned long encoderPressStart = 0;
    unsigned long arcadePressStart = 0;
    bool encoderLongPressDetected = false;
    bool arcadeLongPressDetected = false;

    // LED
    bool ledState = LOW;

    // Input mode
    InputMode mode = MODE_LIFE;

    // Internal helpers
    void handleEncoderButton();
    void handleArcadeButton();
};

extern HardwareManager hardware;

#endif