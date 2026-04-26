find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(glm REQUIRED)

set(ENGINE_SOURCES
    core/animation.h core/timer.h core/application.cpp core/application.h
    core/resourceManager.cpp core/resourceManager.h)

set(GAME_SORCES
    game/gameobject.h
    game/Level.cpp
    game/Level.h
    game/player.cpp
    game/player.h
    game/bullet.h
    game/bullet.cpp
    game/enemy.h
    game/enemy.cpp)
add_executable(galaxy "main.cpp" ${ENGINE_SOURCES} ${GAME_SORCES})
target_include_directories(galaxy PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
add_custom_command(
  TARGET galaxy
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/data"
          "$<TARGET_FILE_DIR:galaxy>/data"
  COMMENT "Copying data assets to build directory"
  VERBATIM)

target_link_libraries(galaxy PRIVATE SDL3::SDL3 SDL3_image::SDL3_image glm::glm)
