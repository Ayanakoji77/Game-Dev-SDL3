# Find the SDL3 library on the system.
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(glm REQUIRED)

# Create the executable from its source file.
add_executable(galaxy "sdl3-demo.cpp")

# Copy data directory to build output
add_custom_command(
  TARGET galaxy
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/data"
          "$<TARGET_FILE_DIR:galaxy>/data"
  COMMENT "Copying data assets to build directory"
  VERBATIM)

# IMPORTANT: Link the executable against the SDL3 library.
target_link_libraries(galaxy PRIVATE SDL3::SDL3 SDL3_image::SDL3_image glm::glm)
