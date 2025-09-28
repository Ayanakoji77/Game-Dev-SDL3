#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e
#export SDL_VIDEODRIVER=x11
# Build the project using the debug preset
echo "--- Building Project ---"
cmake --build --preset linux-debug

# Run the executable
echo "--- Running Application ---"
"./build/debug/src/galaxy"
