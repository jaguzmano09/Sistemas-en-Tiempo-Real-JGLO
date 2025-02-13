| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP_WIFI_AP_STA

This project demonstrates how to configure an ESP32 to operate simultaneously as an access point (AP) and station (STA). This allows the ESP32 to connect to an existing WiFi network while providing its own network for other devices.


## Requirements
- ESP32 (any WiFi-enabled model)
- ESP-IDF (Installed and properly configured)
- Compiler and development tools (Python, CMake, Ninja, etc.)


## Example folder contents
The project **ESP_WIFI_AP_STA** contains one source file in C language [main.c](main/main.c). The file is located in folder [main](main).

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt`
files that provide set of directives and instructions describing the project's source files and targets
(executable, library, or both). 

Below is short explanation of remaining files in the project folder.

── CMakeLists.txt # Main configuration of the project
├─── main
│ ├─── CMakeLists.txt # Main code configuration
│ ├─── main.c # Source code of the project
└└─── README.md # Document you're reading

### Example Usage
To use this example, simply build and flash the project to your ESP32 device. The device will start operating as both a Wi-Fi station and an access point. You can connect to the access point using the SSID and password defined in the code.

### Configuration Parameters

- **Access Point (AP) Mode**:
  - SSID: `ESP_JAVIER_RED`
  - Password: `12345678`
  - Channel: `1`
  - Maximum Connections: `4`

- **Station (STA) Mode**:
  - SSID: `JAVIER_RED`
  - Password: `12345678J`
  - Authentication Mode: `WPA2_PSK`

### Logging

The code uses the ESP-IDF logging library to print information about the Wi-Fi status, such as connection attempts, IP addresses, and connected stations.

For more details, refer to the comments in the `main.c` file.


