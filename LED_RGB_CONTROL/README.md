| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# LED_RGB_CONTROL

This project demonstrates how to configure and control an RGB LED connected to an ESP32 using the LEDC hardware for PWM (Pulse Width Modulation). The project includes a library for RGB LED control and supports configurable GPIO pins and duty cycle adjustments for each color channel (Red, Green, and Blue).



## Features
- Configures the LEDC timer and RGB LED channels (Red, Green, Blue).

- Allows setting the RGB LED color by adjusting the PWM duty cycle for each channel.

- Includes error handling for timer and channel initialization.

- Designed to work with ESP32 LEDC hardware for high-performance PWM control.

## How to Use the Project
This project can serve as a template for configuring and controlling RGB LEDs in ESP32-based applications. Follow the steps below to build and run the project:

Clone the repository and navigate to the project directory.

Connect the RGB LED to the specified GPIO pins:
- Red: GPIO 18

- Green: GPIO 19

- Blue: GPIO 21

Compile and flash the project to your ESP32 board using the idf.py build and idf.py flash commands.

Monitor the output with idf.py monitor to verify functionality

The project contains the following files and folders:
´´´
├── CMakeLists.txt             # Main CMake configuration
├── components
│   └── LED_RGB                # RGB LED control library
│       ├── LED_RGB.h          # Header file for the RGB LED library
│       └── LED_RGB.c          # Implementation file for the RGB LED library
├── main
│   ├── CMakeLists.txt         # CMake configuration for main application
│   └── main.c                 # Main application file
└── README.md                  # Project documentation
´´´

