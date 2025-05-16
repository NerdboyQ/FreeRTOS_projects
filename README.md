# FreeRTOS Projects

*This repository will serve as a workbench to practice freeRTOS concepts using popular microcontroller platforms (e.g. ESP32CAM, Pi Pico, etc)*
> NOTE: This code is all being developed as C/C++ projects in VS Code using the official <ins>**Raspberry Pi Pico Project**</ins> extension.

## Environment Requirements:
* cmake
* gcc (minGw)
* make
* freeRTOS capable RP2040 Repo   
    **(I used the [Smittytone github repository](https://github.com/smittytone/RP2040-FreeRTOS))**   
    > NOTE: These steps were done already.
    1. Clone the repo:  
    `git clone https://github.com/smittytone/RP2040-FreeRTOS`.
    2. Enter the repo: cd RP2040-FreeRTOS.   
    `cd RP2040-FreeRTOS`
    3. Prepare the submodules:    
    `git submodule update --init`.
    4. Install Pico SDK submodules:   
    `cd pico-sdk && git submodule update --init`.

### "Gotchas"
* The pico reset button must be pressed+held when plugging in the USB cable to flash new code
* When in flashing mode, the COM port will not appear in the Serial Monitor 
* Once the code is successfully flashed to the Pico, the coded will immediately start running and the COM port will re-appear
* Absolute paths were used in the .vscode/tasks.json file as the variable for the target path pointed to the project root directory. Without the change the extension failed to find the elf file because the project structure was changed to store the main code in a `src` subdirectory.

## Pi Pico W freeRTOS - Serial LED Flash Demo 
### Objective:   
* Practice concurrency with I/O control + user input processing

### Functionality:
* Two tasks: 
    1. Read user Serial input 
    2. Flash the Pico W onboard LED

    > NOTE: The Pico W onboard LED is not controller by a digital PIN, so board specific code was needed to control the LED directly.
* The LED will flash on a given frequency
* The user can provide timeout values up to 10 digits long that affects the flashing frequency of the LED
    > NOTE: If invalid characters, or values, are provided they will be ignored and an error message will be displayed in the serial console.  