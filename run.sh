#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

# --- CONFIGURATION ---
PRESET="linux-debug"
BUILD_DIR="build/debug"
EXECUTABLE="./build/debug/src/galaxy"

echo "--- Checking Build Environment ---"

# 1. Check if the build directory exists. If not, Configure.
# We also pass -DCMAKE_EXPORT_COMPILE_COMMANDS=ON to ensure the JSON is generated.
if [ ! -d "$BUILD_DIR" ] || [ ! -f "$BUILD_DIR/CMakeCache.txt" ]; then
    echo "Build directory or Cache not found. Configuring project..."
    cmake --preset "$PRESET" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
else
    echo "Build configuration found. Skipping configure step."
fi

# 2. Build the project
echo "--- Building Project ---"
cmake --build --preset "$PRESET"

# 3. Handle compile_commands.json (Crucial for Neovim/LSP)
# If the file exists in the build folder, create a symlink in the root directory.
if [ -f "$BUILD_DIR/compile_commands.json" ]; then
    if [ ! -f "compile_commands.json" ]; then
        echo "--- Linking compile_commands.json to root ---"
        ln -sf "$BUILD_DIR/compile_commands.json" .
    fi
fi

# 4. Run the executable
if [ -f "$EXECUTABLE" ]; then
    echo "--- Running Application ---"
    "$EXECUTABLE"
else
    echo "Error: Executable not found at $EXECUTABLE"
    exit 1
fi
