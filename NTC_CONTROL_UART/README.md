| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# NTC_CONTROL_UART

## Description
This project demonstrates how to control an NTC sensor via UART by means of different commands using an ESP32. The main source code can be found in the file [main.c](main/main.c) inside the [main](main) folder, as well as taking into account the uses of the different libraries [ADC_NTC.h],[LED_RGB.h],[COMMANDS_UART.h].

## Requisites
- ESP32 or any other compatible board mentioned in the table of supported targets.
- NTC sensor.
- Properly configured UART connection.

## Hardware configuration
1. Connect the NTC sensor to the ESP32 according to your sensor schematic and UART pin configuration.
2. Ensure that the UART connection is correctly configured in the `COMMANDS_UART.h` file.

## Customisation

You can customise the source code in `main.c` to suit your specific needs, such as changing the UART pins or adjusting the NTC sensor read logic.

## Example folder contents

The project **sample_project** contains one source file in C language [main.c](main/main.c). The file is located in folder [main](main).

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt`
files that provide set of directives and instructions describing the project's source files and targets
(executable, library, or both). 

Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── main
│   ├── CMakeLists.txt
|   |__ADC_NTC.c
|   |__ADC_NTC.h
|   |__LED_RGB.c
|   |__LED_RGB.h
|   |__COMMANDS_UART.c
|   |__COMMANDS_UART.h
│   └── main.c
└── README.md                  This is the file you are currently reading
```
Additionally, the sample project contains Makefile and component.mk files, used for the legacy Make based build system. 
They are not used or needed when building with CMake and idf.py.

## Additional Resources

- [Documentación de ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp-idf/index.html)
- [Guía de inicio rápido de ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp-idf/get-started/index.html)
- [Ejemplos de ESP-IDF](https://github.com/espressif/esp-idf/tree/master/examples)

