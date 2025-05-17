
#include <Arduino.h>
#include "src/NetworkManager.h"
#include "src/OutputManager.h"
#include "src/GameState.h"
#include "src/Config.h"
#include "src/assets/mtg_logo_symbol.h"
#include "src/DisplayManager.h"
#include "src/InputManager.h"  // Placeholder for future input handling

NetworkArbiter network;
DisplayManager display;
//could merge into a single hardware manager ? iunno they let you type anything here
InputManager input;  // Future use for button/encoder
OutputManager output;  // Responsible for all LED and buzzer output

// experimenting with decoupling logic loop and screen refresh
int temp = 0;
unsigned long previousMillis = 0;  
unsigned long interval = 1000;  // ms    


// Arduino setup() runs once when the device starts
// Life totals for all players
int life1 = 20, life2 = 20, life3 = 20, life4 = 20;
int poison1 = 0, poison2 = 0, poison3 = 0, poison4 = 0;
int currentTurnPlayerID = 0;
int myPlayerID = 0;

// Track last known states for detecting changes
DeviceRole lastRole = ROLE_UNDEFINED;
uint8_t lastPlayerID = 255;
int lastLife1 = -1, lastLife2 = -1, lastLife3 = -1, lastLife4 = -1;
int lastPoison1 = -1, lastPoison2 = -1, lastPoison3 = -1, lastPoison4 = -1;
int lastTurnPlayerID = -1;

//String mode = "life"; // life, poison, 
int myLife = 20;
int myPoison = 0;

String mode = "poison"; // life, poison, tech specs

void setup() {
    Serial.begin(115200);

    network.begin();
    display.begin();

//    if ( needsRedraw && (currentTurnPlayerID == game.myPlayerID) ) {
//        output.update(HIGH);  // Turn on LED or buzzer for your turn

        //output.begin(); // Initialize output hardware (LEDs, Buzzer, etc)
//    }

    previousMillis = millis(); // Update the last time the display was updated

    input.begin();   // buttons, encoders, etc
    output.begin();  // buzzers, LEDs, etc
    Serial.println("Setup complete");
}

void loop() {
    // Detect any state change
    bool changed = false;

    // Always update network logic first
    network.update();

    // Handle input (if implemented later)
   // input.update(); // TODO: merge below into input manager

    // For now, directly mirror network state to display variables
    life1 = network.lifeTotals[0];
    life2 = network.lifeTotals[1];
    life3 = network.lifeTotals[2];
    life4 = network.lifeTotals[3];
    currentTurnPlayerID = network.currentTurn;
    myPlayerID = network.getPlayerID();

    myLife += input.update_encoder(); // Update player X's life based on encoder input
    network.sendLifeUpdate(myLife);
    if (myLife != network.lifeTotals[myPlayerID]) {
        changed = true; // Set changed to true to trigger display updat
    }

    // check if TURN END BUTTON is pressed
    // TEMP CODE REPLACE WITH LOGIC THAT IGNORES IF NOT YOUR TURN

    // technically this send LED OFF signal every loop- better way?
    if (currentTurnPlayerID != myPlayerID) {
        output.update(LOW);  // Turn off LED or buzzer
    }

    if (currentTurnPlayerID == myPlayerID) {
        output.update(HIGH);  // Turn on LED or buzzer for your turn
    }

    // check if it is your turn:  button press -> turn on/off LEDs & send turn change
    if (input.get_arcade_button() ){

        //debug
        Serial.println("arcade button pressed"); 
        Serial.println(currentTurnPlayerID);
        Serial.println(myPlayerID);
        //debug

     //   if (myPlayerID == currentTurnPlayerID){     // CHANGE function name update button is confusing
            //if ( needsRedraw && (currentTurnPlayerID == game.myPlayerID) ) {
            //debug
            Serial.println("turn change sent?"); 
            Serial.println(network.currentTurn);
            //debug

            changed = true; // Set changed to true to trigger display update
      //  }
    }

    if (input.get_encoder_button() ) {
        //debug
        Serial.println("encoder button pressed"); 
        //debug

        // alternate between life and poison mode
        if (mode == "life") {
            mode = "poison";
        } else if (mode == "poison") {
            mode = "life";
        }

        changed = true; // Set changed to true to trigger display update
    }

    

    if (network.getRole() != lastRole || myPlayerID != lastPlayerID) changed = true;
    if (life1 != lastLife1 || life2 != lastLife2 || life3 != lastLife3 || life4 != lastLife4) changed = true;
    if (currentTurnPlayerID != lastTurnPlayerID) changed = true;

    // Update display only if something changed
    if (changed) {
        if (mode == "life") display.drawLifeCounter(life1, life2, life3, life4, currentTurnPlayerID, myPlayerID, network.getRole());
        if (mode == "poison") display.drawPoisonCounter(poison1, poison2, poison3, poison4, currentTurnPlayerID, myPlayerID, network.getRole());

        lastRole = network.getRole();
        lastPlayerID = myPlayerID;
        lastLife1 = life1;
        lastLife2 = life2;
        lastLife3 = life3;
        lastLife4 = life4;
        lastTurnPlayerID = currentTurnPlayerID;
    }
    
     //   if ( (needsRedraw) && (millis() - previousMillis > interval) ) { 
    // see if makes sense to  add in a refresh lock for screen
    // want to decouple input frequency from screen refresh rate
    //  update the display if life totals or turn ownership changed
    // screen refresh rate locked to X ms
    //previousMillis = millis(); // Update the last time the display was updated

    delay(100); // Small delay for ESP-NOW stability and display efficiency
}