# LIFESTREAM

# ESP32 Multiplayer Life Counter System

A battery-powered multiplayer life counter for tabletop games using ESP32, ePaper display, Rotary Encoder, and ESP-NOW wireless networking.

---

## System Overview

### Main Features
- Auto Host/Client detection over ESP-NOW
- Configurable number of players and starting life total
- Dynamic player ID assignment by Host
- Rotary encoder to adjust life total
- Button to toggle turn ownership
- Low-power friendly (Battery powered)
- Real-time sync of life totals and turn state between all players

---

### System States & Flow

```plaintext
BOOT
│
├── SEARCH_FOR_HOST
│     ├── Host Found → JOIN_AS_CLIENT
│     └── No Host Found → BECOME_HOST
│
├── HOST_CONFIG_MENU (Host Only)
│     ├── Select Player Count
│     ├── Select Starting Life Total
│     └── BROADCAST_GAME_CONFIG
│
├── JOIN_AS_CLIENT (Client Only)
│     ├── Send Join Request to Host
│     └── Wait for Player ID Assignment
│
└── GAME_LOOP (All Devices)
      ├── Handle Encoder Inputs
      ├── Handle Button Press
      ├── Send Updates (Life or Turn Change)
      ├── Receive Network Updates
      ├── Update Local Game State
      └── Redraw Display

---
```
### Module Responsibilities

### setup()
- Initialize Serial, GPIO, Encoder, Button
- Initialize ePaper Display
- Initialize ESP-NOW
- Load Saved Settings (Optional)
- Enter SEARCH_FOR_HOST state

---

### SEARCH_FOR_HOST
- Listen for `HOST_BROADCAST` for 3-5 seconds
- If found → Store Host Info → Go to JOIN_AS_CLIENT
- If not found → Become Host → Go to HOST_CONFIG_MENU

---

### HOST_CONFIG_MENU
- Prompt user:
  - Select Number of Players (2-4)
  - Select Starting Life Total (20 or 40)
- On Confirm:
  - Initialize Player States
  - Start Broadcasting `GAME_CONFIG`
  - Listen for `JOIN_REQUEST` messages

---

### JOIN_AS_CLIENT
- Send `JOIN_REQUEST` to Host
- Wait for `ASSIGN_PLAYER_ID` message
- Store Assigned Player ID
- Store Game Config
- Enter GAME_LOOP

---

## GAME_LOOP
For All Devices:

1. Handle Encoder:
   - Adjust Life Total
   - Clamp between 0-99
   - Send `LIFE_UPDATE` if changed

2. Handle Button Press:
   - Toggle Turn Ownership
   - Send `TURN_UPDATE` if changed

3. Handle Incoming Network Messages:
   - `HOST_BROADCAST` → Update Game Config
   - `JOIN_REQUEST` (Host only) → Assign Player ID
   - `ASSIGN_PLAYER_ID` → Store Assigned ID
   - `GAME_CONFIG` → Store Game Settings
   - `LIFE_UPDATE` → Update Player's Life
   - `TURN_UPDATE` → Update Player's Turn Status

4. Update ePaper Display:
   - Show All Players' Life Totals
   - Highlight Current Turn
   - Emphasize Local Player's Life Total

5. Power Management (Optional):
   - Enter Sleep Mode on Inactivity
   - Wake on Encoder or Button Input

---

## Networking Message Types

| Message Type         | Sent By   | Data Included                          |
|---------------------|------------|----------------------------------------|
| HOST_BROADCAST      | Host       | Host MAC, Game Config                 |
| JOIN_REQUEST        | Client     | Temp MAC                              |
| ASSIGN_PLAYER_ID    | Host       | Assigned Player ID                   |
| GAME_CONFIG         | Host       | Number of Players, Starting Life     |
| LIFE_UPDATE         | Any        | Player ID, New Life Total            |
| TURN_UPDATE         | Any        | Player ID, Turn Toggle State         |

---

## Project Structure (Files)

| File          | Purpose                                   |
|---------------|-------------------------------------------|
| main.ino      | Setup, State Machine, Main Loop          |
| networking.h/.cpp | ESP-NOW Communication Logic         |
| ui.h/.cpp     | Display Rendering, Menu Handling         |
| input.h/.cpp  | Encoder and Button Input Handling        |
| power.h/.cpp  | Battery, Sleep, Power State Management   |
| config.h      | Constants, Pin Definitions               |

---

## Future Enhancements

- Battery Level Indicator
- Game History Logging (Optional)
- Multiplayer Game Variants (EDH Commander Life / Poison / Win Counters)
- WiFi/Web Dashboard Support
