# Find the SDL3 library on the system.
find_package(SDL3 REQUIRED)

# Create the executable from its source file.
add_executable(galaxy "sdl3-demo.cpp")

# IMPORTANT: Link the executable against the SDL3 library. This gives your
# program access to all of SDL3's functions.
target_link_libraries(galaxy PRIVATE SDL3::SDL3)
