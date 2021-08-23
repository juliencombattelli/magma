# Magma

## Supported platforms

This project is tested on the following platforms:
- Ubuntu 20.04
- Windows 10 (x64)

The steps to build and use the project are not guarantied to work on other platforms/versions.

## Install dependencies

- Vulkan SDK
- GLFW 3.3
- GLM 0.9.9
- EnTT ??

### Ubuntu

```
sudo apt install \
    vulkan-tools libvulkan-dev vulkan-validationlayers-dev spirv-tools \
    libglfw3-dev \
    libglm-dev
```

> Note: if running Ubuntu on WSL2 with a Windows build that does not support WSLg, please install
[wsl-vulkan-mesa](https://github.com/gnsmrky/wsl-vulkan-mesa)

### Windows

* Install the Microsoft Visual C/C++ Compiler 2019 at least:
  - if you want the full-fledged Visual Studio IDE, install [Visual Studio 2019](https://visualstudio.microsoft.com/vs/)
  - if you prefer just the command line tools, install the [Build Tools for Visual Studio 2019](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2019)

* Install the latest [Vulkan SDK](https://sdk.lunarg.com/sdk/download/latest/windows/vulkan-sdk.exe)

* Install [GLFW v3.3.4](https://github.com/glfw/glfw/releases/download/3.3.4/glfw-3.3.4.bin.WIN64.zip)

* Install the latest [GLM v0.9.9.x](https://github.com/g-truc/glm/releases)

## Building

### Ubuntu

### Windows

Open a prompt with x64 build tools available, go at the project root location and run the following commands:
```shell
# Configure the project
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -Dglfw3_DIR="<path/to/glfw>" -Dglm_DIR="<path/to/glm>" \
    -S . -B build-ninja-multi-msvc -G "Ninja Multi-Config"

# Build the project
cmake --build build-ninja-multi-msvc --config release
```

 rm -Recurse .\build-ninja-multi-msvc\ ; cmake -S . -B build-ninja-multi-msvc -Dglfw3_DIR=C:\Users\jucom\Development\VulkanWorkspace\Dependencies\glfw-3.3.4-win64-release\lib\cmake\glfw3 -Dglm_DIR=C:\Users\jucom\Development\VulkanWorkspace\Dependencies\glm-0.9.9.8\cmake\glm -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -G"Ninja Multi-Config" ; cmake --build .\build-ninja-multi-msvc\