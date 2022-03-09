# ITk IoT Infrastructure - Developer documentation
This is a developer documentation which describes the project's overall structure and explains how to add new features. To learn how to use the project, see [Getting started](../README.md).
## Architecture and design
The aim of the project is to simplify testing of sensors built at the University of Glasgow - School of Physics & Astronomy. It provides a template which one can fill in and use the following two features:
## CLI - Command-line interface
This feature provides a text-based user interface through microcontroller's serial port. This is implementing a [SimpleCLI](https://github.com/SpacehuhnTech/SimpleCLI) library.

To see the various functions already implemented, see [Getting started](../README.md).

### How to add a new function


## OTA - Over-the-air
This part of the project provides over-the-air firmware update support. This is enabled by the [AsyncElegantOTA](https://github.com/ayushsharma82/AsyncElegantOTA) library.

## Code structure

