// Main entry point for LifeStream, responsible for initializing system modules and running the main loop.
//#include "Config.h"
#include "DeviceManager.h"
#include "NetworkManager.h"
#include "DisplayManager.h"
#include "GameSetup.h"
#include "GameState.h"
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

int playerLives[4];  // Global or static life total array

void setup() {
  unsigned long delayTime = random(1000, 3000); // Random delay to stagger device startup
  delay(delayTime);

  LOG_BEGIN(115200);
  Serial.println("\n[Main] Setup starting...");

  // --- Initialize Display ---
  display.begin();
  Serial.println("[Main] Display initialized");

  // --- Initialize Network ---
  network.begin();
  Serial.println("[Main] Network initialized");

  network.markReady();  // Notify that network can apply incoming game state
  network.updateRole(); // Determine and assign device role (host/client)
  Serial.printf("[Main] Role Assigned: %s\n", DeviceManager::roleToString(device.getRole()));

  // --- Initialize Input ---
  input->begin();
  Serial.println("[Main] Input system initialized");

  // --- Game Setup ---
  gameSetup.begin();        
  Serial.println("[Main] Game setup environment prepared");

  network.sendGameState();  // If host, broadcast game setup to all devices
  if (device.getRole() == ROLE_CLIENT) {
    Serial.println("[Main] Setup received from host");
  }

  // --- Initialize Game State ---
  gameState.begin(device.getPlayerID(), gameSetup.getStartingLife());

  // --- Initial Render of All Players ---
  for (int id = 0; id < gameSetup.getPlayerCount(); ++id) {
    display.renderPlayerState(id, gameState.getPlayerState(id));
  }

  EPD_2IN9_V2_Display((UBYTE*)display.getDisplayBuffer());

}

void loop() {
  network.updateRole();        // Continue checking for valid host/client role

  input->update();             // Poll input from encoder and button
  network.heartbeat();         // If host, broadcast heartbeat
  network.applyPendingGameState(); // Sync any incoming game state updates

  delay(10); // Debounce + lower CPU usage
}