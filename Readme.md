# Magma

## Supported platforms

This project is tested on the following platforms:
- Ubuntu 20.04
- Windows 10 (x64)

The steps to build and use the project are not guarantied to work on other platforms/versions.

## Install dependencies

- Vulkan SDK 1.2
- GLFW 3.3
- GLM 0.9.9

### Ubuntu 20.04

* Install the latest Vulkan SDK:
```bash
wget -qO - https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo apt-key add -
sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-focal.list https://packages.lunarg.com/vulkan/lunarg-vulkan-focal.list
sudo apt update
sudo apt install vulkan-sdk
```

* Install the remaining packages:
```bash
sudo apt install libglfw3-dev libglm-dev
```

> Note: if running Ubuntu on WSL1, or WSL2 with a Windows build that does not support WSLg, please install [wsl-vulkan-mesa](https://github.com/gnsmrky/wsl-vulkan-mesa)

### Windows 10

* Install the Microsoft Visual C/C++ Compiler 2019 at least:
  - if you want the full-fledged Visual Studio IDE, install [Visual Studio 2019](https://visualstudio.microsoft.com/vs/)
  - if you prefer just the command line tools, install the [Build Tools for Visual Studio 2019](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2019)

* Install the latest [Vulkan SDK](https://sdk.lunarg.com/sdk/download/latest/windows/vulkan-sdk.exe)

* Install [GLFW v3.3.4](https://github.com/glfw/glfw/releases/download/3.3.4/glfw-3.3.4.bin.WIN64.zip)

* Install the latest [GLM v0.9.9.x](https://github.com/g-truc/glm/releases)

## Building

### Ubuntu

In a terminal open at the project root directory, run the following commands:
```bash
# Configure the project
cmake -S . -B _build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=_install

# Build, install and package the project
cmake --build _build/release --target all install package_source package
```

### Windows

Open a prompt with x64 build tools available, go at the project root location and run the following commands:
```bash
# Configure the project
cmake -Dglfw3_DIR="<path/to/glfw>" -Dglm_DIR="<path/to/glm>" \
    -S . -B build-ninja-multi-msvc -G "Ninja Multi-Config"

# Build the project
cmake --build build-ninja-multi-msvc --config release
```
