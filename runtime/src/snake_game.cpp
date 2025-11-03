#include "snake_game.hpp"
#include <cstdlib>
#include <iostream>
#include <ctime>

// ============================================================================
// Snake Game Implementation
// ============================================================================

SnakeGame::SnakeGame(int gridWidth, int gridHeight, int cellSize)
    : engine(gridWidth, gridHeight, cellSize),
      score(0), lives(3), gameSpeed(8), gameOver(false), paused(false),
      currentDirection(RIGHT), nextDirection(RIGHT),
      moveTimer(0.0f), moveDelay(1.0f / 8.0f) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

SnakeGame::~SnakeGame() {
}

bool SnakeGame::initialize() {
    if (!engine.initialize()) {
        return false;
    }
    
    // Map controls
    input.mapKeyToAction(SDLK_UP, "up");
    input.mapKeyToAction(SDLK_w, "up");
    input.mapKeyToAction(SDLK_DOWN, "down");
    input.mapKeyToAction(SDLK_s, "down");
    input.mapKeyToAction(SDLK_LEFT, "left");
    input.mapKeyToAction(SDLK_a, "left");
    input.mapKeyToAction(SDLK_RIGHT, "right");
    input.mapKeyToAction(SDLK_d, "right");
    input.mapKeyToAction(SDLK_SPACE, "pause");
    
    initializeGame();
    
    return true;
}

void SnakeGame::initializeGame() {
    // Clear snake
    snake.clear();
    
    // Initialize snake with 3 segments
    snake.push_back(SnakeSegment(engine.getGridWidth() / 2, engine.getGridHeight() / 2));
    snake.push_back(SnakeSegment(engine.getGridWidth() / 2 - 1, engine.getGridHeight() / 2));
    snake.push_back(SnakeSegment(engine.getGridWidth() / 2 - 2, engine.getGridHeight() / 2));
    
    // Spawn fruit
    spawnFruit();
    spawnSuperFruit();
    
    // Reset game state
    score = 0;
    lives = 3;
    gameSpeed = 8;
    gameOver = false;
    paused = false;
    currentDirection = RIGHT;
    nextDirection = RIGHT;
    moveTimer = 0.0f;
    moveDelay = 1.0f / gameSpeed;
    
    // Add entities to engine
    engine.addEntity(Entity("fruit", fruitPosition, Color(255, 0, 0)));
    engine.addEntity(Entity("superfruit", superfruitPosition, Color(255, 215, 0)));
}

void SnakeGame::spawnFruit() {
    bool validPosition = false;
    while (!validPosition) {
        fruitPosition = Point(
            std::rand() % engine.getGridWidth(),
            std::rand() % engine.getGridHeight()
        );
        validPosition = !isSnakeBody(fruitPosition) && fruitPosition != superfruitPosition;
    }
    
    Entity* fruitEntity = engine.getEntity("fruit");
    if (fruitEntity) {
        fruitEntity->position = fruitPosition;
    }
}

void SnakeGame::spawnSuperFruit() {
    bool validPosition = false;
    while (!validPosition) {
        superfruitPosition = Point(
            std::rand() % engine.getGridWidth(),
            std::rand() % engine.getGridHeight()
        );
        validPosition = !isSnakeBody(superfruitPosition) && superfruitPosition != fruitPosition;
    }
    
    Entity* superfruit = engine.getEntity("superfruit");
    if (superfruit) {
        superfruit->position = superfruitPosition;
    }
}

bool SnakeGame::isValidPosition(const Point& pos) const {
    return pos.x >= 0 && pos.x < engine.getGridWidth() &&
           pos.y >= 0 && pos.y < engine.getGridHeight();
}

bool SnakeGame::isSnakeBody(const Point& pos) const {
    for (const auto& segment : snake) {
        if (segment.position == pos) {
            return true;
        }
    }
    return false;
}

void SnakeGame::handleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        input.handleEvent(event);
    }
    
    input.update();
    
    // Handle direction changes (prevent 180-degree turns)
    if (input.isActionActive("up") && currentDirection != DOWN) {
        nextDirection = UP;
    } else if (input.isActionActive("down") && currentDirection != UP) {
        nextDirection = DOWN;
    } else if (input.isActionActive("left") && currentDirection != RIGHT) {
        nextDirection = LEFT;
    } else if (input.isActionActive("right") && currentDirection != LEFT) {
        nextDirection = RIGHT;
    }
    
    if (input.isActionActive("pause")) {
        paused = !paused;
        SDL_Delay(200); // Debounce
    }
}

