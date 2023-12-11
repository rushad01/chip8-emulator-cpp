# OpenGL project template using c++

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
