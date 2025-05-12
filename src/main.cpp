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

  delay(500);                  // Allow time for display hardware to initialize
  display.begin();             // Initialize display module

  delay(500);                  // Allow battery sensor to stabilize
  battery.begin();             // Initialize battery monitoring

  delay(500);                  // Prepare device-specific components
  device.begin();              // Initialize device manager

  delay(500);                  // Ensure EEPROM is ready for access
  eeprom.begin();              // Initialize EEPROM manager

  delay(500);                  // Setup network hardware and connections
  network.begin();             // Initialize network manager

  delay(500);                  // Prepare game setup environment
  gameSetup.begin();           // Initialize game setup module

  delay(500);                  // Initialize game state tracking
  gameState.begin();           // Initialize game state manager
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
    Serial.println("Long press detected");   // Handle long button press action
  }

  delay(10);                   // Small delay to debounce inputs and reduce CPU usage
}