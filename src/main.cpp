// Main entry point for LifeStream, responsible for initializing system modules and running the main loop.
#include "Config.h"

#ifdef SIMULATION_MODE
//#warning SIMULATION_MODE is enabled
#include "testing/SimulationInputManager.h"
SimulationInputManager simInput;
IInputManager* input = &simInput;
#else
#include "InputManager.h"
InputManager hwInput;
IInputManager* input = &hwInput;
#endif

void setup() {
  delay(5000);  // Allow time for serial monitor to connect
  Serial.begin(115200);
  Serial.println("\n[Main] Setup starting...");

  network.begin();          // Initialize network system
  Serial.println("\n[Main] Network initialized");

  display.begin();          // Initialize e-paper display
  Serial.println("\n[Main] Display initialized");

  input->begin();           // Initialize input system
  Serial.println("\n[Main] Input system initialized");

  network.updateRole();     // Check and update device role  
  Serial.printf("\n[Main] Role Assigned: %s\n", NetworkManager::roleToString(network.getRole()));

  gameSetup.begin();        // Initialize game setup  
  Serial.println("\n[Main] Game setup environment prepared");

/*   network.sendGameState();  // Send game state to all devices
  if (network.getRole() == ROLE_CLIENT) {
      Serial.println("\n[Main] Setup received from host");
  } */

/*   gameState.begin(device.getPlayerId(), gameSetup.getStartingLife());
  Serial.printf("[Main] Game state initialized for player %d with life %d\n", device.getPlayerId(), gameSetup.getStartingLife());

  for (int id = 0; id < gameSetup.getPlayerCount(); ++id) {
    display.renderPlayerState(id, gameState.getPlayerState(id));
    Serial.printf("[Render] Drawing player %d\n", id);
  } */

  EPD_2IN9_V2_Display(display.frameBuffer);
}

void loop() {
  // Heartbeat - Used for debugging to ensure loop is active
  static unsigned long lastHeartbeat = 0;
    if (millis() - lastHeartbeat > 5000) {
    Serial.println("");
      Serial.printf("[Loop] Heartbeat - Player ID: %d, Role: %s\n",
              device.getPlayerId(),
              NetworkManager::roleToString(network.getRole()));
    // Serial.printf("[Loop] Heartbeat - Player ID: %d, Is Host: %s\n", device.getPlayerId(), network.getRole());
    Serial.println("");
    lastHeartbeat = millis();
    }

  //network.update();
  //input->update();             // Poll and update input state

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