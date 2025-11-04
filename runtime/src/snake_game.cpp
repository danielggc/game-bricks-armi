#include "snake_game.hpp"
#include "../../brickc/src/symbols.hpp"
#include <cstdlib>
#include <iostream>
#include <ctime>

// ============================================================================
// Snake Game Implementation
// ============================================================================

SnakeGame::SnakeGame(int gridWidth, int gridHeight, int cellSize)
    : engine(gridWidth, gridHeight, cellSize), useExternalWindow(false), useBrickFile(false),
      score(0), lives(3), gameSpeed(8), gameOver(false), paused(false),
      currentDirection(RIGHT), nextDirection(RIGHT),
      moveTimer(0.0f), moveDelay(1.0f / 8.0f),
      obstacleTimer(0.0f), obstacleDelay(2.0f), wallTimer(0.0f), wallDelay(3.0f) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

SnakeGame::SnakeGame(SDL_Window* window, SDL_Renderer* renderer, int gridWidth, int gridHeight, int cellSize)
    : engine(window, renderer, gridWidth, gridHeight, cellSize), useExternalWindow(true), useBrickFile(false),
      score(0), lives(3), gameSpeed(8), gameOver(false), paused(false),
      currentDirection(RIGHT), nextDirection(RIGHT),
      moveTimer(0.0f), moveDelay(1.0f / 8.0f),
      obstacleTimer(0.0f), obstacleDelay(2.0f), wallTimer(0.0f), wallDelay(3.0f) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

SnakeGame::SnakeGame(SDL_Window* window, SDL_Renderer* renderer, const std::string& brickFile, int cellSize)
    : engine(window, renderer, 20, 20, cellSize), useExternalWindow(true), useBrickFile(true),
      score(0), lives(3), gameSpeed(8), gameOver(false), paused(false),
      currentDirection(RIGHT), nextDirection(RIGHT),
      moveTimer(0.0f), moveDelay(1.0f / 8.0f),
      obstacleTimer(0.0f), obstacleDelay(2.0f), wallTimer(0.0f), wallDelay(3.0f) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    if (brickLoader.loadBrickFile(brickFile)) {
        engine = GameEngine(window, renderer, brickLoader.getGridWidth(), brickLoader.getGridHeight(), cellSize);
        gameSpeed = brickLoader.getSpeed();
        lives = brickLoader.getLives();
        score = brickLoader.getScore();
        moveDelay = 1.0f / gameSpeed;
        
        Color bgColor = Color::fromHex(brickLoader.getGameColor());
        engine.setBackgroundColor(bgColor);
    }
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
    loadEntitiesFromBrick();
    
    return true;
}

void SnakeGame::initializeGame() {
    snake.clear();
    
    if (useBrickFile && brickLoader.hasEntity("head")) {
        const brick::Entity* headEntity = brickLoader.findEntity("head");
        snake.push_back(SnakeSegment(headEntity->spawn.x, headEntity->spawn.y));
        snake.push_back(SnakeSegment(headEntity->spawn.x - 1, headEntity->spawn.y));
        snake.push_back(SnakeSegment(headEntity->spawn.x - 2, headEntity->spawn.y));
        std::cout << "Snake initialized at position from .brick: (" << headEntity->spawn.x << ", " << headEntity->spawn.y << ")" << std::endl;
    } else {
        snake.push_back(SnakeSegment(engine.getGridWidth() / 2, engine.getGridHeight() / 2));
        snake.push_back(SnakeSegment(engine.getGridWidth() / 2 - 1, engine.getGridHeight() / 2));
        snake.push_back(SnakeSegment(engine.getGridWidth() / 2 - 2, engine.getGridHeight() / 2));
    }
    
    if (useBrickFile && brickLoader.hasEntity("fruit")) {
        const brick::Entity* fruitEntity = brickLoader.findEntity("fruit");
        fruitPosition = Point(fruitEntity->spawn.x, fruitEntity->spawn.y);
        std::cout << "Fruit spawned at position from .brick: (" << fruitEntity->spawn.x << ", " << fruitEntity->spawn.y << ")" << std::endl;
        
        // Create the visual fruit entity
        Color fruitColor = Color::fromHex(fruitEntity->color);
        Entity fruitGameEntity("fruit", fruitPosition, fruitColor);
        engine.addEntity(fruitGameEntity);
    } else {
        spawnFruit();
    }
    
    if (useBrickFile && brickLoader.hasEntity("superfruit")) {
        const brick::Entity* superfruitEntity = brickLoader.findEntity("superfruit");
        superfruitPosition = Point(superfruitEntity->spawn.x, superfruitEntity->spawn.y);
        std::cout << "Superfruit spawned at position from .brick: (" << superfruitEntity->spawn.x << ", " << superfruitEntity->spawn.y << ")" << std::endl;
        
        // Create the visual superfruit entity
        Color superfruitColor = Color::fromHex(superfruitEntity->color);
        Entity superfruitGameEntity("superfruit", superfruitPosition, superfruitColor);
        engine.addEntity(superfruitGameEntity);
    } else {
        spawnSuperFruit();
    }
    
    // Initialize moving obstacles positions
    if (useBrickFile && brickLoader.hasEntity("obstacle")) {
        const brick::Entity* obstacleEntity = brickLoader.findEntity("obstacle");
        obstaclePosition = Point(obstacleEntity->spawn.x, obstacleEntity->spawn.y);
    } else {
        obstaclePosition = Point(12, 18); // Default position
    }
    
    if (useBrickFile && brickLoader.hasEntity("wall")) {
        const brick::Entity* wallEntity = brickLoader.findEntity("wall");
        wallPosition = Point(wallEntity->spawn.x, wallEntity->spawn.y);
    } else {
        wallPosition = Point(0, 0); // Default position
    }
    
    gameOver = false;
    paused = false;
    currentDirection = RIGHT;
    nextDirection = RIGHT;
    moveTimer = 0.0f;
    obstacleTimer = 0.0f;
    wallTimer = 0.0f;
}

