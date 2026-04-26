# Toolchain file for cross-compiling for 64-bit Windows from Linux

# 1. Set the target system name
set(CMAKE_SYSTEM_NAME Windows)

# 1. Specify the cross-compilers
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

# 1. Set the resource compiler (important for Windows executables)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# 1. Point to the root of the target environment (MinGW handles this
#   automatically, but this is good practice)
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)

# 1. Modify search behavior for libraries and programs
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
