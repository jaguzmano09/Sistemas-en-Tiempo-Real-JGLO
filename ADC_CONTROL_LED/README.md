| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# ADC_CONTROL_LED

This project demonstrates how to control an RGB LED based on ADC readings and button inputs using the ESP32. The brightness of each color channel (Red, Green, and Blue) is dynamically adjusted based on ADC input and button presses, allowing for real-time LED color configuration.

## Prerequisites

To run this project, you need to have the necessary libraries for handling the RGB LED. Make sure to install the required libraries before proceeding.

## How to use example
We encourage the users to use the example as a template for the new projects. A recommended way is to follow the instructions on a [docs page](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html#start-a-new-project).

## Example folder contents

The project ADC_CONTROL_LED contains the following files and folders:

```
├── CMakeLists.txt             # Root CMake configuration file
├── main
│   ├── CMakeLists.txt         # CMake configuration for the main folder
│   ├── LED_RGB.c              # Main source file RGB LED functions
│   ├── LED_RGB.h              # Header file for RGB LED functions
│   ├── main.c                 # Main source code for the project
└── README.md                  # Project documentation (this file)
```
Explanation of Files

- CMakeLists.txt: Configures the project build system using CMake.

- main/CMakeLists.txt: Provides build instructions specific to the main folder.

- main.c: Contains the primary logic for:
    - Reading ADC values to adjust brightness.
    - Detecting button inputs to switch LED colors.
    - Combining RGB channels based on user interaction.

- LED_RGB.h: Defines functions for controlling the RGB LED and managing color settings.

- LED_RGB.c: Implements the functions defined in LED_RGB.h for controlling the RGB LED.

- README.md: This file, documenting the purpose and usage of the project
