#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#pragma once
#include "GameState.h"
/*
  InputManager Class (Stub)
  Designed to handle inputs from Rotary Encoder and Buttons.
  Currently empty for future expansion.
*/

namespace InputManager {
    // Initialize the input system (normally used for setting up encoders/buttons).
    void begin();

    // Get encoder-like input (returns -1, 0, or +1).
    int getEncoderDelta();

    // Returns true if a "button press" is detected (simulated via Serial).
    bool buttonPressed();

    // Prompts user to select the number of players.
    u_int8_t selectNumberOfPlayers();

    // Prompts user to choose a starting life total.
    int selectStartingLife();

    // Prompts user to choose a game type ("Standard", etc.).
    String selectGameType();

    // Prompts user to confirm the full GameSettings configuration.
    bool confirmGameSettings(const GameState& settings);
}

// class InputManager {
// public:
//     // Setup hardware for inputs
//     void begin() {
//         // Future implementation
//     }

//     // Update function to read inputs each loop
//     void update() {
//         // Future implementation
//     }
// };

#endif
