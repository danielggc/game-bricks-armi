#include "game_engine.hpp"
#include "renderer.hpp"
#include <iostream>

// ============================================================================
// Game Engine Implementation
// ============================================================================

GameEngine::GameEngine(int gridWidth, int gridHeight, int cellSize)
    : gridWidth(gridWidth), gridHeight(gridHeight), cellSize(cellSize),
      gameSpeed(8), running(false), window(nullptr), renderer(nullptr),
      backgroundColor(0, 17, 34) {
}

GameEngine::~GameEngine() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

bool GameEngine::initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Error: SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    int windowWidth = gridWidth * cellSize;
    int windowHeight = gridHeight * cellSize;
    
    window = SDL_CreateWindow(
        "Brick Snake Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_SHOWN
    );
    
    if (!window) {
        std::cerr << "Error: Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Error: Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    running = true;
    
    return true;
}

void GameEngine::addEntity(const Entity& entity) {
    entities[entity.name] = entity;
}

void GameEngine::removeEntity(const std::string& name) {
    entities.erase(name);
}

Entity* GameEngine::getEntity(const std::string& name) {
    auto it = entities.find(name);
    if (it != entities.end()) {
        return &it->second;
    }
    return nullptr;
}

void GameEngine::update(float deltaTime) {
    // Game logic updates happen here
}

void GameEngine::render() {
    Renderer::clear(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b);
    
    renderGrid();
    renderEntities();
    
    Renderer::present(renderer);
}

void GameEngine::renderGrid() {
    // Draw grid lines
    Uint8 gridR = backgroundColor.r + 20;
    Uint8 gridG = backgroundColor.g + 20;
    Uint8 gridB = backgroundColor.b + 20;
    
    // Vertical lines
    for (int x = 0; x <= gridWidth; x++) {
        Renderer::drawLine(renderer, x * cellSize, 0, x * cellSize, gridHeight * cellSize,
                          gridR, gridG, gridB, 100);
    }
    
    // Horizontal lines
    for (int y = 0; y <= gridHeight; y++) {
        Renderer::drawLine(renderer, 0, y * cellSize, gridWidth * cellSize, y * cellSize,
                          gridR, gridG, gridB, 100);
    }
}

void GameEngine::renderEntities() {
    for (auto& pair : entities) {
        Entity& entity = pair.second;
        
        if (!entity.visible) continue;
        
        int pixelX = entity.position.x * cellSize + cellSize / 2;
        int pixelY = entity.position.y * cellSize + cellSize / 2;
        int pixelW = entity.width * cellSize;
        int pixelH = entity.height * cellSize;
        
        Renderer::fillRect(renderer, 
                          pixelX - pixelW / 2, 
                          pixelY - pixelH / 2, 
                          pixelW, 
                          pixelH,
                          entity.color.r, 
                          entity.color.g, 
                          entity.color.b, 
                          entity.color.a);
    }
}

