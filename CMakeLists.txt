# Top-level CMake project file

# Set the minimum required CMake version.
cmake_minimum_required(VERSION 3.15)

# Define the project name and specify C++ as the language.
project("Game Dev using SDL3" LANGUAGES CXX)

# Set the C++ standard for the entire project (including sub-projects).
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add common compiler warnings for better code quality.
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include the sub-project where the executable is actually defined.
add_subdirectory("src")
