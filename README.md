# Base Demo of SDL3 Game Trial Demo and Understanding of SDL3 Gained

This repository contains a prototype 2D game built to explore the new SDL3 library. It features a custom C++ engine architecture with a built-in animation framework, resource management, and basic physics/collision detection. The project leverages CMake for modern C++20 builds and includes automated scripts optimized for terminal-based development environments.

---

## Architecture & Features

The codebase is split into two distinct modules:
* **Core Engine (`src/core/`):** Handles the application lifecycle, frame timing, 2D animation parsing, and texture resource management.
* **Game Logic (`src/game/`):** Contains the gameplay implementation, including a tile-based level system with parallax scrolling, a controllable player character, projectile bullets, and a patrolling enemy class.

---

## Dependencies

To build this project, ensure the following libraries and tools are installed on your system:
* **CMake** (v3.15 or higher)
* **C++20** compatible compiler (GCC/Clang)
* **SDL3** (Core multimedia library)
* **SDL3_image** (Texture loading)
* **GLM** (Mathematics library for vector calculations)

---

## Build and Execution

The project includes a unified Bash script (`run.sh`) to streamline the build process. It utilizes CMake presets (specifically `linux-debug`) to configure, compile, and execute the game in a single command.

### Running the Game
Simply execute the script from the root of the project directory:

```bash
chmod +x run.sh
./run.sh
```

# Project Insights: Build System & SDL3 Learnings

This document outlines the utility of the automation scripts and the core technical concepts explored during the development of the SDL3 game engine prototype.

---

## What `run.sh` Does
The `run.sh` script is designed to streamline the development workflow, particularly for terminal-centric environments like Neovim.

* **Environment Check**: Verifies if the `build/debug` directory exists and runs the CMake configuration step if missing.
* **Compilation**: Builds the `galaxy` executable using the defined CMake preset (`linux-debug`).
* **LSP Integration**: Automatically exports and links `compile_commands.json` to the project root, providing immediate language server support for editors like Neovim.
* **Asset Management**: Triggers the CMake post-build step to copy the `data/` directory (containing sprites and textures) into the build folder so the executable can locate them.
* **Execution**: Launches the compiled game executable directly after a successful build.

---

## Understanding of SDL3 Gained
Developing this prototype provided hands-on experience with several fundamental and advanced concepts in SDL3 and modern C++ game development:

* **Modern SDL3 Initialization**: Transitioning from SDL2 paradigms to the updated SDL3 video and renderer initialization flow, managing resizable windows, and handling logical presentation (letterboxing) natively.
* **Texture & Asset Management**: Implementing an efficient `ResourceManager` utilizing `SDL3_image` to cache and retrieve textures, preventing memory leaks and redundant disk I/O.
* **Delta Time & Timers**: Creating a custom `Timer` class to decouple game logic and animation framerates from hardware rendering speed, ensuring consistent movement across different machines.
* **Custom Animation Framework**: Building a robust `Animation` class that slices sprite sheets into `SDL_FRect` source boundaries based on row indices and timing thresholds.
* **AABB Collision Detection**: Utilizing `SDL_GetRectIntersectionFloat` to build a physics engine that resolves overlaps between dynamic objects (player, enemies, bullets) and static level geometry.
* **Camera & Parallax Systems**: Implementing a tracking camera that follows the player entity while simultaneously scrolling background layers at varied speeds using `std::fmod` to create depth.
