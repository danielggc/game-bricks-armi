#include "menu.hpp"
#include <iostream>

// ============================================================================
// Main Menu Implementation - Large and Visual
// ============================================================================

MainMenu::MainMenu()
    : engine(40, 26, 20),  // 800x520 window with 20px cells
      selectedOption(0),
      menuActive(true) {
}

MainMenu::~MainMenu() {
}

bool MainMenu::initialize() {
    if (!engine.initialize()) {
        return false;
    }
    
    input.mapKeyToAction(SDLK_UP, "up");
    input.mapKeyToAction(SDLK_w, "up");
    input.mapKeyToAction(SDLK_DOWN, "down");
    input.mapKeyToAction(SDLK_s, "down");
    input.mapKeyToAction(SDLK_LEFT, "left");
    input.mapKeyToAction(SDLK_a, "left");
    input.mapKeyToAction(SDLK_RIGHT, "right");
    input.mapKeyToAction(SDLK_d, "right");
    input.mapKeyToAction(SDLK_RETURN, "select");
    input.mapKeyToAction(SDLK_SPACE, "select");
    
    return true;
}

void MainMenu::handleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        input.handleEvent(event);
    }
    
    input.update();
    
    if (input.isActionActive("up")) {
        if (selectedOption == 2) selectedOption = 1; // From EXIT to TETRIS
        SDL_Delay(150);
    }
    if (input.isActionActive("down")) {
        if (selectedOption < 2) selectedOption = 2;  // Any to EXIT
        SDL_Delay(150);
    }
    if (input.isActionActive("left")) {
        if (selectedOption == 1) selectedOption = 0; // From TETRIS to SNAKE
        SDL_Delay(150);
    }
    if (input.isActionActive("right")) {
        if (selectedOption == 0) selectedOption = 1; // From SNAKE to TETRIS
        SDL_Delay(150);
    }
}

