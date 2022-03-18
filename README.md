# ITk IoT infrastructure

## Branches
This repository has 4 branches:
- [main](https://stgit.dcs.gla.ac.uk/team-project-h/2021/cs34/itk_iot_infrastructure) which contains the final codebase with an example
- [Template](https://stgit.dcs.gla.ac.uk/team-project-h/2021/cs34/itk_iot_infrastructure/-/tree/Template) which contains the final codebase with a template for `experiment` namespace
- [LVGL Integration](https://stgit.dcs.gla.ac.uk/team-project-h/2021/cs34/itk_iot_infrastructure/-/tree/LVGL_integration) which is a research branch investigating LVGL
- [Simple OTA](https://stgit.dcs.gla.ac.uk/team-project-h/2021/cs34/itk_iot_infrastructure/-/tree/Simple_OTA) which implements simple OTA updates feature from scratch (this is kept as a research branch as the final codebase uses AsyncElegantOTA library)

## About this branch
This branch contains an example implementation of the project's integration with max31865 PT100.

To implement a different sensor, use [template branch](https://stgit.dcs.gla.ac.uk/team-project-h/2021/cs34/itk_iot_infrastructure/-/tree/Template).

## Organisation
### Contributors
- [Jakub Jelinek](https://stgit.dcs.gla.ac.uk/2478625j), 2478625J
- [Yousuf Abdullatif](https://stgit.dcs.gla.ac.uk/2482572a), 2482572A
- [Robbie Tippen](https://stgit.dcs.gla.ac.uk/2403237t), 2403237T

### Main customer
- Dima Maneuski


### Project itself
- Implementation of simple functions callable by serial monitor command-line interface using the Arduino framework with SimpleCLI library.
- Implementation of Over-the-Air updates of firmware.

## Getting started
### Setting up
Import the repository into VSCode with PlatformIO extension installed.

Connect your TTGO board to one of the USB ports on your device.

Press the PlatformIO Build button (or press Ctrl+Alt+B).

The project should build without any error. If an error saying
```bash
src/main.cpp:134:26: error: 'TFT_BACKLIGHT_ON' was not declared in this scope
```
occurs, copy the file `User_Setup_Select.h` from the main directory to the location specified below:
```bash
.pio/libdeps/esp32dev/TFT_eSPI/User_Setup_Select.h
```

Once the project can build, one can flash the firmware of the board with it by pressing the PlatformIO Upload button (or pressing Ctrl+Alt+U simultaneously).

### How to interact with the board
The project implements 3 states:
Measure (M)  - Update (U) - CLI (C)

#### Measure

This state runs the measurement defined in the experiment namespace.
It is the default state and is run automatically when the firmware starts.
It starts by connecting to WiFi. If this fails, it automatically enters the CLI.
If the board successfully connects to WiFi, the board starts executing the experiment::measure function, which should print data to serial.

#### Update

This state enables Over-the-air updates of firmware. When the setup finishes, it should print something like the following to serial:
```bash
SPIFFS mounted successfully
Connecting to WiFi ..Connected to PPE_IoT
You can update the firmware at:
(Use Ctrl+click to open the url in a browser.)
http://192.168.171.104/update
```

One can follow the link to a browser and update the firmware there by choosing a file.

Note: There is no confirmation. The firmware just updates to the new file.

The device restarts after updating and starts the new firmware.

#### CLI

The CLI enables the user to execute the following commands from serial:
- `info` - Lists status about WiFi, password and url
- `ssid <value>` - Changes WiFi SSID to `<value>`
- `pass <value>` - Changes WiFi password to `<value>`
- `run` - Enters the Measurement state
- `otau` - Enters the Update state

The TTGO board has two buttons under the display.
If one looks at the board with the TTGO sign on left, the button on top switches states in the order:
```
M->U->C
```
and from `C` it goes back to `M`.

The button at the bottom switches states in reverse order:
```
M<-U<-C
```

Note: Some states take few seconds to start (M and U). While a state is initialising, one cannot change it.

