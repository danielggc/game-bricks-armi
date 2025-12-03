cmake_minimum_required(VERSION 3.10)
project(BrickGameEngine)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})
include_directories(/opt/homebrew/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../brickc/src)

set(BRICKC_SOURCES
    ../brickc/src/lexer.cpp
    ../brickc/src/parser.cpp
    ../brickc/src/ast.cpp
    ../brickc/src/symbols.cpp
    ../brickc/src/error.cpp
    ../brickc/src/util.cpp
)

set(ENGINE_SOURCES
    src/renderer.cpp
    src/input_manager.cpp
    src/game_engine.cpp
    src/brick_loader.cpp
    ${BRICKC_SOURCES}
)

set(SDL_MENU_SOURCES
    ${ENGINE_SOURCES}
    src/snake_game.cpp
    src/tetris_game.cpp
    src/tank_game.cpp
    src/main.cpp
)

add_executable(brick_menu ${SDL_MENU_SOURCES})
target_link_libraries(brick_menu ${SDL2_LIBRARIES} -L/opt/homebrew/lib -lSDL2_ttf m)

set_target_properties(brick_menu PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_custom_command(TARGET brick_menu POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Build complete!"
    COMMAND ${CMAKE_COMMAND} -E echo "Run: ./bin/brick_menu"
    COMMAND ${CMAKE_COMMAND} -E echo ""
)