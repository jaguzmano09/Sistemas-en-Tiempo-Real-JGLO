| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# WEB_TEMPERATURE_ESP32
This project implements a web-based system to monitor the voltage of a potentiometer and the temperature measured by an NTC sensor, using an ESP32 with the ESP-IDF framework. Also by means of the management of buttons on the web page it allows the sending of data by uart in this case the temperature and the denial of this action. A web interface is provided where the data is updated in real time via HTTP requests.

## How to use example
This project demonstrates how to read temperature from an NTC sensor and the voltage from a potentiometer using the ESP32 ADC, process the data, and send it over Wi-Fi.
### Requirements
- ESP32 Development Board
- ESP-IDF installed and configured
- NTC Thermistor
- Potentiometer

## Example folder contents

The project **WEB_TEMPERATURE_ESP32t** contains one source file in C language [main.c](main/main.c). The file is located in folder [main](main).

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt`
files that provide set of directives and instructions describing the project's source files and targets
(executable, library, or both). 

Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── main
|   ├── Librerias
|   |   ├── Adc_lib
|   |   |   ├── ADC_NTC_POT.c
|   |   |   ├── ADC_NTC_POT.h
|   |   ├── rgb_lib
|   |   |   ├── rgb_leb.c
|   |   |   ├── rgb_leb.h
|   |   ├── server_lib
|   |   |   ├── http_server.c
|   |   |   ├── http_server.h
|   |   ├── Uart_lib
|   |   |   ├── COMANDS_UART.c
|   |   |   ├── COMANDS_UART.h
|   |   ├── Wifi_lib
|   |   |   ├── wifi_app.c
|   |   |   ├── wifi_app.h
|   ├── webpage
|   |   ├── app.cs
|   |   ├── app.js
|   |   ├── favicon.ico
|   |   ├── index.html
|   |   ├── jquery-3.3.1.min.js
│   ├── CMakeLists.txt
|   ├── tasks_common.h
│   └── main.c
└── README.md                  This is the file you are currently reading
```
Additionally, the sample project contains Makefile and component.mk files, used for the legacy Make based build system. 
They are not used or needed when building with CMake and idf.py.
