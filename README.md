# CHIP8 Emulator using C++

Chip-8 is a simple, interpreted, programming language which was first used on some do-it-yourself computer systems in the late 1970s and early 1980s. The COSMAC VIP, DREAM 6800, and ETI 660 computers are a few examples. These computers typically were designed to use a television as a display, had between 1 and 4K of RAM, and used a 16-key hexadecimal keypad for input. The interpreter took up only 512 bytes of memory, and programs, which were entered into the computer in hexadecimal, were even smaller.

## Building the project

First fetch all submodules using the below command

```
git pull && git submodule init && git submodule update && git submodule status
```

Once all of the submodules are fetched from github then run the below command to build the project.

```
mkdir build
cd build
cmake ..
```

## Tools

- CMake(build system)
- clangd(lsp)
- GCC/LLVM toolchain

## Library

- glfw
- glm
- glad
- imgui
