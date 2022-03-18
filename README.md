# ITk IoT infrastructure

## Branches
This repository has 4 branches:
- [main](https://stgit.dcs.gla.ac.uk/team-project-h/2021/cs34/itk_iot_infrastructure) which contains the final codebase with an example
- [Template](https://stgit.dcs.gla.ac.uk/team-project-h/2021/cs34/itk_iot_infrastructure/-/tree/Template) which contains the final codebase with a template for `experiment` namespace
- [LVGL Integration](https://stgit.dcs.gla.ac.uk/team-project-h/2021/cs34/itk_iot_infrastructure/-/tree/LVGL_integration) which is a research branch investigating LVGL
- [Simple OTA](https://stgit.dcs.gla.ac.uk/team-project-h/2021/cs34/itk_iot_infrastructure/-/tree/Simple_OTA) which implements simple OTA updates feature from scratch (this is kept as a research branch as the final codebase uses AsyncElegantOTA library)

## About this branch
This is a research branch implementing a simple over-the-air updates feature. It does not use any library to do this. In the main project, it was replaced by the `AsyncElegantOTA` library.

## Contributors
- [Jakub Jelinek](https://stgit.dcs.gla.ac.uk/2478625j), 2478625J
- [Yousuf Abdullatif](https://stgit.dcs.gla.ac.uk/2482572a), 2482572A

## Main customer
- Dima Maneuski


## Project itself
- Implementation of simple functions callable by serial monitor command-line interface using the Arduino framework with SimpleCLI library.
- Implementation of Over-the-Air updates of firmware.
