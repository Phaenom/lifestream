# lifestream

# ESP32 Multiplayer Life Counter System — Pseudocode Design

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

## System States & Flow

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
