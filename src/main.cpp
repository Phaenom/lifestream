#ifdef SIMULATION_MODE
#warning SIMULATION_MODE is enabled
#endif

// Main entry point for LifeStream, responsible for initializing system modules and running the main loop.
#include "Config.h"
#include "IInputManager.h"        // Interface for input management

#ifdef SIMULATION_MODE
#include "testing/SimulationInputManager.h"
SimulationInputManager simInput;
IInputManager* input = &simInput;
#else
#include "InputManager.h"
InputManager hwInput;
IInputManager* input = &hwInput;
#endif

DisplayManager display;        // Manages display output
BatteryManager battery;        // Handles battery monitoring
DeviceManager device;          // Manages device-specific functions
EEPROMManager eeprom;          // Manages EEPROM storage
NetworkManager network;        // Handles network connectivity
GameSetup gameSetup;           // Manages game setup procedures
GameState gameState;           // Maintains current gameState state

void setup() {
  Serial.println("[Main] Setup starting...");
  Serial.begin(115200);
  display.begin();  // Initialize e-paper display
  Serial.println("[Main] Display initialized");
  input->begin();              // Initialize input system
  Serial.println("[Main] Input system initialized");

  if (!network.hasHost()) {
    Serial.println("[Main] No host detected, assuming host role");
    device.assumeHostRole();   // No host found, assume host role
    gameSetup.begin();         // Configure game settings (players, starting life)
  } else {
    Serial.println("[Main] Host detected, assuming player role");
    device.assumePlayerRole(1); // Host found, assume player role and wait for parameters
    // Wait to receive game parameters from the host
  }

  delay(500);                  // Prepare game setup environment
  Serial.println("[Main] Game setup environment prepared");

  if (device.isHost()) {
    gameState.begin(device.getPlayerId(), gameSetup.getStartingLife());
    Serial.printf("[Main] Game state initialized for player %d with life %d\n", device.getPlayerId(), gameSetup.getStartingLife());
    for (int id = 0; id < gameSetup.getPlayerCount(); ++id) {
      display.renderPlayerState(id, gameState.getPlayerState(id));
      Serial.printf("[Render] Drawing player %d\n", id);
    }
    EPD_2IN9_V2_Display(display.frameBuffer);
  } else {
    // Wait for host to send game parameters
    while (!network.hasReceivedGameParams()) {
      delay(100);
    }
    gameState.begin(device.getPlayerId(), gameSetup.getStartingLife());
    Serial.printf("[Main] Game state initialized for player %d with life %d\n", device.getPlayerId(), gameSetup.getStartingLife());
    for (int id = 0; id < gameSetup.getPlayerCount(); ++id) {
      display.renderPlayerState(id, gameState.getPlayerState(id));
      Serial.printf("[Render] Drawing player %d\n", id);
    }
    EPD_2IN9_V2_Display(display.frameBuffer);
  }
}

void loop() {
  // Heartbeat - Used for debugging to ensure loop is active
  static unsigned long lastHeartbeat = 0;
    if (millis() - lastHeartbeat > 3000) {
    Serial.printf("[Loop] Heartbeat - Player ID: %d, Is Host: %d\n", device.getPlayerId(), device.isHost());
    lastHeartbeat = millis();
    }

  input->update();             // Poll and update input state

  if (input->getRotation() != 0) {
    Serial.print("Rotated: ");
    Serial.println(input->getRotation());    // Log rotation input
    Serial.print("[Loop] getRotation: ");
    Serial.println(input->getRotation());
  }

  if (input->wasButtonShortPressed()) {
    Serial.println("Short press detected");  // Handle short button press action
    Serial.printf("[Input] Rotation delta: %d\n", input->getRotation());
  }

  if (input->wasButtonLongPressed()) {
    Serial.println("Long press detected");
    Serial.printf("[Input] Rotation delta: %d\n", input->getRotation());
    if (gameState.getPlayerState(device.getPlayerId()).isTurn) {
      network.sendTurnAdvanceRequest(device.getPlayerId());
    }
  }

  delay(10);                   // Small delay to debounce inputs and reduce CPU usage
}