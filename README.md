
# LifeStream

## Multiplayer Life Counter for ESP32 with ePaper Display

LifeStream is a scalable, battery-friendly multiplayer life tracking system for tabletop games like Magic: The Gathering.

Built on the ESP32 platform using a Waveshare 2.9" V2 ePaper display, rotary encoder input (optional), and ESP-NOW wireless networking, LifeStream enables up to 4 players to synchronize life and poison totals in real-time. One device automatically becomes the host; others join as clients. The system is fully usable in both hardware and simulation modes.

---

## Project Goals

- Personal life counter for 4 players (Planeswalkers)
- Seamless wireless synchronization between devices
- Designed for battery-powered operation
- Modular, scalable architecture for future expansions
- Player-friendly, MTG-themed user experience with simulation support

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

> Note: In simulation mode, the system runs without ESP-NOW networking and uses serial commands to simulate input and turn flow.

---

## Features Overview

## Features Overview

## Features Overview

| Feature                                      | Status  |
|---------------------------------------------|---------|
| 🎮 4-Player Life Tracking                    | ✅       |
| ☠️ Poison Counter Support                    | ✅       |
| 🔁 Client-to-Host Sync (Life + Poison)       | ✅       |
| 🧠 Host Broadcast of Full Game State         | ✅       |
| 📡 ESP-NOW Role Detection & Arbitration      | ✅       |
| 🔃 Turn Indicator & Sync                    | ✅       |
| 🖥️ Dynamic Screen Redraw                     | ✅       |
| 🎨 Role Display + Turn Highlight             | ✅       |
| 💬 Serial Simulation Input (Life, Poison)    | ✅       |
| 🧪 Local Input Simulation (SimInputManager)  | ✅       |
| 🧱 Clamped Drawing Bounds                    | ✅       |
| 🔘 Rotary Encoder Support                    | 🚧       |
| ⏺️ Button Turn Cycling                       | 🚧       |
| 🔋 Battery Monitoring                        | ❌       |

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
└── GAME_LOOP (All Devices or Sim Mode)
      ├── Handle Inputs (Encoder / Button or Serial)
      ├── Send Updates (Life/Turn Changes)
      ├── Receive Network Updates (ESP-NOW only)
      ├── Update Display Dynamically
      └── Sleep on Inactivity (optional)
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

2. Open in PlatformIO (recommended) or Arduino IDE.

3. Install Required Libraries:
- GxEPD2
- Adafruit GFX Library
- Adafruit BusIO

4. Compile & Upload to ESP32.

5. Power on multiple devices to test Host/Client behavior.

- Alternatively, run in `sim` mode using:
  ```bash
  pio run -e sim -t upload
  ```

---

## Future Enhancements (Planned)

- Rotary Encoder Integration (life adjust + UI navigation)
- Button Input for Turn Advancement
- Poison-Based Elimination at ≥10
- Player Elimination & Overlay Display
- Dynamic Player ID Assignment & Join Flow
- Battery Level Monitoring & Icon Overlay
- OTA Updates & Versioning
- Game Mode Variants (Commander, EDH, Win Counter)
- UI Polishing for Startup, Elimination, and Victory States

---

## License

MIT License

---

## Credits

Inspired by the Magic: The Gathering community and makers passionate about elevating analog play with digital tools.
