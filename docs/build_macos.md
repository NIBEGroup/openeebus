# Building OpenEEBUS by NIBE for macOS

This document explains how to build and run OpenEEBUS by NIBE on a macOS system for development and testing.

## Dependencies Installation

Make sure you have installed the required dependencies. This includes the development files needed, such as headers and CMake configuration files.

1. Install OpenSSL
   ```sh
   brew install openssl@3.0
   ```
1. Install libwebsockets
   ```sh
   brew install libwebsockets
   ```
1. Install cjson
   ```sh
   brew install cjson
   ```

You also need to make sure you have a GCC toolchain with CMake installed.

## Building One of the Examples

1. Create a build subdirectory under the repository and change working directory to it
   ```sh
   mkdir build && cd build
   ```
1. Run CMake to configure the project. In the example we use the `ninja` build tool, but other build tools should
   work similarly.
   ```sh
   cmake -GNinja ..
   ```
1. Build the application
   ```sh
   ninja
   ```

## Running the Examples

The examples are built within the OpenEEBUS repository and no additional repository is needed.

### hems Example

```sh
cd build
./hems
```

### heat_pump Example

```sh
cd build
./heat_pump
```

## Testing

See [Testing](testing.md) for the test layout and commands to build and run unit and integration tests.
