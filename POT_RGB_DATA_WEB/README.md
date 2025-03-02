| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# POT_RGB_DATA_WEB

## Example description

This example demonstrates how to create a simple web server that reads the ADC value of a potentiometer and the temperature value read by the ADC of an NTC, and display these values on a web page. The RGB values are also displayed on the web page, and these RGB values can be modified directly on the page. The potentiometer is connected to a GPIO pin and the RGB LEDs are connected to the PWM pins. The web server is created using the ESP-IDF HTTP server. The example uses the ESP32 Wi-Fi module to create an access point and host the web server. The example also demonstrates how to use the ESP-IDF PWM module to control the RGB LEDs.

## How to use example
### Hardware Required

This example requires a potentiometer, NTC and RGB LEDs connected to the ESP32. The potentiometer is connected to GPIO pin 32, NTC is connected to GPIO pin 34 and the RGB LEDs are connected to GPIO pins 18, 19, and 21.

## Example folder contents

The project **POT_RGB_DATA_WEB** contains one source file in C language [main.c](main/main.c). The file is located in folder [main](main).

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt`
files that provide set of directives and instructions describing the project's source files and targets
(executable, library, or both). 

Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── main
│   ├── CMakeLists.txt
│   └── main.c
├── Librerias
│   ├── Adc_lib
│   │   ├── ADC_NTC_POT.c
│   │   └── ADC_NTC_POT.h
│   ├── Http_lib
│   │   ├── http_server.c
│   │   └── http_server.h
│   ├── RGB_lib
│   │   ├── LED_RGB.c
│   │   └── LED_RGB.h
│   ├── Uart_lib
│   │   ├── COMANDS_UART.c
│   │   └── COMANDS_UART.h
│   ├── Wifi_lib
│   │   ├── wifi_app.c
│   │   └── wifi_app.h
├── webpage
│   ├── app.css
│   ├── app.js
│   ├── favicon.ico
│   ├── index.html
│   ├── jquery-3.3.1.min.js
└── Readme.md
```
Additionally, the sample project contains Makefile and component.mk files, used for the legacy Make based build system. 
They are not used or needed when building with CMake and idf.py.
