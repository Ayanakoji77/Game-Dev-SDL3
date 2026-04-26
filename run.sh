#!/bin/bash

ro status.
set -e


PRESET="linux-debug"
BUILD_DIR="build/debug"
EXECUTABLE="./build/debug/src/galaxy"

echo "--- Checking Build Environment ---"

if [ ! -d "$BUILD_DIR" ] || [ ! -f "$BUILD_DIR/CMakeCache.txt" ]; then
    echo "Build directory or Cache not found. Configuring project..."
    cmake --preset "$PRESET" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
else
    echo "Build configuration found. Skipping configure step."
fi


echo "--- Building Project ---"
cmake --build --preset "$PRESET"

if [ -f "$BUILD_DIR/compile_commands.json" ]; then
    if [ ! -f "compile_commands.json" ]; then
        echo "--- Linking compile_commands.json to root ---"
        ln -sf "$BUILD_DIR/compile_commands.json" .
    fi
fi


if [ -f "$EXECUTABLE" ]; then
    echo "--- Running Application ---"
    "$EXECUTABLE"
else
    echo "Error: Executable not found at $EXECUTABLE"
    exit 1
fi
