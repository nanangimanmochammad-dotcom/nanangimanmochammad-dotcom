# STM32 Integrated UI System & Mini-Game 🍄

> A comprehensive embedded User Interface (UI) system developed for the **MES204** course. It features a multi-state menu architecture, real-time system monitoring, and a custom **Super Mario Parkour** mini-game.

![Status](https://img.shields.io/badge/status-active-success.svg)
![Platform](https://img.shields.io/badge/platform-STM32-blue.svg)
![Hardware](https://img.shields.io/badge/hardware-Nucleo--F446RE-green.svg)
![Display](https://img.shields.io/badge/display-LCD1602-orange.svg)


## 📺 Demo Preview

<table width="100%">
  <tr>
    <td width="50%" align="center">
      <b>1. 🌊 Splash & Boot</b>
      <br>
      <img src="https://github.com/user-attachments/assets/2011dcb1-e0f7-4c20-a48d-270dcc063f72" width="100%" />
    </td>
    <td width="50%" align="center">
      <b>2. ℹ️ System Monitor</b>
      <br>
      <img src="https://github.com/user-attachments/assets/34d2df2b-556f-4518-bed3-c3daa9baf734" width="100%" />
    </td>
  </tr>

  <tr>
    <td align="center">
      <b>3. ☮️ Symbol & Text Art</b>
      <br>
      <img src="https://github.com/user-attachments/assets/859c5e3d-53dd-4349-946e-e5376598bfdb" width="100%" />
    </td>
    <td align="center">
      <b>4. 🎨 Animation Demo</b>
      <br>
      <img src="https://github.com/user-attachments/assets/85c48f72-0279-42ab-b4c3-8d0945018c59" />
    </td>
  </tr>

  <tr>
    <td colspan="2" align="center">
      <b>5. 🎮 Feature Spotlight: Super Mario Parkour</b>
      <br>
      <img src="https://github.com/user-attachments/assets/bc0d003d-a137-4495-92cb-18f5f766787a" />
    </td>
  </tr>
</table>


## 📖 Project Overview

This project implements a complete **Finite State Machine (FSM)** on the STM32F446RE microcontroller. The goal was to design a responsive, non-blocking user interface that manages multiple functional modes on a limited 16x2 character display.

While the core functionality includes system monitoring and visual demonstrations, the highlight of the project is a fully playable platformer game that pushes the hardware's rendering capabilities.

## ✨ System Features

The system architecture is designed around **five distinct functional modes**, which can be cycled through using the User Button:

### 1. 🌊 Splash & Boot Sequence
* **Startup Animation**: A smooth scrolling "Welcome" interface that triggers upon system power-up.
* **Initialization**: Automatically verifies hardware integrity before entering the main loop.

### 2. ℹ️ System Monitor (Info Mode)
* **Real-Time Status**: Displays current system metrics, including RTC time or system ticks.
* **Identity Display**: Shows the Student ID and Project Name on a static layout.

### 3. ☮️ Symbol & Text Showcase
* **Custom Patterns**: A dedicated mode displaying special graphical icons (e.g., "Peace", "Love") created using custom character maps.
* **Text Integration**: Combines these graphical icons with corresponding text strings to demonstrate the LCD's mixed-mode rendering capability.

### 4. 🎨 Animation Demo
* **Large-Scale Sprite**: Demonstrates a custom rendering engine that combines multiple character blocks to create large moving sprites (e.g., a 3x2 grid-sized character) moving across the screen.

### 5. 🎮 Feature Spotlight: Super Mario Parkour
A fully playable mini-game that pushes the hardware limits:
* **Physics Engine**: Simulates gravity and jumping velocity.
* **Smart Rendering**: Uses **CGRAM** to render the Mario character and obstacles dynamically.
* **Gameplay**: Features real-time collision detection, procedural obstacle generation, and score tracking.

## 📂 Project Structure

This project follows the standard STM32CubeIDE directory structure, with application logic strictly separated from hardware drivers.

```text
MES204_TC/
├── Core/
│   ├── Inc/                    # Header Files (.h)
│   │   ├── main.h              # Global configuration & prototypes
│   │   ├── dino_game.h         # Game physics, sprite assets, & logic
│   │   ├── lcd_driver.h        # Low-level LCD1602 command definitions
│   │   ├── ui_manager.h        # FSM State definitions & Menu prototypes
│   │   └── stm32f4xx_it.h      # Interrupt Service Routines (ISR) headers
│   │
│   └── Src/                    # Source Code (.c)
│       ├── main.c              # Entry point, Hardware Init, & Main Loop
│       ├── dino_game.c         # Implementation of game mechanics & collision
│       ├── lcd_driver.c        # LCD initialization & custom char loading
│       ├── ui_manager.c        # UI State Machine & Screen navigation logic
│       ├── stm32f4xx_it.c      # Interrupt Handlers (Button PC13, Timers)
│       └── system_stm32f4xx.c  # System Clock Configuration
│
├── Drivers/                    # STM32 HAL (Hardware Abstraction Layer)
├── MES204_TC.ioc               # STM32CubeMX Hardware Configuration File
├── MES204_TC.launch            # Debugger Launch Configuration
└── README.md                   # Project Documentation
```

## ⚙️ How It Works (FSM Architecture)

The system operates in a continuous loop, checking the current `State` variable. This allows the MCU to handle multiple tasks seemingly at the same time (Time-multiplexing).

1.  **Boot Phase**:
    * Initializes HAL Drivers (GPIO, UART, I2C).
    * Initializes LCD1602 in 4-bit mode.
    * Loads Custom Characters (Dino/Mario sprites) into CGRAM.

2.  **Main Loop (Infinite)**:
    * **Input Handling**: Polls the Blue User Button with software debouncing.
    * **State Machine Switch**:
        * `STATE_INFO`: Updates RTC time and system status strings.
        * `STATE_ART`: Renders the "Peace & Love" symbol and text showcase.
        * `STATE_ANIMATION`: Cycles through large sprite frames.
        * `STATE_GAME`: Executes the `dino_game_update()` function (physics & rendering).
    * **Rendering**: Refreshes the LCD buffer based on the active state.

3.  **Interrupts**:
    * Button presses trigger state transitions (e.g., jumping from *Info* -> *Art* -> *Animation*...).
      
## 🚀 Quick Start

1.  **Import**:
    * Open **STM32CubeIDE**.
    * Go to `File` -> `Import` -> `Existing Projects into Workspace`.
2.  **Select**:
    * Browse and select the `MES204_TC` root folder.
3.  **Build**:
    * Click the **Hammer icon 🔨** (Build Debug).
    * *Note: If you see errors about missing `main.h`, open `MES204_TC.ioc` and click "Generate Code".*
4.  **Run**:
    * Connect your Nucleo-F446RE board via USB.
    * Click the **Green Bug icon 🐞** (Debug) to flash and run.

## 🎮 Controls & Interaction

The system features a **context-sensitive** input scheme. The Blue User Button (B1) performs different actions depending on the current screen.

### 1. Menu Navigation 
When browsing the system (Non-Game Modes), pressing the **Blue Button** cycles through the screens in this specific order:

> **[ ℹ️ System Info ]** ➡️ **[ ☮️ Symbol & Text Art ]** ➡️ **[ 🎨 Animation ]** ➡️ **[ 🎮 Game Start ]** ➡️ *(Loop back to Info)*

### 2. Gameplay Mode
Once you enter the **Game Mode**:
* **Action**: Press the **Blue Button** to **JUMP**.
* **Game Over**: Press the button again to restart the level.

### 3. Hardware Reset
* **Black Button**: Instantly reboots the STM32 board and returns to the Splash Screen.

## 📚 Course Information

* **Course**: MES204 - Embedded Systems
* **Platform**: STM32 Nucleo-64 (F446RE)
* **Objective**: Design a modular embedded system with UI and Hardware integration.

---
Created with ❤️ (https://github.com/nanangimanmochammad-dotcom)
