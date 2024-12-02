| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Real-time systems

This project is part of an ESP32-based real-time system designed to manage tasks and events using FreeRTOS. It implements functionalities such as interrupt handling, LED control and communication between tasks using queues and flags.

# Project Overview 
This project demonstrates the use of FreeRTOS in ESP32. Key features include:
- Configuring GPIOs for input and output.
- Creating and managing tasks with FreeRTOS.
- Task delays and scheduling using vTaskDelay.


## Example folder contents
The project Task_Create_AND_Semaphores contains one source file in C language [main.c](main/main.c). The file is located in folder [main](main).

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt`
files that provide set of directives and instructions describing the project's source files and targets
(executable, library, or both). 

Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── main
│   ├── CMakeLists.txt
│   └── main.c
└── README.md                  This is the file you are currently reading
```
Additionally, the sample project contains Makefile and component.mk files, used for the legacy Make based build system. 
They are not used or needed when building with CMake and idf.py.
