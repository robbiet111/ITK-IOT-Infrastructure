# ITk IoT Infrastructure - Developer documentation

This is a developer documentation which describes the project's overall structure and explains how to add new features. To learn how to use the project, see [Getting started](../README.md).

## Architecture and design

The aim of the project is to simplify testing of sensors built at the University of Glasgow - School of Physics & Astronomy. It provides a template which one can fill in and use the following two features:

## CLI - Command-line interface

This feature provides a text-based user interface through microcontroller's serial port. This is implementing a [SimpleCLI](https://github.com/SpacehuhnTech/SimpleCLI) library.

To see the various functions already implemented, see [Getting started](../README.md#how-to-interact-with-the-board).

### How to add a new function

The CLI feature has its own dedicated namespace. To get to know how namespaces work, see [Code structure - Namespaces](#namespaces).

Each CLI function needs to define a command with `Command <name_of_command>`. This should be done in the `CLI` namespace. Define it as extern in the header file and then declare it again in the C++ file `src/cli.cpp`. This source file also implements the `CLI::begin` function, where the command is actually initialised. Use the [SimpleCLI documentation](https://github.com/SpacehuhnTech/SimpleCLI#adding-commands) to add the command. To add a callback function to a command (function which is called when the command is executed), define it in the namespace and just pass it as an argument to the command when initialising it.

Example:

We want to implement a function which prints `Hello, <name>!` to serial.

We would start by defining a new command in `include/cli_namespace.h` with
```cpp
extern Command sayhello;
```

and then in `src/cli.cpp` as follows:
```cpp
Command sayhello;
```

We also need to create the callback function. At least the signature needs to be in `include/cli_namespace.h`. The body can be implemented in `src/cli.cpp`.

1. Inline function in the header file
```cpp
inline void sayhelloCallback(cmd* c) {
    Command cmd(c); // Create wrapper object

    // Get first (and only) Argument
    Argument arg = cmd.getArgument(0);

    
    // Get value of argument
    String argVal = arg.getValue();
    const char * name = argVal.c_str();

    Serial.println("Hello, %s!", value);
}
```

2. Signature in the header file, implementation in `src/cli.cpp`.
```cpp
/*
 *  In include/cli_namespace.h
 */
void sayhelloCallback(cmd* c);

/*
 *  In src/cli.cpp
 */
void sayhelloCallback(cmd* c) {
    Command cmd(c); // Create wrapper object

    // Get first (and only) Argument
    Argument arg = cmd.getArgument(0);

    
    // Get value of argument
    String argVal = arg.getValue();
    const char * name = argVal.c_str();

    Serial.println("Hello, %s!", value);
}
```

Once the callback function is implemented, we can add the command to the `CLI::begin` function in `src/cli.cpp`:
```cpp
sayhello = cli.addSingleArgCmd("sayhello", sayhelloCallback);
```

## OTA - Over-the-air
This part of the project provides over-the-air firmware update support. This is enabled by the [AsyncElegantOTA](https://github.com/ayushsharma82/AsyncElegantOTA) library.

## Code structure
### Namespaces
