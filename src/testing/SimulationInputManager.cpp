#include "GameState.h"
#include "testing/SimulationInputManager.h"
#include "Config.h"

// Initializes the simulation input manager.
// Outputs a startup message to Serial for confirmation.
void SimulationInputManager::begin() {
    Serial.println("\n[SimInput] Simulation input initialized.");
    Serial.printf("\n[InputManager] Update cycle at %lu ms", millis());
}

// Checks for incoming serial input and maps characters to simulated input behavior.
// Allows for life/poison adjustment, turn advancement, reset, and redraws for simulation testing.
void SimulationInputManager::update() {
    static int selectedPlayer = 0;
    if (!Serial.available()) return;

    char input = tolower(Serial.read());
    Serial.printf("[SimInput] Received char: 0x%02X (%c)\n", input, input);

    unsigned long now = millis();
    extern GameState gameState;
    extern DisplayManager display;

    switch (input) {
        case '1': case '2': case '3': case '4':
            // Select target player by number
            selectedPlayer = input - '1';
            Serial.printf("[SimInput] Targeting player %d\n", selectedPlayer);
            break;

        case 'w':
        case 's': {
            int delta = (input == 'w') ? 1 : -1;
            int oldPoison = gameState.getPlayerState(selectedPlayer).poison;

            gameState.adjustPoison(selectedPlayer, delta);

            if (network.getRole() == ROLE_HOST &&
                gameState.getPlayerState(selectedPlayer).poison != oldPoison) {
                network.sendGameState();
            }

            if (network.getRole() == ROLE_CLIENT) {
                uint8_t newPoison = gameState.getPlayerState(selectedPlayer).poison;
                network.sendPoisonChangeRequest(selectedPlayer, newPoison);
            }
            break;
}

        case 'a':
        case 'd': {
            int delta = (input == 'a') ? -1 : 1;
            simulateRotation(delta);

            int oldLife = gameState.getLife(selectedPlayer);
            gameState.adjustLife(selectedPlayer, delta);

            if (network.getRole() == ROLE_HOST &&
                gameState.lifeChanged(selectedPlayer, oldLife)) {
                network.sendGameState();
            }

            if (network.getRole() == ROLE_CLIENT) {
                uint8_t newLife = gameState.getLife(selectedPlayer);
                network.sendLifeChangeRequest(selectedPlayer, newLife);
            }

            break;
        }


        case 't':
            // Advance turn to next player
            gameState.nextTurn();
            break;

        case 'x':
            // Eliminate selected player (life -> 0)
            gameState.adjustLife(selectedPlayer, -99);
            break;

        case 'v':
            // Eliminate all other players (simulate win)
            for (int i = 0; i < 4; ++i)
                if (i != selectedPlayer) gameState.adjustLife(i, -99);
            break;

        case 'r':
            // Reset all player states and refresh display
            gameState.resetAll();
            display.renderAllPlayerStates(gameState);
            Serial.println("[SimInput] All player states and turn reset.");
            break;

        case 'g':
            // Redraw current game state for all players
            display.renderAllPlayerStates(gameState);
            Serial.println("[SimInput] Redrawing all player states.");
            break;

        case 'b':
            // Simulate button hold
            buttonPressTime = now;
            buttonHeld = true;
            break;

        default:
            break;
    }

    // if (rotationDelta != 0) {
    //     Serial.printf("[SimInput] Adjusting life of P%d by %d\n", selectedPlayer, rotationDelta);
    //     gameState.adjustLife(selectedPlayer, rotationDelta);
    //     rotationDelta = 0;
    // }
}

// Returns the simulated rotation value and clears it.
int SimulationInputManager::getRotation() const {
    int delta = rotationDelta;
    // Serial.printf("[SimInput] getRotation: returning %d\n", delta);
    rotationDelta = 0;
    return delta;
}

// Returns and clears the simulated short/long press state.
bool SimulationInputManager::wasButtonShortPressed() const {
    bool result = shortPressDetected;
    shortPressDetected = false;
    return result;
}

// Returns and clears the simulated short/long press state.
bool SimulationInputManager::wasButtonLongPressed() const {
    bool result = longPressDetected;
    longPressDetected = false;
    return result;
}

// Increments the internal rotation delta and logs the change.
void SimulationInputManager::simulateRotation(int delta) {
    rotationDelta += delta;
    Serial.printf("[SimInput] simulateRotation called: delta=%d, new value=%d\n", delta, rotationDelta);
}

// Simulates a button press/release and sets short/long press flags based on press duration.
void SimulationInputManager::simulateButtonPress(bool held) {
    unsigned long now = millis();

    if (held && !buttonHeld) {
        buttonPressTime = now;
        buttonHeld = true;
    } else if (!held && buttonHeld) {
        unsigned long duration = now - buttonPressTime;
        if (duration >= 1000) longPressDetected = true;
        else shortPressDetected = true;
        buttonHeld = false;
    }
}