#ifdef SIMULATION_MODE
#warning SIMULATION_MODE is enabled
#endif

// Main entry point for LifeStream, responsible for initializing system modules and running the main loop.
#include "Config.h"

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
//BatteryManager battery;        // Handles battery monitoring
DeviceManager device;          // Manages device-specific functions
//EEPROMManager eeprom;          // Manages EEPROM storage
//NetworkManager network;        // Handles network connectivity
GameSetup gameSetup;           // Manages game setup procedures
GameState gameState;           // Maintains current gameState state

void setup() {
  delay(2000);
  Serial.begin(115200);
  Serial.println("\n[Main] Setup starting...\n");

  network.begin();    // Initialize network system
  Serial.println("\n[Main] Network initialized");

  display.begin();    // Initialize e-paper display
  Serial.println("\n[Main] Display initialized");

  input->begin();     // Initialize input system
  Serial.println("\n[Main] Input system initialized");

  // Assume host until setup packet is received
  //device.assumeHostRole();

  gameSetup.begin();

  network.sendGameState();

  delay(500);                  // Prepare game setup environment
  Serial.println("[Main] Game setup environment prepared");

  Serial.println("[Main] Setup received from host");
  gameState.begin(device.getPlayerId(), gameSetup.getStartingLife());
  Serial.printf("[Main] Game state initialized for player %d with life %d\n", device.getPlayerId(), gameSetup.getStartingLife());

  for (int id = 0; id < gameSetup.getPlayerCount(); ++id) {
    display.renderPlayerState(id, gameState.getPlayerState(id));
    Serial.printf("[Render] Drawing player %d\n", id);
  }

  EPD_2IN9_V2_Display(display.frameBuffer);
}

void loop() {
  // Heartbeat - Used for debugging to ensure loop is active
  static unsigned long lastHeartbeat = 0;
    if (millis() - lastHeartbeat > 30000) {
    Serial.printf("[Loop] Heartbeat - Player ID: %d, Is Host: %s\n", device.getPlayerId(), network.getRole());
    lastHeartbeat = millis();
    }

  network.update();
  input->update();             // Poll and update input state

  // if (input->getRotation() != 0) {
  //   Serial.print("Rotated: ");
  //   Serial.println(input->getRotation());    // Log rotation input
  //   Serial.print("[Loop] getRotation: ");
  //   Serial.println(input->getRotation());
  // }

  // if (input->wasButtonShortPressed()) {
  //   Serial.println("Short press detected");  // Handle short button press action
  //   Serial.printf("[Input] Rotation delta: %d\n", input->getRotation());
  // }

  // if (input->wasButtonLongPressed()) {
  //   Serial.println("Long press detected");
  //   Serial.printf("[Input] Rotation delta: %d\n", input->getRotation());
  //   if (gameState.getPlayerState(device.getPlayerId()).isTurn) {
  //     network.sendTurnAdvanceRequest(device.getPlayerId());
  //   }
  // }

  delay(10);                   // Small delay to debounce inputs and reduce CPU usage
}