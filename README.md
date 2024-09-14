# CHIP8 Emulator using C++

Chip-8 is a simple, interpreted, programming language which was first used on some do-it-yourself computer systems in the late 1970s and early 1980s. The COSMAC VIP, DREAM 6800, and ETI 660 computers are a few examples. These computers typically were designed to use a television as a display, had between 1 and 4K of RAM, and used a 16-key hexadecimal keypad for input. The interpreter took up only 512 bytes of memory, and programs, which were entered into the computer in hexadecimal, were even smaller.

## Building the project

First clone the repo with all submodules using the below command

```
git clone https://github.com/rushad01/chip8-emulator-cpp.git --recursive
```

If you didn't able to download all submodule using above comment then use the below command.

```
git pull && git submodule init && git submodule update --progress && git submodule status
```

Once all of the submodules are fetched from github then run the below command to build the project if you have only one compiler. CMake will detect it and setup everything according to it without any problem.

```
mkdir build
cd build
cmake -G"MinGW Makefiles" -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc ..  //for Mingw GCC Toolchain
make all
```

## Tools

- CMake(build system)
- clangd(lsp)
- GCC toolchain
- VS Code Editor(clangd plugin,CMake,CMake Tools)

## Library

- glfw
- glm
- glad
- imgui

## CHIP8 Programs

I used [Chip8 test suit](https://github.com/Timendus/chip8-test-suite) to test out my emulator. program folder contain roms from various repo. I will share their link here for clearification.

## Running CHIP8 Programs

From cmd or terminal use the below command for running the chip8 file. Use the files from programs folder. Executable of chip8 emulator will the in the build directory.

```
.\chip8-emulator-cpp path\to\valid_chip8_program.ch8
```