void SnakeGame::spawnFruit() {
    bool validPosition = false;
    while (!validPosition) {
        fruitPosition = Point(
            std::rand() % engine.getGridWidth(),
            std::rand() % engine.getGridHeight()
        );
        validPosition = !isSnakeBody(fruitPosition) && 
                       fruitPosition != superfruitPosition &&
                       !isSolidEntity(fruitPosition);
    }
    
    // Create new fruit entity at the new position
    Color fruitColor = Color(255, 0, 0); // Red color
    if (useBrickFile && brickLoader.hasEntity("fruit")) {
        const brick::Entity* fruitBrickEntity = brickLoader.findEntity("fruit");
        fruitColor = Color::fromHex(fruitBrickEntity->color);
    }
    
    Entity fruitEntity("fruit", fruitPosition, fruitColor);
    engine.addEntity(fruitEntity);
}

void SnakeGame::spawnSuperFruit() {
    bool validPosition = false;
    while (!validPosition) {
        superfruitPosition = Point(
            std::rand() % engine.getGridWidth(),
            std::rand() % engine.getGridHeight()
        );
        validPosition = !isSnakeBody(superfruitPosition) && 
                       superfruitPosition != fruitPosition &&
                       !isSolidEntity(superfruitPosition);
    }
    
    // Create new superfruit entity at the new position
    Color superfruitColor = Color(255, 215, 0); // Gold color
    if (useBrickFile && brickLoader.hasEntity("superfruit")) {
        const brick::Entity* superfruitBrickEntity = brickLoader.findEntity("superfruit");
        superfruitColor = Color::fromHex(superfruitBrickEntity->color);
    }
    
    Entity superfruitEntity("superfruit", superfruitPosition, superfruitColor);
    engine.addEntity(superfruitEntity);
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

bool SnakeGame::isSolidEntity(const Point& pos) const {
    // Check moving obstacles at their current positions
    if (pos == obstaclePosition || pos == wallPosition) {
        return true;
    }
    
    if (!useBrickFile) return false;
    
    const auto& entities = brickLoader.getEntities();
    
    for (const auto& pair : entities) {
        const std::string& name = pair.first;
        const brick::Entity& brickEntity = pair.second;
        
        // Skip moving entities (they're handled above with dynamic positions)
        if (name == "obstacle" || name == "wall") {
            continue;
        }
        
        // Check if this entity is solid and at the given position
        if (brickEntity.solid && 
            brickEntity.spawn.x == pos.x && 
            brickEntity.spawn.y == pos.y) {
            
            // Skip snake-related entities (they're handled separately)
            if (name == "head" || name == "body") {
                continue;
            }
            
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
            // Clear visual snake entities and reset snake to middle
            clearSnakeEntities();
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
            // Clear visual snake entities and reset snake
            clearSnakeEntities();
            snake.clear();
            snake.push_back(SnakeSegment(engine.getGridWidth() / 2, engine.getGridHeight() / 2));
            snake.push_back(SnakeSegment(engine.getGridWidth() / 2 - 1, engine.getGridHeight() / 2));
            snake.push_back(SnakeSegment(engine.getGridWidth() / 2 - 2, engine.getGridHeight() / 2));
            currentDirection = RIGHT;
            nextDirection = RIGHT;
            return;
        }
        
        // Check collision with moving obstacles (different penalties)
        if (newHead == obstaclePosition) {
            // Obstacle (blue) - loses 1 life
            lives -= 1;
            std::cout << "Hit obstacle! Lost 1 life. Lives remaining: " << lives << std::endl;
            if (lives <= 0) {
                gameOver = true;
                handleGameOver();
                return;
            }
            // Clear visual snake entities and reset snake
            clearSnakeEntities();
            snake.clear();
            snake.push_back(SnakeSegment(engine.getGridWidth() / 2, engine.getGridHeight() / 2));
            snake.push_back(SnakeSegment(engine.getGridWidth() / 2 - 1, engine.getGridHeight() / 2));
            snake.push_back(SnakeSegment(engine.getGridWidth() / 2 - 2, engine.getGridHeight() / 2));
            currentDirection = RIGHT;
            nextDirection = RIGHT;
            return;
        } else if (newHead == wallPosition) {
            // Wall (gray) - loses 3 lives
            lives -= 3;
            std::cout << "Hit wall! Lost 3 lives. Lives remaining: " << lives << std::endl;
            if (lives <= 0) {
                gameOver = true;
                handleGameOver();
                return;
            }
            // Clear visual snake entities and reset snake
            clearSnakeEntities();
            snake.clear();
            snake.push_back(SnakeSegment(engine.getGridWidth() / 2, engine.getGridHeight() / 2));
            snake.push_back(SnakeSegment(engine.getGridWidth() / 2 - 1, engine.getGridHeight() / 2));
            snake.push_back(SnakeSegment(engine.getGridWidth() / 2 - 2, engine.getGridHeight() / 2));
            currentDirection = RIGHT;
            nextDirection = RIGHT;
            return;
        }
        
        // Check collision with other solid entities
        if (isSolidEntity(newHead)) {
            lives--;
            if (lives <= 0) {
                gameOver = true;
                handleGameOver();
                return;
            }
            // Clear visual snake entities and reset snake
            clearSnakeEntities();
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
            // Remove the fruit entity from the screen before spawning new one
            engine.removeEntity("fruit");
            spawnFruit();
            gameSpeed = std::min(15, gameSpeed + 1);
            moveDelay = 1.0f / gameSpeed;
        } else if (newHead == superfruitPosition) {
            score += 50;
            // Grow 3 segments
            for (int i = 0; i < 3; i++) {
                snake.push_back(snake.back());
            }
            // Remove the superfruit entity from the screen
            engine.removeEntity("superfruit");
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
    
    // Clear all previous snake entities before drawing new ones
    clearSnakeEntities();
    
    // Update snake entities
    for (int i = 0; i < snake.size(); i++) {
        std::string name = (i == 0) ? "snake_head" : "snake_body_" + std::to_string(i);
        
        Color color = (i == 0) ? Color(0, 255, 0) : Color(0, 136, 0);
        Entity segment(name, snake[i].position, color);
        engine.addEntity(segment);
    }
    
    // Ensure static entities are still present (reload if needed)
    ensureStaticEntities();
    
    engine.render();
}

void SnakeGame::handleGameOver() {
    std::cout << "\n=== GAME OVER ===" << std::endl;
    std::cout << "Final Score: " << score << std::endl;
    std::cout << "================\n" << std::endl;
}

void SnakeGame::loadEntitiesFromBrick() {
    if (!useBrickFile) return;
    
    const auto& entities = brickLoader.getEntities();
    
    std::cout << "Loading entities from .brick file:" << std::endl;
    for (const auto& pair : entities) {
        const std::string& name = pair.first;
        const brick::Entity& brickEntity = pair.second;
        
        std::cout << "  Entity: " << name << std::endl;
        std::cout << "    Spawn: (" << brickEntity.spawn.x << ", " << brickEntity.spawn.y << ")" << std::endl;
        std::cout << "    Color: 0x" << std::hex << brickEntity.color << std::dec << std::endl;
        std::cout << "    Solid: " << (brickEntity.solid ? "true" : "false") << std::endl;
        
        // Skip fruit and superfruit entities - they are managed dynamically
        if (name == "fruit" || name == "superfruit") {
            std::cout << "    Skipping " << name << " - managed dynamically" << std::endl;
            continue;
        }
        
        // Skip snake head and body entities - they are managed by the snake system
        if (name == "head" || name == "body") {
            std::cout << "    Skipping " << name << " - managed by snake system" << std::endl;
            continue;
        }
        
        // Skip moving obstacles - they are managed by the moving system
        if (name == "obstacle" || name == "wall") {
            std::cout << "    Skipping " << name << " - managed as moving obstacle" << std::endl;
            continue;
        }
        
        // Load static entities like obstacles and walls
        Color entityColor = Color::fromHex(brickEntity.color);
        Entity gameEntity(name, Point(brickEntity.spawn.x, brickEntity.spawn.y), entityColor);
        engine.addEntity(gameEntity);
        std::cout << "    Loaded " << name << " at (" << brickEntity.spawn.x << ", " << brickEntity.spawn.y << ")" << std::endl;
    }
}

void SnakeGame::updateMovingObstacles(float deltaTime) {
    if (paused) return;
    
    // Update obstacle movement
    obstacleTimer += deltaTime;
    if (obstacleTimer >= obstacleDelay) {
        obstacleTimer = 0.0f;
        moveObstacle();
    }
    
    // Update wall movement
    wallTimer += deltaTime;
    if (wallTimer >= wallDelay) {
        wallTimer = 0.0f;
        moveWall();
    }
}

void SnakeGame::moveObstacle() {
    // Move obstacle in a random direction
    int direction = std::rand() % 4;
    Point newPos = obstaclePosition;
    
    switch (direction) {
        case 0: newPos.y--; break; // UP
        case 1: newPos.y++; break; // DOWN
        case 2: newPos.x--; break; // LEFT
        case 3: newPos.x++; break; // RIGHT
    }
    
    // Check if new position is valid (within bounds and not colliding with snake)
    if (isValidPosition(newPos) && !isSnakeBody(newPos) && 
        newPos != fruitPosition && newPos != superfruitPosition && newPos != wallPosition) {
        obstaclePosition = newPos;
        
        // Update the visual entity
        engine.removeEntity("obstacle");
        if (useBrickFile && brickLoader.hasEntity("obstacle")) {
            const brick::Entity* obstacleEntity = brickLoader.findEntity("obstacle");
            Color obstacleColor = Color::fromHex(obstacleEntity->color);
            Entity obstacleGameEntity("obstacle", obstaclePosition, obstacleColor);
            engine.addEntity(obstacleGameEntity);
        }
    }
}

void SnakeGame::moveWall() {
    // Move wall in a different pattern (maybe circular or back and forth)
    int direction = std::rand() % 4;
    Point newPos = wallPosition;
    
    switch (direction) {
        case 0: newPos.y--; break; // UP
        case 1: newPos.y++; break; // DOWN
        case 2: newPos.x--; break; // LEFT
        case 3: newPos.x++; break; // RIGHT
    }
    
    // Check if new position is valid
    if (isValidPosition(newPos) && !isSnakeBody(newPos) && 
        newPos != fruitPosition && newPos != superfruitPosition && newPos != obstaclePosition) {
        wallPosition = newPos;
        
        // Update the visual entity
        engine.removeEntity("wall");
        if (useBrickFile && brickLoader.hasEntity("wall")) {
            const brick::Entity* wallEntity = brickLoader.findEntity("wall");
            Color wallColor = Color::fromHex(wallEntity->color);
            Entity wallGameEntity("wall", wallPosition, wallColor);
            engine.addEntity(wallGameEntity);
        }
    }
}

void SnakeGame::update(float deltaTime) {
    handleInput();
    updateSnake(deltaTime);
    updateMovingObstacles(deltaTime);
    checkCollisions();
}

void SnakeGame::clearSnakeEntities() {
    // Remove all snake-related entities from the game engine
    engine.removeEntity("snake_head");
    
    // Remove all snake body segments (we need to check a reasonable range)
    for (int i = 1; i < 1000; i++) { // 1000 should be more than enough for any snake length
        std::string bodyName = "snake_body_" + std::to_string(i);
        engine.removeEntity(bodyName);
    }
}

void SnakeGame::ensureStaticEntities() {
    if (!useBrickFile) return;
    
    const auto& entities = brickLoader.getEntities();
    
    for (const auto& pair : entities) {
        const std::string& name = pair.first;
        const brick::Entity& brickEntity = pair.second;
        
        // Only ensure static entities (obstacles, walls, etc.)
        if (name == "fruit" || name == "superfruit" || name == "head" || name == "body") {
            continue;
        }
        
        // Check if the entity exists in the engine, if not, add it
        Entity* existingEntity = engine.getEntity(name);
        if (!existingEntity) {
            Color entityColor = Color::fromHex(brickEntity.color);
            Entity gameEntity(name, Point(brickEntity.spawn.x, brickEntity.spawn.y), entityColor);
            engine.addEntity(gameEntity);
        }
    }
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

