
# LifeStream

## Multiplayer Life Counter for ESP32 with ePaper Display

LifeStream is a scalable, battery-friendly multiplayer life tracking system for tabletop games like Magic: The Gathering.

Built on the ESP32 platform using a Waveshare 2.9" V2 ePaper display, rotary encoder input, and ESP-NOW wireless networking.

---

## Project Goals

- Personal life counter for 4 players (Planeswalkers)
- Seamless wireless synchronization between devices
- Designed for battery-powered operation
- Modular, scalable architecture for future expansions
- Player-friendly, MTG-themed user experience

---

## System Overview

### Architecture Diagram

```plaintext
ESP32 LifeStream Devices x4
       (via ESP-NOW)
           |
    ┌───────────────┐
    │  Host Device  │ <-- First powered-on device
    └───────────────┘
        ↑   ↑   ↑
      Clients auto join
```

---

## Features Overview

| Feature                                      | Status  |
|---------------------------------------------|---------|
| Player Life Tracking (4 Players)            | ✓       |
| MTG Themed: Planeswalker Labels             | ✓       |
| Turn Indicator                              | ✓       |
| Dynamic Redraw Optimization                 | ✓       |
| Rotary Encoder Life Adjustment              | ✗       |
| Button Press to Cycle Turn                  | ✗       |
| ESP-NOW Host/Client Auto Detection          | ✗       |
| Multiplayer Life Sync over Wi-Fi            | ✗       |
| Battery Level Monitoring                    | ✗       |
| OTA Updates Support                        | ✗       |

---

## Simulation Mode

LifeStream supports a serial-based simulation mode for development without hardware.

### How to Enable

1. Build using the `sim` environment in `platformio.ini`:
   ```bash
   pio run -e sim -t upload
   ```

2. Connect to Serial Monitor (115200 baud) and use the following key commands:

### Serial Commands

| Key | Action                                 |
|-----|----------------------------------------|
| `1`–`4` | Select active player                |
| `a`   | Decrease selected player's life       |
| `d`   | Increase selected player's life       |
| `w`   | Increase poison counter               |
| `s`   | Decrease poison counter               |
| `t`   | Advance to the next player's turn     |
| `x`   | Eliminate selected player             |
| `v`   | Eliminate all others (simulate win)   |
| `r`   | Reset all players                     |
| `g`   | Redraw current game state             |

Simulation mode helps test game logic, UI redraws, and turn flow without requiring physical input devices.

### Example Simulation Log

```plaintext
[Main] No host detected, assuming host role
[DeviceManager] Host role assumed. Player ID set to 0.
[GameSetup] Default setup — Players: 4, Starting Life: 20
[Display] Rendering state for P1 — Life: 20, Poison: 0, Turn: 0
[SimInput] Received char: 0x74 (t)
[GameState] Turn passed from Player 0 to Player 1
[SimInput] Received char: 0x61 (a)
[GameState] Adjusting life for player 1 by -1. New life: 19
```

### Simulation Mode UI Layout

Example 4-player display layout:

```
┌────────────────────────────────────────────┐
│ P1: 20   PSN: 0     |     P2: 20   PSN: 0  │
│                                ●           │
│                                            │
│ P3: 20   PSN: 0     |     P4: 20   PSN: 0  │
│                                ○           │
└────────────────────────────────────────────┘
```

Legend:
- `●` = Active player's turn (black dot)
- `○` = Inactive turn marker

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
      ├── Handle Inputs (Encoder / Button)
      ├── Send Updates (Life/Turn Changes)
      ├── Receive Network Updates
      ├── Update Display Dynamically
      └── Sleep on Inactivity
```

---

## Current Display Layout Example

![lifestream](https://github.com/Phaenom/lifestream/blob/main/media/lifestream.png)

---

## Project Structure

| File/Folder      | Purpose                                 |
|-----------------|------------------------------------------|
| src/            | All source code for Arduino / ESP32     |
| docs/           | Wiring diagrams, visuals, assets        |
| platformio.ini  | PlatformIO configuration                |
| README.md       | This project overview                   |

---

## Hardware Requirements

- ESP32 Dev Module
- Waveshare 2.9" V2 ePaper Display
- Rotary Encoder (with optional button)
- LiPo Battery + TP4056 Charging Module
- Optional LEDs / Buzzer for feedback
- Optional On/Off Toggle Switch

---

## Setup Instructions

1. Clone this repository:
```bash
git clone https://github.com/yourusername/LifeStream.git
```

2. Open in PlatformIO or Arduino IDE.

3. Install Required Libraries:
- GxEPD2
- Adafruit GFX Library
- Adafruit BusIO

4. Compile & Upload to ESP32.

5. Power on multiple devices to test Host/Client behavior.

---

## Future Enhancements (Planned)

- Rotary Encoder Life Adjustment
- Button-driven Turn Cycling
- ESP-NOW Multiplayer Communication
- OTA Update Support
- Battery Level Indicator
- Partial ePaper Refresh
- Game Mode Variants (EDH, Commander, Win Counters)

---

## License

TBD

---

## Credits

Inspired by the Magic: The Gathering community for fostering innovation in physical game accessories.
