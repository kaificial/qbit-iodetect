# QBIT - Iodetect (Firmware Archive)

**Iodetect** repository (Firmware Archive) - **Queen's Biomedical Innovation Team (QBIT)** (2025/2026). 

Iodetect is a portable, point-of-care iodine screening system designed to measure urinary iodine levels in low-resource settings. The goal was to create an accessible, low-cost solution for iodine deficiency screening worldwide. 

This repository is the firmware for the microcontrollers (Arduino/ESP32) that was used topower the device, collect and process sensor data, and manage local hardware interfaces.

---

## Repository Structure

```text
qbit-iodetect/
├── docs/
│   └── design/                 # System designs, UI mockups, and project documentation
├── src/
│   └── firmware/
│       ├── archive/            # Historical iterations and older prototypes (v1 - v4)
│       └── components/         # Active, unified firmware components (e.g., lcd_buttons.ino)
├── tools/                      # Helper scripts and utilities
└── README.md                   # You are here!
```

## Hardware & Firmware

The Iodetect hardware relies on microcontrollers to interface with our custom chemical sensors. The firmware is responsible for:
- Reading raw analog data from the sensors
- Processing and calibrating the iodine concentration levels
- Displaying results locally via an LCD interface
- Managing peripheral inputs (buttons, indicators)

### Getting Started with the Firmware
1. Open the active `.ino` files located in `src/firmware/components/` using the [Arduino IDE](https://www.arduino.cc/en/software) or your preferred editor.
2. Make sure you have the correct board manager installed for your specific microcontroller.
3. Install any required libraries (like `LiquidCrystal` for the LCD).
4. Verify and upload the code to your connected board.

---

