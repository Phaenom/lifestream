#ifndef CONFIG_H
#define CONFIG_H

#pragma once

// =====================================================
// DEBUG MODE
// Set to 1 to enable debug logs and serial UART
// Set to 0 for power-optimized release mode
// =====================================================
#define ENABLE_DEBUG 0

// =================================================================================
//
// HEADER FILE INCLUDES
//
// =================================================================================

/* #include "NetworkManager.h"
#include "DisplayManager.h"
#include "DeviceManager.h"
#include "GameSetup.h"
#include "GameState.h"
#include "BatteryManager.h"
#include "EEPROMManager.h"
#include "Utils.h"
#include "IInputManager.h"
#include "HardwareManager.h" */

// =================================================================================
//
// HARDWARE PIN CONFIGURATION
//
// =================================================================================

// --- Rotary Encoder Pins ---
#define PIN_ENCODER_DT              26  // Rotary encoder data pin
#define PIN_ENCODER_CLK             17  // Rotary encoder clock pin
#define PIN_ENCODER_BUTTON  21  // Rotary encoder push button

// --- Arcade Button + LED ---
#define PIN_ARCADE_BUTTON          19  // End turn (arcade) button
#define PIN_ARCADE_LED      14  // LED inside arcade button

// --- Buzzer ---
#define PIN_BUZZ            18  // Piezo buzzer output pin

// --- Onboard LED (unused) ---
#define PIN_ONBOARD_LED      2  // Onboard LED (currently unused)

// =================================================================================
//
// LOGGING MACRO
//
//     HOW TO USE?
//         IN setup()
//             LOG_BEGIN(115200);
//             LOGLN("Device starting up...");
//
//         Anywhere else:
//             LOG("Life total: ");
//             LOGLN(life);
//
//         With formatting:
//             LOGF("Life: %d, Poison: %d\n", life, poison);
//
// =================================================================================

#if ENABLE_DEBUG
  #define LOG_BEGIN(baud) Serial.begin(baud)
  #define LOG(x) Serial.print(x)
  #define LOGLN(x) Serial.println(x)
  #define LOGF(...) Serial.printf(__VA_ARGS__)
#else
  #define LOG_BEGIN(baud)
  #define LOG(x)
  #define LOGLN(x)
  #define LOGF(...)
#endif

#endif