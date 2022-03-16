<!--
    Author: Jakub Jelinek
    In case of any questions, reach out at: 2478625J@student.gla.ac.uk
    or at: jakubjjelinek@gmail.com
-->

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
Note: We use `addSingleArgCmd` as the command takes one argument. To find out more about command types, see [SimpleCLI command documentation](https://github.com/SpacehuhnTech/SimpleCLI#command).

## OTA - Over-the-air

This part of the project provides over-the-air firmware update support. This is enabled by the [AsyncElegantOTA](https://github.com/ayushsharma82/AsyncElegantOTA) library. Originally, we have built this feature from scratch, but the features and the performance of the library made us switch to it. We followed this [tutorial](https://randomnerdtutorials.com/esp32-ota-over-the-air-arduino/) to implement a simple functionality of updating the firmware.

One can update the firmware by navigating to the Update state via buttons or a command in the CLI. See [Getting started](../README.md#how-to-interact-with-the-board) to learn more. Then opening a serial port, the OTA setup function prints the url to navigate to for the update. After choosing a binary file (`.bin`) the library updates the firmware and the device restarts.

## Code structure

To make the codebase more modular and easier to follow, we have divided it into multple files. Firstly, we wanted to use classes to do so as we were familiar with the concept, however, namespaces were more appropriate.

### Namespaces

Namespaces should be defined in the `include` directory as a header file. If needed, they can also implement a `.cpp` file in the `src` directory, but more about that below.

The main function of namespaces is to prevent name conflicts, however, they can be used as a means of a block containing variables and functions related to one part of a project.

To use a namespace in a different file, the header file has to be included. For example, to include the `common` namespace defined in `common_namespace.h`, one would use:

```cpp
#include "common_namespace.h"
```

To use variables and function defined in a namespace, there are two options:
1. With the using-directive `using namespace <name>` introduce every name from the namespace into the global namespace.
2. Use the variables and functions with the scope resolution operator `<name>::<var/function>`

For example, to use function `sensor_setup()` defined in the `common` namespace, one can either do:

```cpp
#include "common_namespace.h"

using namespace common;

sensor_setup();
```

or use the scope resolution operator as follows:

```cpp
#include "common_namespace.h"

common::sensor_setup();
```

The using-directive can be useful if there are no other variables or functions with the same name as variables in the namespace in current scope and namespace is used frequently.

On the other hand, the scope resolution operators can make the origin of variables and functions clearer, therefore, this is the recommended way for most use cases.

#### Header and C++ files

The following sections explain what should be defined in what type of a file.

In general, each namespace needs a header file to be defined and it might need an additional source file. The header file should be in the `include` directory. The source file should be in `src`.

The source file needs to include the header file and redefine the namespace to be used. It can define the variables or bodies of non-inline functions as can be seen below.

#### Declaring variables in a namespace

To prevent linking errors reporting about redefinition of variables, it is recommended to declare variables as `extern` in the namespace header file. They should also be just declared and not defined (they should not be assigned a value). In this case, the variable needs to be defined in a separate file. The example below should make this clearer.

**Example:**

We want to define a variable `name` in a new namespace called `student`.

First, we declare the variable in a file called `include/student_namespace.h`:

```cpp
#ifndef STUDENT_NAMESPACE_H
#define STUDENT_NAMESPACE_H

namespace student {
    extern String name;
}

#endif
```

Then we need to actually define the variable somewhere. As it is a `String`, it does not need a value to start with, therefore we can define it in `src/student.cpp` as follows:

```cpp
#include "student_namespace.h"

namespace student {
    String name;
}
```

#### Declaring functions in a namespace

There are two ways how to declare functions in a namespace.

In the examples, we are going to illustrate how to define function called `examine()` in `example` namespace.

##### Inline functions

C++ provides the inline functions functionality to reduce number of function calls as they are expanded in line when called. This means that the whole code of the inline function gets inserted or substituted at the place where the function gets called. This insert is performed by the compiler itself. It might increase the efficiency of the function provided it is of a resonable size.

We can define function to be inline using the `inline` keyword as follows:

```cpp
inline <return-type> <function-name>(<parameters>) {
    // body of the function
}
```

*Note:* `inline` functions are not recommended for large functions and I/O bound functions.

Inline functions, therefore, remove the function call overhead, save the stack operations for the variables, and remove the overhead of the return call.

The main advantage for the namespace use case, however, is that they can just be defined fully in the namespace and no redefinition errors are going to occur.

**Example:**

To define the inline function `examine()` in `include/example_namespace.h`, we can proceed as follows:

```cpp
#ifndef EXAMPLE_NAMESPACE_H
#define EXAMPLE_NAMESPACE_H

namespace example {
    inline void examine() {
        // short body of a function
    }
}

#endif
```

##### Functions defined in the `C++` source file

Functions can be also defined in the `C++` file corresponding to the namespace header. This is the recommended option for any function that is longer than just few commands and for any I/O bound functions.

One needs to declare the signature of the function in the namespace header and then define the function in the source file.

**Example:**

To declare function `examine()` in the `example` namespace, we use the following in `include/example_namespace.h`:

```cpp
#ifndef EXAMPLE_NAMESPACE_H
#define EXAMPLE_NAMESPACE_H

namespace example {
    void examine();
}

#endif
```

and the following in the corresponding `C++` file. In this case this should be `example.cpp`:

```cpp
#include "example_namespace.h"

namespace example {
    void examine() {
        // longer
        // body
        // of
        // a
        // function
    }
}
```