void SnakeGame::updateSnake(float deltaTime) {
    if (paused) return;
    
    moveTimer += deltaTime;
    
    if (moveTimer >= moveDelay) {
        moveTimer = 0.0f;
        currentDirection = nextDirection;
        
        // Calculate new head position
        Point newHead = snake.front().position;
        
        switch (currentDirection) {
            case UP:
                newHead.y--;
                break;
            case DOWN:
                newHead.y++;
                break;
            case LEFT:
                newHead.x--;
                break;
            case RIGHT:
                newHead.x++;
                break;
            default:
                break;
        }
        
        // Check wall collision
        if (!isValidPosition(newHead)) {
            lives--;
            if (lives <= 0) {
                gameOver = true;
                handleGameOver();
                return;
            }
            // Reset snake to middle
            snake.clear();
            snake.push_back(SnakeSegment(engine.getGridWidth() / 2, engine.getGridHeight() / 2));
            snake.push_back(SnakeSegment(engine.getGridWidth() / 2 - 1, engine.getGridHeight() / 2));
            snake.push_back(SnakeSegment(engine.getGridWidth() / 2 - 2, engine.getGridHeight() / 2));
            currentDirection = RIGHT;
            nextDirection = RIGHT;
            return;
        }
        
        // Check self collision
        if (isSnakeBody(newHead)) {
            lives--;
            if (lives <= 0) {
                gameOver = true;
                handleGameOver();
                return;
            }
            // Reset snake
            snake.clear();
            snake.push_back(SnakeSegment(engine.getGridWidth() / 2, engine.getGridHeight() / 2));
            snake.push_back(SnakeSegment(engine.getGridWidth() / 2 - 1, engine.getGridHeight() / 2));
            snake.push_back(SnakeSegment(engine.getGridWidth() / 2 - 2, engine.getGridHeight() / 2));
            currentDirection = RIGHT;
            nextDirection = RIGHT;
            return;
        }
        
        // Move snake
        snake.push_front(SnakeSegment(newHead));
        
        // Check fruit collision
        if (newHead == fruitPosition) {
            score += 10;
            spawnFruit();
            gameSpeed = std::min(15, gameSpeed + 1);
            moveDelay = 1.0f / gameSpeed;
        } else if (newHead == superfruitPosition) {
            score += 50;
            // Grow 3 segments
            for (int i = 0; i < 3; i++) {
                snake.push_back(snake.back());
            }
            spawnSuperFruit();
        } else {
            // Normal movement - remove tail
            snake.pop_back();
        }
    }
}

void SnakeGame::checkCollisions() {
    // Collisions are checked in updateSnake
}

void SnakeGame::drawGame() {
    engine.update(0.016f);
    
    // Update snake entities
    engine.removeEntity("snake_head");
    for (int i = 0; i < snake.size(); i++) {
        std::string name = (i == 0) ? "snake_head" : "snake_body_" + std::to_string(i);
        
        Color color = (i == 0) ? Color(0, 255, 0) : Color(0, 136, 0);
        Entity segment(name, snake[i].position, color);
        engine.addEntity(segment);
    }
    
    engine.render();
}

void SnakeGame::handleGameOver() {
    std::cout << "\n=== GAME OVER ===" << std::endl;
    std::cout << "Final Score: " << score << std::endl;
    std::cout << "================\n" << std::endl;
}

void SnakeGame::update(float deltaTime) {
    handleInput();
    updateSnake(deltaTime);
    checkCollisions();
}

void SnakeGame::run() {
    const float FPS = 60.0f;
    const float FRAME_TIME = 1.0f / FPS;
    
    unsigned int lastTime = SDL_GetTicks();
    
    while (engine.isRunning() && !input.shouldQuit() && !gameOver) {
        unsigned int currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        // Cap deltaTime to avoid large jumps
        if (deltaTime > FRAME_TIME) {
            deltaTime = FRAME_TIME;
        }
        
        update(deltaTime);
        drawGame();
    }
    
    handleGameOver();
}

