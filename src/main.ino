#include "BatteryManager.h"
#include "DeviceManager.h"
#include "DisplayManager.h"
#include "EEPROMManager.h"
#include "GameSetup.h"
#include "GameState.h"
#include "InputManager.h"
#include "NetworkManager.h"
#include "Utils.h"

#include <Arduino.h>

InputManager input;
BatteryManager battery;
DeviceManager device;
DisplayManager display;
EEPROMManager eeprom;
GameSetup gSetup;
GameState gState;
NetworkManager network;

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println("\n");

  battery.begin();
    delay(1000);

  device.begin();
    delay(1000);

  display.begin();
    delay(1000);

  eeprom.begin();
    delay(1000);

  gSetup.begin();
    delay(1000);

  gState.begin();
    delay(1000);

  input.begin();
    delay(1000);

  network.begin();
    delay(1000);

}

void loop() {
  delay(1000);
  input.update();

  int rotation = input.getRotation();
  if (rotation != 0) {
    Serial.print("Rotated: ");
    Serial.println(rotation);
  }

  if (input.wasButtonPressed()) {
    Serial.println("Button Pressed!");
  }

  delay(100);
}