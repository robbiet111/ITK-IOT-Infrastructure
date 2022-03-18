# ITk IoT infrastructure

## Branches
This repository has 4 branches:
- [main](https://stgit.dcs.gla.ac.uk/team-project-h/2021/cs34/itk_iot_infrastructure) which contains the final codebase with an example
- [Template](https://stgit.dcs.gla.ac.uk/team-project-h/2021/cs34/itk_iot_infrastructure/-/tree/Template) which contains the final codebase with a template for `experiment` namespace
- [LVGL Integration](https://stgit.dcs.gla.ac.uk/team-project-h/2021/cs34/itk_iot_infrastructure/-/tree/LVGL_integration) which is a research branch investigating LVGL
- [Simple OTA](https://stgit.dcs.gla.ac.uk/team-project-h/2021/cs34/itk_iot_infrastructure/-/tree/Simple_OTA) which implements simple OTA updates feature from scratch (this is kept as a research branch as the final codebase uses AsyncElegantOTA library)

## Contributors
- [Jakub Jelinek](https://stgit.dcs.gla.ac.uk/2478625j), 2478625J
- [Yousuf Abdullatif](https://stgit.dcs.gla.ac.uk/2482572a), 2482572A

## Main customer
- Dima Maneuski


## Project itself
- Implementation of simple functions callable by serial monitor command-line interface using the Arduino framework with SimpleCLI library.
- Implementation of Over-the-Air updates of firmware.

# LVGL
LVGL (Light and Versatile Graphics Library) is a library enabling an embedded GUI with easy to use widgets.

This branch is a research branch with the aim of integrating the LVGL library with an existing project with CLI enabled. It was designed for the WT32-SC01 board which uses the ESP32-WROVER-B module.

In LVGL the basic building blocks are the objects. In the `main.cpp` they are declared between lines `25` and `31`. Then they are assigned attributes in the `setup` function which starts at line `443`. More about the objects can be learned from the LVGL documentation [here](https://docs.lvgl.io/master/overview/object.html).

The main LVGL functions are handled by FreeRTOS tasks and an event handler. Unfortunately, the [documentation](https://docs.lvgl.io/master/index.html) is very extensive, but as the library is quite involved, the [documentation](https://docs.lvgl.io/master/index.html) is very difficult to follow or search for information in it. Therefore, we were not able to figure out how exactly the LVGL - FreeRTOS integration works.

This branch, therefore, compiles, but does not work. The problem is that both LVGL and WiFi try to run on the same core, which makes the firmware crash. An attempt has been made to fix this issue, but due to LVGL integration being a lower priority for the overall project, we decided with the customer to stop the development to be able to finish the other parts of the project.
