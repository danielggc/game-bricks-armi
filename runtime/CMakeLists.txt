cmake_minimum_required(VERSION 3.10)
project(BrickGameEngine)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2
find_package(SDL2 REQUIRED)

# Include directories
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Common source files
set(COMMON_SOURCES
    src/renderer.cpp
    src/input_manager.cpp
    src/game_engine.cpp
)

# Snake game executable
set(SNAKE_SOURCES
    ${COMMON_SOURCES}
    src/snake_game.cpp
    src/main.cpp
)

add_executable(brick_snake ${SNAKE_SOURCES})
target_link_libraries(brick_snake ${SDL2_LIBRARIES})

# Tetris game executable
set(TETRIS_SOURCES
    ${COMMON_SOURCES}
    src/tetris_game.cpp
    src/main_tetris.cpp
)

add_executable(brick_tetris ${TETRIS_SOURCES})
target_link_libraries(brick_tetris ${SDL2_LIBRARIES})

# Main menu launcher executable
set(MENU_SOURCES
    ${COMMON_SOURCES}
    src/snake_game.cpp
    src/tetris_game.cpp
    src/menu.cpp
    src/main_menu.cpp
)

add_executable(brick_games ${MENU_SOURCES})
target_link_libraries(brick_games ${SDL2_LIBRARIES})

# Set output directory
set_target_properties(brick_snake brick_tetris brick_games PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Print info after build
add_custom_command(TARGET brick_snake brick_tetris brick_games POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "======================================"
    COMMAND ${CMAKE_COMMAND} -E echo "Build complete! 🎮"
    COMMAND ${CMAKE_COMMAND} -E echo "======================================"
    COMMAND ${CMAKE_COMMAND} -E echo "Available games:"
    COMMAND ${CMAKE_COMMAND} -E echo "  ./bin/brick_games      - Main launcher with menu"
    COMMAND ${CMAKE_COMMAND} -E echo "  ./bin/brick_snake      - Snake game"
    COMMAND ${CMAKE_COMMAND} -E echo "  ./bin/brick_tetris     - Tetris game"
    COMMAND ${CMAKE_COMMAND} -E echo ""
)
