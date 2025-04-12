
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

```
LifeStream - Life Totals

Planeswalker 1: 20 <-
Planeswalker 2: 18
Planeswalker 3: 15
Planeswalker 4: 22

You are Planeswalker 2
```

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

MIT License

---

## Credits

Inspired by the Magic: The Gathering community for fostering innovation in physical game accessories.