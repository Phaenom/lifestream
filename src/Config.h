#ifndef CONFIG_H
#define CONFIG_H

// =================================================================================
//
// HEADER FILE INCLUDES
//
// =================================================================================

#include "NetworkManager.h"
#include "DisplayManager.h"
#include "DeviceManager.h"
#include "GameSetup.h"
#include "GameState.h"
#include "BatteryManager.h"
#include "EEPROMManager.h"
#include "Utils.h"
#include "IInputManager.h"
#include "HardwareManager.h"

// =================================================================================
//
// HARDWARE PIN CONFIGURATION
//
// =================================================================================

// --- Rotary Encoder Pins ---
#define EPD_DT              26  // Rotary encoder data pin
#define EPD_CLK             17  // Rotary encoder clock pin
#define EPD_BUTTON_ENCODER  21  // Rotary encoder push button

// --- Arcade Button + LED ---
#define EPD_BUTTON          19  // End turn (arcade) button
#define EPD_BUTTON_LED      14  // LED inside arcade button

// --- Buzzer ---
#define EPD_BUZZ            18  // Piezo buzzer output pin

// --- Onboard LED (unused) ---
#define EPD_ONBOARD_LED      2  // Onboard LED (currently unused)

#endif