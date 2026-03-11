# Echoes of the Forgotten Keep

## Requirements

- CMake 4.1 or newer
- C++20 compiler

## Dependencies

This project uses:

- SDL3
- OpenGL
- GLM

CMake will try to find installed packages first.  
If SDL3 or GLM are not installed, they will be downloaded automatically with `FetchContent`.

## Build

```bash
cmake -B build
cmake --build build
