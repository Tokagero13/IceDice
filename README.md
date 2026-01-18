# 🎲 IceDice — ESP32 Dice Roller

IceDice is a firmware project for **ESP32** with a color TFT ST7735 display, a push button, and a buzzer. The device shows the result of rolling two dice, plays a short roll animation, starts a countdown timer, and triggers a visual and audible alert when the timer finishes.

The main application code lives in [`src/main.cpp`](src/main.cpp). All hardware pins and tunable parameters are collected in [`include/config.h`](include/config.h). A step‑by‑step quickstart guide is available in [`QUICKSTART.md`](QUICKSTART.md), and more detailed documentation is stored in the [`plans/`](plans/README.md) folder.

## ✨ Features

- Rendering of **two dice** with realistic pips and rounded corners.
- **Color‑coded sum** of the dice (different background colors for different totals).
- **Roll animation** with short tick sounds.
- **Countdown timer** (60 seconds by default) with color changes as the time runs out.
- **Alert mode** after the timer finishes: blinking triangle and periodic alarm sound.
- **Button behavior**:
  - short press in idle/after roll state — start a dice roll;
  - short press after the roll — start the countdown timer;
  - short press while the timer is running — interrupt the timer and start a new roll;
  - short press in alert mode — acknowledge the alert and start a new roll;
  - long press — software reboot of the ESP32.
- **Intro screen** with title and hint, accompanied by a one‑time **intro melody** at startup.

## 🧱 Hardware requirements

Minimum hardware setup:

| Component            | Description                                     |
|----------------------|-------------------------------------------------|
| Board                | ESP32 DevKit (compatible with `esp32dev` env)  |
| Display              | TFT ST7735, 1.44" 128×128 / 160×128            |
| Buzzer               | Passive/active buzzer on GPIO 13               |
| Button               | Momentary push button between GPIO 15 and GND  |
| Power                | 5V/USB for ESP32, display powered from 3.3V    |

The wiring diagram for the display matches the one described in [`QUICKSTART.md`](QUICKSTART.md) and [`plans/ARCHITECTURE.md`](plans/ARCHITECTURE.md).

## 🚀 Getting started

1. Install **Visual Studio Code** and the **PlatformIO** extension.
2. Open the `IceDice` folder as a PlatformIO project.
3. Make sure [`platformio.ini`](platformio.ini) uses the `esp32dev` environment and includes the Adafruit libraries.
4. Build the firmware:

   ```bash
   pio run
   ```

5. Upload the firmware to the ESP32 board (when you see `Connecting...` in the console, hold the **BOOT** button on the board):

   ```bash
   pio run --target upload
   ```

6. Open a serial monitor to see debug logs:

   ```bash
   pio device monitor --baud 115200
   ```

For more detailed instructions and common issues, see [`QUICKSTART.md`](QUICKSTART.md).

## 📁 Project structure

Key files and directories:

- [`src/main.cpp`](src/main.cpp) — main firmware file: state machine logic, dice rendering, timer, button handling, and sound.
- [`include/config.h`](include/config.h) — all configurable parameters: display/button/buzzer pins, colors, dice geometry, timer duration, animation and sound settings.
- [`platformio.ini`](platformio.ini) — PlatformIO configuration (board `esp32dev`, library dependencies).
- [`QUICKSTART.md`](QUICKSTART.md) — quickstart guide, wiring, and FAQ.
- Documentation folder [`plans/`](plans/README.md):
  - project overview — [`plans/README.md`](plans/README.md);
  - architecture — [`plans/ARCHITECTURE.md`](plans/ARCHITECTURE.md);
  - implementation plan — [`plans/IMPLEMENTATION_PLAN.md`](plans/IMPLEMENTATION_PLAN.md);
  - testing guide — [`plans/TESTING_GUIDE.md`](plans/TESTING_GUIDE.md);
  - button integration — [`plans/BUTTON_GUIDE.md`](plans/BUTTON_GUIDE.md);
  - troubleshooting — [`plans/TROUBLESHOOTING.md`](plans/TROUBLESHOOTING.md).

Additionally, the repository contains [`include/`](include/README), [`lib/`](lib/README), and [`test/`](test/README) directories with support and example files.

## 🧠 How it works (high‑level)

The high‑level behavior is implemented as a simple state machine in [`src/main.cpp`](src/main.cpp).

- On boot the function [`setup()`](src/main.cpp:525) is executed:
  - initializes the serial port, display, and random number generator;
  - configures button and buzzer pins;
  - shows the intro screen and starts the intro melody;
  - sets the initial state to "waiting for roll".
- In the main loop [`loop()`](src/main.cpp:558):
  - the button is debounced and its events are detected;
  - the current state is updated (roll animation, timer countdown, alert blinking);
  - when a short press is detected, the handler [`handleButtonPress()`](src/main.cpp:457) is called and either starts a new roll, starts the timer, or interrupts the timer/alert and rolls again depending on the current state.

All numeric parameters (pins, timer duration, animation speed, sound frequencies and durations, etc.) are kept in a single configuration file [`include/config.h`](include/config.h), which makes it easy to adapt the project to a different display or board.

## 🔧 Adapting to your hardware

Typical changes you might want to make in [`include/config.h`](include/config.h):

- adjust display, button, and buzzer pins in [`Config::Hardware`](include/config.h:11);
- change dice size and positions in [`Config::Dice`](include/config.h:63);
- update timer duration and text size in [`Config::Timer`](include/config.h:125);
- tweak alert blink interval and colors in [`Config::Alert`](include/config.h:136) and [`Config::Colors`](include/config.h:34);
- modify roll animation length and frame delay in [`Config::Animation`](include/config.h:146);
- tune melodies, tempo, and sound parameters in [`Config::Sound`](include/config.h:180).

After changing the configuration, rebuild and re‑upload the firmware to the ESP32.

## 🐛 Debugging and common issues

If the display stays black, the image is shifted/rotated, or the firmware fails to upload:

- check the "Common problems" section in [`QUICKSTART.md`](QUICKSTART.md);
- follow the detailed troubleshooting guide in [`plans/TROUBLESHOOTING.md`](plans/TROUBLESHOOTING.md);
- verify that [`platformio.ini`](platformio.ini) matches your specific ESP32 board.

IceDice is intended as a starting point for experimenting with animations, sound effects, button‑driven interaction, and more advanced dice‑game logic.
