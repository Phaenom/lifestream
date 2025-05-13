// Main entry point for LifeStream, responsible for initializing system modules and running the main loop.

#include "Config.h"               // Configuration settings and mode flags
#include "IInputManager.h"        // Interface for input management

// Initialize using Hardware or Simulation, adjust flag in Config.h to swtich mode
#ifdef SIMULATION_MODE
  #include "testing/SimulationInputManager.h"  // Simulation input for testing
  SimulationInputManager simInput;
  IInputManager* input = &simInput;            // Pointer to input manager interface
#else
  #include "InputManager.h"                      // Hardware input manager
  InputManager hwInput;
  IInputManager* input = &hwInput;              // Pointer to input manager interface
#endif

DisplayManager display;        // Manages display output
BatteryManager battery;        // Handles battery monitoring
DeviceManager device;          // Manages device-specific functions
EEPROMManager eeprom;          // Manages EEPROM storage
NetworkManager network;        // Handles network connectivity
GameSetup gameSetup;           // Manages game setup procedures
GameState gameState;           // Maintains current game state

void setup() {
  Serial.begin(115200);

  input->begin();              // Initialize input system

  if (!network.hasHost()) {
    device.assumeHostRole();   // No host found, assume host role
    gameSetup.begin();         // Configure game settings (players, starting life)
  } else {
    device.assumePlayerRole(1); // Host found, assume player role and wait for parameters
    // Wait to receive game parameters from the host
  }

  delay(500);                  // Prepare game setup environment

  gameState.begin(device.getPlayerId(), gameSetup.getStartingLife());
  for (int id = 0; id < gameSetup.getPlayerCount(); ++id) {
    display.renderPlayerState(id, gameState.getPlayerState(id));
  }

  delay(500);                  // Initialize game state tracking
}

void loop() {
  input->update();             // Poll and update input state

  if (input->getRotation() != 0) {
    Serial.print("Rotated: ");
    Serial.println(input->getRotation());    // Log rotation input
  }

  if (input->wasButtonShortPressed()) {
    Serial.println("Short press detected");  // Handle short button press action
  }

  if (input->wasButtonLongPressed()) {
    Serial.println("Long press detected");
    if (gameState.getPlayerState(device.getPlayerId()).isTurn) {
      network.sendTurnAdvanceRequest(device.getPlayerId());
    }
  }

  delay(10);                   // Small delay to debounce inputs and reduce CPU usage
}