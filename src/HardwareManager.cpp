#include "HardwareManager.h"
#include "Config.h"
#include "NetworkManager.h"
#include "GameState.h"
#include "DeviceManager.h"

HardwareManager hardware;

HardwareManager::HardwareManager()
    : encoderButton(PIN_ENCODER_BUTTON), arcadeButton(PIN_ARCADE_BUTTON), encoder() {
    
    encoder.attachHalfQuad(PIN_ENCODER_DT, PIN_ENCODER_CLK);
    encoder.clearCount();

    oldPosition = 0;
    newPosition = 0;
    delta = 0;

    mode = MODE_LIFE;
    ledState = LOW;
}

void HardwareManager::begin() {
    encoder.setCount(0);
    oldPosition = encoder.getCount() / 2;

    encoderButton.setDebounceTime(50);
    arcadeButton.setDebounceTime(50);

    pinMode(PIN_ARCADE_LED, OUTPUT);
    digitalWrite(PIN_ARCADE_LED, LOW);
}

void HardwareManager::update() {
    extern GameState gameState;
    extern NetworkManager network;
    extern DeviceManager device;

    encoderButton.loop();
    arcadeButton.loop();

    handleEncoderButton();
    handleArcadeButton();

    delta = update_encoder();
    if (delta != 0) {
        uint8_t playerID = device.getPlayerID();

        if (mode == MODE_LIFE) {
            gameState.adjustLife(playerID, delta);
        } else if (mode == MODE_POISON) {
            gameState.adjustPoison(playerID, delta);
        }

        if (device.getRole() == ROLE_HOST) {
            network.sendGameState();
        } else if (device.getRole() == ROLE_CLIENT) {
            if (mode == MODE_LIFE) {
                network.sendLifeChangeRequest(playerID, gameState.getLife(playerID));
            } else {
                network.sendPoisonChangeRequest(playerID, gameState.getPlayerState(playerID).poison);
            }
        }
    }

    // LED state: ON if it's this player's turn
    ledState = (gameState.getCurrentTurnPlayer() == device.getPlayerID()) ? HIGH : LOW;
    digitalWrite(PIN_ARCADE_LED, ledState);
}

void HardwareManager::handleEncoderButton() {
    if (encoderButton.isPressed()) {
        encoderPressStart = millis();
        encoderLongPressDetected = false;
    }

    if (encoderButton.isPressed() && !encoderLongPressDetected &&
        (millis() - encoderPressStart > LONG_PRESS_THRESHOLD)) {
        encoderLongPressDetected = true;
        reset();  // Long press resets device state
    }

    if (encoderButton.isReleased() && !encoderLongPressDetected) {
        mode = (mode == MODE_LIFE) ? MODE_POISON : MODE_LIFE;
        display.refreshAll();  // trigger full screen refresh on mode change
    }

}

void HardwareManager::handleArcadeButton() {
    extern GameState gameState;
    extern NetworkManager network;
    extern DeviceManager device;

    if (arcadeButton.isPressed()) {
        arcadePressStart = millis();
        arcadeLongPressDetected = false;
    }

    if (arcadeButton.isPressed() && !arcadeLongPressDetected &&
        (millis() - arcadePressStart > LONG_PRESS_THRESHOLD)) {
        arcadeLongPressDetected = true;

        // Reset life/poison for all 4 players
        for (int i = 0; i < 4; i++) {
            int currentLife = gameState.getLife(i);
            gameState.adjustLife(i, 20 - currentLife);

            int currentPoison = gameState.getPlayerState(i).poison;
            gameState.adjustPoison(i, -currentPoison);
        }

        gameState.setTurn(0);
        network.sendGameState();
    }

    if (arcadeButton.isReleased() && !arcadeLongPressDetected) {
        gameState.nextTurn();
        if (device.getRole() == ROLE_HOST) {
            network.sendGameState();
        } else {
            network.sendTurnChange();
        }
    }
}

long HardwareManager::update_encoder() {
    newPosition = encoder.getCount() / 2;
    delta = newPosition - oldPosition;
    oldPosition = newPosition;
    return delta;
}

bool HardwareManager::isEncoderButtonReleased() {
    encoderButton.loop();
    return encoderButton.isReleased();
}

bool HardwareManager::isArcadeButtonReleased() {
    arcadeButton.loop();
    return arcadeButton.isReleased();
}

void HardwareManager::reset() {
    encoder.clearCount();
    oldPosition = 0;
    newPosition = 0;
    delta = 0;
    mode = MODE_LIFE;
    digitalWrite(PIN_ARCADE_LED, LOW);
}