void MainMenu::drawMenu() {
    engine.update(0.016f);
    
    // Title bar at top
    Color titleBorder(100, 150, 255);
    Color titleFill(30, 50, 80);
    
    // Draw title area (full width)
    for (int x = 0; x < 40; x++) {
        Entity top("title_top_" + std::to_string(x), Point(x, 1), titleBorder);
        engine.addEntity(top);
        Entity bot("title_bot_" + std::to_string(x), Point(x, 3), titleBorder);
        engine.addEntity(bot);
    }
    
    for (int y = 1; y <= 3; y++) {
        Entity left("title_left_" + std::to_string(y), Point(0, y), titleBorder);
        engine.addEntity(left);
        Entity right("title_right_" + std::to_string(y), Point(39, y), titleBorder);
        engine.addEntity(right);
    }
    
    // Title fill
    for (int x = 1; x < 39; x++) {
        for (int y = 2; y <= 2; y++) {
            Entity fill("title_fill_" + std::to_string(x), Point(x, y), Color(100, 200, 255));
            engine.addEntity(fill);
        }
    }
    
    // ===== GAME OPTIONS (Large boxes) =====
    
    // SNAKE (LEFT side)
    Color snakeColor = (selectedOption == 0) ? Color(255, 255, 0) : Color(0, 255, 0);
    
    // Snake box - big
    int snakeX = 2;
    int snakeY = 7;
    int snakeW = 15;
    int snakeH = 10;
    
    // Border
    for (int x = snakeX; x < snakeX + snakeW; x++) {
        Entity top("snake_border_top_" + std::to_string(x), Point(x, snakeY), snakeColor);
        engine.addEntity(top);
        Entity bot("snake_border_bot_" + std::to_string(x), Point(x, snakeY + snakeH - 1), snakeColor);
        engine.addEntity(bot);
    }
    
    for (int y = snakeY; y < snakeY + snakeH; y++) {
        Entity left("snake_border_left_" + std::to_string(y), Point(snakeX, y), snakeColor);
        engine.addEntity(left);
        Entity right("snake_border_right_" + std::to_string(y), Point(snakeX + snakeW - 1, y), snakeColor);
        engine.addEntity(right);
    }
    
    // Interior fill
    for (int x = snakeX + 1; x < snakeX + snakeW - 1; x++) {
        for (int y = snakeY + 1; y < snakeY + snakeH - 1; y++) {
            Entity fill("snake_fill_" + std::to_string(x) + "_" + std::to_string(y), 
                       Point(x, y), Color(20, 40, 60));
            engine.addEntity(fill);
        }
    }
    
    // Snake symbol inside (simple snake pattern)
    for (int i = 0; i < 4; i++) {
        Entity segment("snake_sym_" + std::to_string(i), 
                      Point(snakeX + 3 + i, snakeY + 5), snakeColor);
        engine.addEntity(segment);
    }
    
    // TETRIS (RIGHT side)
    Color tetrisColor = (selectedOption == 1) ? Color(255, 255, 0) : Color(0, 255, 255);
    
    int tetrisX = 23;
    int tetrisY = 7;
    int tetrisW = 15;
    int tetrisH = 10;
    
    // Border
    for (int x = tetrisX; x < tetrisX + tetrisW; x++) {
        Entity top("tetris_border_top_" + std::to_string(x), Point(x, tetrisY), tetrisColor);
        engine.addEntity(top);
        Entity bot("tetris_border_bot_" + std::to_string(x), Point(x, tetrisY + tetrisH - 1), tetrisColor);
        engine.addEntity(bot);
    }
    
    for (int y = tetrisY; y < tetrisY + tetrisH; y++) {
        Entity left("tetris_border_left_" + std::to_string(y), Point(tetrisX, y), tetrisColor);
        engine.addEntity(left);
        Entity right("tetris_border_right_" + std::to_string(y), Point(tetrisX + tetrisW - 1, y), tetrisColor);
        engine.addEntity(right);
    }
    
    // Interior fill
    for (int x = tetrisX + 1; x < tetrisX + tetrisW - 1; x++) {
        for (int y = tetrisY + 1; y < tetrisY + tetrisH - 1; y++) {
            Entity fill("tetris_fill_" + std::to_string(x) + "_" + std::to_string(y), 
                       Point(x, y), Color(20, 40, 60));
            engine.addEntity(fill);
        }
    }
    
    // Tetris symbol inside (T-shape)
    for (int i = 0; i < 3; i++) {
        Entity block("tetris_sym_horiz_" + std::to_string(i), 
                    Point(tetrisX + 4 + i, tetrisY + 5), tetrisColor);
        engine.addEntity(block);
    }
    Entity blockVert("tetris_sym_vert", Point(tetrisX + 5, tetrisY + 6), tetrisColor);
    engine.addEntity(blockVert);
    
    // ===== EXIT (BOTTOM CENTER) =====
    Color exitColor = (selectedOption == 2) ? Color(255, 255, 0) : Color(255, 100, 100);
    
    int exitX = 12;
    int exitY = 20;
    int exitW = 16;
    int exitH = 4;
    
    // Border
    for (int x = exitX; x < exitX + exitW; x++) {
        Entity top("exit_border_top_" + std::to_string(x), Point(x, exitY), exitColor);
        engine.addEntity(top);
        Entity bot("exit_border_bot_" + std::to_string(x), Point(x, exitY + exitH - 1), exitColor);
        engine.addEntity(bot);
    }
    
    for (int y = exitY; y < exitY + exitH; y++) {
        Entity left("exit_border_left_" + std::to_string(y), Point(exitX, y), exitColor);
        engine.addEntity(left);
        Entity right("exit_border_right_" + std::to_string(y), Point(exitX + exitW - 1, y), exitColor);
        engine.addEntity(right);
    }
    
    // Interior fill
    for (int x = exitX + 1; x < exitX + exitW - 1; x++) {
        for (int y = exitY + 1; y < exitY + exitH - 1; y++) {
            Entity fill("exit_fill_" + std::to_string(x) + "_" + std::to_string(y), 
                       Point(x, y), Color(20, 40, 60));
            engine.addEntity(fill);
        }
    }
    
    // Exit symbol (X)
    Entity exitX1("exit_sym_1", Point(exitX + 5, exitY + 1), exitColor);
    Entity exitX2("exit_sym_2", Point(exitX + 9, exitY + 1), exitColor);
    engine.addEntity(exitX1);
    engine.addEntity(exitX2);
    
    // Render
    engine.render();
}

GameSelection MainMenu::run() {
    const float FPS = 60.0f;
    const float FRAME_TIME = 1.0f / FPS;
    
    unsigned int lastTime = SDL_GetTicks();
    
    while (engine.isRunning() && !input.shouldQuit() && menuActive) {
        unsigned int currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        if (deltaTime > FRAME_TIME) {
            deltaTime = FRAME_TIME;
        }
        
        handleInput();
        drawMenu();
        
        // Check for selection
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_RETURN || 
                    event.key.keysym.sym == SDLK_SPACE) {
                    menuActive = false;
                    
                    if (selectedOption == 0) {
                        return GAME_SNAKE;
                    } else if (selectedOption == 1) {
                        return GAME_TETRIS;
                    } else {
                        return GAME_EXIT;
                    }
                }
            }
        }
    }
    
    return GAME_EXIT;
}
