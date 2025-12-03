#include "tank_game.hpp"
#include "../../brickc/src/symbols.hpp"
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <algorithm>

// ============================================================================
// Tank Game Implementation
// ============================================================================

TankGame::TankGame(int gridWidth, int gridHeight, int cellSize)
    : engine(gridWidth, gridHeight, cellSize), useExternalWindow(false), useBrickFile(false),
      score(0), lives(3), gameSpeed(1), gameOver(false), victory(false), paused(false),
      powerupActive(false), enemySpawnTimer(0.0f), enemySpawnDelay(8.0f),
      enemyMoveTimer(0.0f), enemyMoveDelay(2.0f), enemyShootTimer(0.0f), enemyShootDelay(4.0f),
      playerMoveTimer(0.0f), playerMoveDelay(0.2f) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

TankGame::TankGame(SDL_Window* window, SDL_Renderer* renderer, int gridWidth, int gridHeight, int cellSize)
    : engine(window, renderer, gridWidth, gridHeight, cellSize), useExternalWindow(true), useBrickFile(false),
      score(0), lives(3), gameSpeed(1), gameOver(false), victory(false), paused(false),
      powerupActive(false), enemySpawnTimer(0.0f), enemySpawnDelay(8.0f),
      enemyMoveTimer(0.0f), enemyMoveDelay(2.0f), enemyShootTimer(0.0f), enemyShootDelay(4.0f),
      playerMoveTimer(0.0f), playerMoveDelay(0.2f) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

TankGame::TankGame(SDL_Window* window, SDL_Renderer* renderer, const std::string& brickFile, int cellSize)
    : engine(window, renderer, 25, 20, cellSize), useExternalWindow(true), useBrickFile(true),
      score(0), lives(3), gameSpeed(1), gameOver(false), victory(false), paused(false),
      powerupActive(false), enemySpawnTimer(0.0f), enemySpawnDelay(8.0f),
      enemyMoveTimer(0.0f), enemyMoveDelay(2.0f), enemyShootTimer(0.0f), enemyShootDelay(4.0f),
      playerMoveTimer(0.0f), playerMoveDelay(0.2f) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    if (brickLoader.loadBrickFile(brickFile)) {
        engine = GameEngine(window, renderer, brickLoader.getGridWidth(), brickLoader.getGridHeight(), cellSize);
        gameSpeed = brickLoader.getSpeed();
        lives = brickLoader.getLives();
        score = brickLoader.getScore();
        
        Color bgColor = Color::fromHex(brickLoader.getGameColor());
        engine.setBackgroundColor(bgColor);
    }
}

TankGame::~TankGame() {
}

bool TankGame::initialize() {
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
    input.mapKeyToAction(SDLK_SPACE, "shoot");
    input.mapKeyToAction(SDLK_p, "pause");
    
    initializeGame();
    loadEntitiesFromBrick();
    
    return true;
}

void TankGame::initializeGame() {
    // Initialize player tank
    if (useBrickFile && brickLoader.hasEntity("player_tank")) {
        const brick::Entity* playerEntity = brickLoader.findEntity("player_tank");
        playerTank = Tank(playerEntity->spawn.x, playerEntity->spawn.y, true);
        std::cout << "Player tank initialized at position from .brick: (" << playerEntity->spawn.x << ", " << playerEntity->spawn.y << ")" << std::endl;
    } else {
        playerTank = Tank(2, engine.getGridHeight() - 2, true);
    }
    
    // Initialize enemy tanks
    enemyTanks.clear();
    if (useBrickFile && brickLoader.hasEntity("enemy_tank")) {
        const brick::Entity* enemyEntity = brickLoader.findEntity("enemy_tank");
        Tank enemy(enemyEntity->spawn.x, enemyEntity->spawn.y, false);
        enemyTanks.push_back(enemy);
        std::cout << "Enemy tank spawned at position from .brick: (" << enemyEntity->spawn.x << ", " << enemyEntity->spawn.y << ")" << std::endl;
    } else {
        Tank enemy(engine.getGridWidth() - 3, 1, false);
        enemyTanks.push_back(enemy);
    }
    
    // Initialize walls (create border walls)
    walls.clear();
    for (int x = 0; x < engine.getGridWidth(); x++) {
        walls.push_back(Point(x, 0)); // Top wall
        walls.push_back(Point(x, engine.getGridHeight() - 1)); // Bottom wall
    }
    for (int y = 0; y < engine.getGridHeight(); y++) {
        walls.push_back(Point(0, y)); // Left wall
        walls.push_back(Point(engine.getGridWidth() - 1, y)); // Right wall
    }
    
    // Add some internal walls
    for (int x = 5; x < 10; x++) {
        walls.push_back(Point(x, 5));
        walls.push_back(Point(x, engine.getGridHeight() - 6));
    }
    for (int x = 15; x < 20; x++) {
        walls.push_back(Point(x, 5));
        walls.push_back(Point(x, engine.getGridHeight() - 6));
    }
    
    // Initialize destructible walls
    destructibleWalls.clear();
    if (useBrickFile && brickLoader.hasEntity("destructible_wall")) {
        const brick::Entity* wallEntity = brickLoader.findEntity("destructible_wall");
        destructibleWalls.push_back(Point(wallEntity->spawn.x, wallEntity->spawn.y));
    } else {
        // Add some destructible walls in the middle
        for (int x = 10; x < 15; x++) {
            for (int y = 8; y < 12; y++) {
                destructibleWalls.push_back(Point(x, y));
            }
        }
    }
    
    // Initialize powerup
    if (useBrickFile && brickLoader.hasEntity("powerup")) {
        const brick::Entity* powerupEntity = brickLoader.findEntity("powerup");
        powerupPosition = Point(powerupEntity->spawn.x, powerupEntity->spawn.y);
        powerupActive = true;
    } else {
        spawnPowerup();
    }
    
    // Clear collections
    bullets.clear();
    explosions.clear();
    
    // Reset game state
    gameOver = false;
    victory = false;
    paused = false;
    enemySpawnTimer = 0.0f;
    enemyMoveTimer = 0.0f;
    enemyShootTimer = 0.0f;
}

void TankGame::spawnPowerup() {
    bool validPosition = false;
    while (!validPosition) {
        powerupPosition = Point(
            std::rand() % (engine.getGridWidth() - 2) + 1,
            std::rand() % (engine.getGridHeight() - 2) + 1
        );
        validPosition = !isWall(powerupPosition) && 
                       !isDestructibleWall(powerupPosition) &&
                       !isTankAt(powerupPosition);
    }
    powerupActive = true;
    
    // Create powerup entity
    Color powerupColor = Color(0, 255, 255); // Cyan
    if (useBrickFile && brickLoader.hasEntity("powerup")) {
        const brick::Entity* powerupBrickEntity = brickLoader.findEntity("powerup");
        powerupColor = Color::fromHex(powerupBrickEntity->color);
    }
    
    Entity powerupEntity("powerup", powerupPosition, powerupColor);
    engine.addEntity(powerupEntity);
}

void TankGame::spawnEnemyTank() {
    if (enemyTanks.size() >= 5) return; // Limit enemy count
    
    // Try to spawn at the edges
    std::vector<Point> spawnPoints;
    for (int x = 1; x < engine.getGridWidth() - 1; x++) {
        spawnPoints.push_back(Point(x, 1));
        spawnPoints.push_back(Point(x, engine.getGridHeight() - 2));
    }
    for (int y = 1; y < engine.getGridHeight() - 1; y++) {
        spawnPoints.push_back(Point(1, y));
        spawnPoints.push_back(Point(engine.getGridWidth() - 2, y));
    }
    
    // Find valid spawn point
    for (const Point& spawn : spawnPoints) {
        if (!isTankAt(spawn) && !isDestructibleWall(spawn)) {
            Tank enemy(spawn, false);
            enemyTanks.push_back(enemy);
            std::cout << "Enemy tank spawned at (" << spawn.x << ", " << spawn.y << ")" << std::endl;
            break;
        }
    }
}

bool TankGame::isValidPosition(const Point& pos) const {
    return pos.x >= 0 && pos.x < engine.getGridWidth() &&
           pos.y >= 0 && pos.y < engine.getGridHeight();
}

bool TankGame::isWall(const Point& pos) const {
    for (const Point& wall : walls) {
        if (wall.x == pos.x && wall.y == pos.y) {
            return true;
        }
    }
    return false;
}

bool TankGame::isDestructibleWall(const Point& pos) const {
    for (const Point& wall : destructibleWalls) {
        if (wall.x == pos.x && wall.y == pos.y) {
            return true;
        }
    }
    return false;
}

bool TankGame::isTankAt(const Point& pos) const {
    if (playerTank.position.x == pos.x && playerTank.position.y == pos.y && playerTank.isAlive) {
        return true;
    }
    for (const Tank& enemy : enemyTanks) {
        if (enemy.position.x == pos.x && enemy.position.y == pos.y && enemy.isAlive) {
            return true;
        }
    }
    return false;
}

void TankGame::destroyDestructibleWall(const Point& pos) {
    auto it = std::find_if(destructibleWalls.begin(), destructibleWalls.end(),
        [&pos](const Point& wall) { return wall.x == pos.x && wall.y == pos.y; });
    
    if (it != destructibleWalls.end()) {
        destructibleWalls.erase(it);
        engine.removeEntity("destructible_wall_" + std::to_string(pos.x) + "_" + std::to_string(pos.y));
        createExplosion(pos);
    }
}

void TankGame::handleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        input.handleEvent(event);
    }
    
    input.update();
    
    if (input.isActionActive("pause")) {
        paused = !paused;
        SDL_Delay(200); // Debounce
    }
    
    if (paused) return;
    
    if (input.isActionActive("shoot")) {
        shootBullet(playerTank);
    }
}

void TankGame::updatePlayerMovement(float deltaTime) {
    if (paused) return;
    
    playerMoveTimer += deltaTime;
    
    if (playerMoveTimer >= playerMoveDelay) {
        // Handle tank movement
        if (input.isActionActive("up")) {
            moveTank(playerTank, TANK_UP);
            playerMoveTimer = 0.0f;
        } else if (input.isActionActive("down")) {
            moveTank(playerTank, TANK_DOWN);
            playerMoveTimer = 0.0f;
        } else if (input.isActionActive("left")) {
            moveTank(playerTank, TANK_LEFT);
            playerMoveTimer = 0.0f;
        } else if (input.isActionActive("right")) {
            moveTank(playerTank, TANK_RIGHT);
            playerMoveTimer = 0.0f;
        }
    }
}

void TankGame::moveTank(Tank& tank, TankDirection direction) {
    tank.direction = direction;
    Point newPos = tank.position;
    
    switch (direction) {
        case TANK_UP:
            newPos.y--;
            break;
        case TANK_DOWN:
            newPos.y++;
            break;
        case TANK_LEFT:
            newPos.x--;
            break;
        case TANK_RIGHT:
            newPos.x++;
            break;
        default:
            return;
    }
    
    // Check if new position is valid
    if (isValidPosition(newPos) && !isWall(newPos) && !isDestructibleWall(newPos) && !isTankAt(newPos)) {
        tank.position = newPos;
    }
}

void TankGame::shootBullet(const Tank& tank) {
    if (tank.shootCooldown > 0.0f) return;
    
    Point bulletPos = tank.position;
    Point offset = getDirectionOffset(tank.direction);
    bulletPos.x += offset.x;
    bulletPos.y += offset.y;
    
    if (isValidPosition(bulletPos)) {
        Bullet bullet(bulletPos, tank.direction, tank.isPlayer);
        bullets.push_back(bullet);
        
        if (tank.isPlayer) {
            std::cout << "Player shot bullet at (" << bulletPos.x << ", " << bulletPos.y << ")" << std::endl;
        }
        
        // Set cooldown
        const_cast<Tank&>(tank).shootCooldown = 0.3f;
    }
}

Point TankGame::getDirectionOffset(TankDirection direction) const {
    switch (direction) {
        case TANK_UP: return Point(0, -1);
        case TANK_DOWN: return Point(0, 1);
        case TANK_LEFT: return Point(-1, 0);
        case TANK_RIGHT: return Point(1, 0);
        default: return Point(0, 0);
    }
}

TankDirection TankGame::getRandomDirection() const {
    int dir = std::rand() % 4;
    switch (dir) {
        case 0: return TANK_UP;
        case 1: return TANK_DOWN;
        case 2: return TANK_LEFT;
        case 3: return TANK_RIGHT;
        default: return TANK_UP;
    }
}

void TankGame::createExplosion(const Point& position) {
    Explosion explosion(position);
    explosions.push_back(explosion);
}

void TankGame::updateTanks(float deltaTime) {
    if (paused) return;
    
    // Update cooldowns
    if (playerTank.shootCooldown > 0.0f) {
        playerTank.shootCooldown -= deltaTime;
    }
    
    for (Tank& enemy : enemyTanks) {
        if (enemy.shootCooldown > 0.0f) {
            enemy.shootCooldown -= deltaTime;
        }
    }
}

void TankGame::updateBullets(float deltaTime) {
    if (paused) return;
    
    for (auto it = bullets.begin(); it != bullets.end();) {
        Bullet& bullet = *it;
        
        if (!bullet.isActive) {
            it = bullets.erase(it);
            continue;
        }
        
        // Update bullet timer
        bullet.moveTimer += deltaTime;
        
        // Only move bullet when timer expires
        if (bullet.moveTimer < bullet.moveDelay) {
            ++it;
            continue;
        }
        
        bullet.moveTimer = 0.0f;
        
        // Move bullet
        Point offset = getDirectionOffset(bullet.direction);
        Point newPos = bullet.position;
        newPos.x += offset.x;
        newPos.y += offset.y;
        
        std::cout << "Moving bullet from (" << bullet.position.x << ", " << bullet.position.y << ") to (" << newPos.x << ", " << newPos.y << ")" << std::endl;
        
        // Check boundaries
        if (!isValidPosition(newPos)) {
            bullet.isActive = false;
            it = bullets.erase(it);
            continue;
        }
        
        // Check wall collision
        if (isWall(newPos)) {
            bullet.isActive = false;
            it = bullets.erase(it);
            continue;
        }
        
        // Check destructible wall collision
        if (isDestructibleWall(newPos)) {
            destroyDestructibleWall(newPos);
            bullet.isActive = false;
            it = bullets.erase(it);
            continue;
        }
        
        // Check tank collision
        bool hitTank = false;
        
        // Check player tank collision (enemy bullets)
        if (!bullet.isPlayerBullet && playerTank.position.x == newPos.x && playerTank.position.y == newPos.y && playerTank.isAlive) {
            lives--;
            createExplosion(playerTank.position);
            std::cout << "Player hit! Lives remaining: " << lives << std::endl;
            
            if (lives <= 0) {
                gameOver = true;
            }
            
            bullet.isActive = false;
            hitTank = true;
        }
        
        // Check enemy tank collision (player bullets)
        if (bullet.isPlayerBullet) {
            std::cout << "Checking bullet collision at (" << newPos.x << ", " << newPos.y << ") with " << enemyTanks.size() << " enemies" << std::endl;
            for (auto enemyIt = enemyTanks.begin(); enemyIt != enemyTanks.end(); ++enemyIt) {
                std::cout << "  Enemy at (" << enemyIt->position.x << ", " << enemyIt->position.y << ") alive: " << enemyIt->isAlive << std::endl;
                if (enemyIt->position.x == newPos.x && enemyIt->position.y == newPos.y && enemyIt->isAlive) {
                    score += 100;
                    createExplosion(enemyIt->position);
                    std::cout << "*** ENEMY DESTROYED at (" << newPos.x << ", " << newPos.y << ")! Score: " << score << " ***" << std::endl;
                    enemyIt->isAlive = false;
                    enemyTanks.erase(enemyIt);
                    
                    bullet.isActive = false;
                    hitTank = true;
                    break;
                }
            }
        }
        
        if (hitTank) {
            it = bullets.erase(it);
            continue;
        }
        
        bullet.position = newPos;
        ++it;
    }
}

void TankGame::updateExplosions(float deltaTime) {
    if (paused) return;
    
    for (auto it = explosions.begin(); it != explosions.end();) {
        Explosion& explosion = *it;
        explosion.timer -= deltaTime;
        
        if (explosion.timer <= 0.0f) {
            explosion.isActive = false;
            it = explosions.erase(it);
        } else {
            ++it;
        }
    }
}

void TankGame::updateEnemyAI(float deltaTime) {
    if (paused) return;
    
    // Enemy spawning
    enemySpawnTimer += deltaTime;
    if (enemySpawnTimer >= enemySpawnDelay && enemyTanks.size() < 3) {
        enemySpawnTimer = 0.0f;
        spawnEnemyTank();
    }
    
    // Enemy movement
    enemyMoveTimer += deltaTime;
    if (enemyMoveTimer >= enemyMoveDelay) {
        enemyMoveTimer = 0.0f;
        
        for (Tank& enemy : enemyTanks) {
            if (!enemy.isAlive) continue;
            
            // Simple AI: move towards player or random direction
            TankDirection newDirection = enemy.direction;
            
            if (std::rand() % 3 == 0) { // 33% chance to change direction towards player
                if (playerTank.position.x < enemy.position.x) {
                    newDirection = TANK_LEFT;
                } else if (playerTank.position.x > enemy.position.x) {
                    newDirection = TANK_RIGHT;
                } else if (playerTank.position.y < enemy.position.y) {
                    newDirection = TANK_UP;
                } else if (playerTank.position.y > enemy.position.y) {
                    newDirection = TANK_DOWN;
                }
            } else { // Random direction
                newDirection = getRandomDirection();
            }
            
            moveTank(enemy, newDirection);
        }
    }
    
    // Enemy shooting
    enemyShootTimer += deltaTime;
    if (enemyShootTimer >= enemyShootDelay) {
        enemyShootTimer = 0.0f;
        
        for (Tank& enemy : enemyTanks) {
            if (!enemy.isAlive) continue;
            
            if (std::rand() % 2 == 0) { // 50% chance to shoot
                shootBullet(enemy);
            }
        }
    }
}

void TankGame::checkCollisions() {
    if (paused) return;
    
    // Check player tank collision with powerup
    if (powerupActive && playerTank.position.x == powerupPosition.x && playerTank.position.y == powerupPosition.y) {
        score += 50;
        lives++;
        powerupActive = false;
        engine.removeEntity("powerup");
        std::cout << "Powerup collected! Lives: " << lives << ", Score: " << score << std::endl;
        
        // Spawn new powerup after some time
        spawnPowerup();
    }
    
    // Check player tank collision with enemy tanks
    for (const Tank& enemy : enemyTanks) {
        if (enemy.isAlive && playerTank.position.x == enemy.position.x && playerTank.position.y == enemy.position.y) {
            lives--;
            createExplosion(playerTank.position);
            std::cout << "Tank collision! Lives remaining: " << lives << std::endl;
            
            if (lives <= 0) {
                gameOver = true;
            }
            break;
        }
    }
    
    // Check victory condition
    if (score >= 1000) {
        victory = true;
    }
}

void TankGame::drawGame() {
    engine.update(0.016f);
    
    // Clear all previous game entities
    clearGameEntities();
    
    // Draw player tank
    if (playerTank.isAlive) {
        Color playerColor = Color(0, 255, 0); // Green
        if (useBrickFile && brickLoader.hasEntity("player_tank")) {
            const brick::Entity* playerEntity = brickLoader.findEntity("player_tank");
            playerColor = Color::fromHex(playerEntity->color);
        }
        Entity playerEntity("player_tank", playerTank.position, playerColor);
        engine.addEntity(playerEntity);
    }
    
    // Draw enemy tanks
    Color enemyColor = Color(255, 0, 0); // Red
    if (useBrickFile && brickLoader.hasEntity("enemy_tank")) {
        const brick::Entity* enemyEntity = brickLoader.findEntity("enemy_tank");
        enemyColor = Color::fromHex(enemyEntity->color);
    }
    
    for (int i = 0; i < enemyTanks.size(); i++) {
        if (enemyTanks[i].isAlive) {
            std::string name = "enemy_tank_" + std::to_string(i);
            Entity enemyEntity(name, enemyTanks[i].position, enemyColor);
            engine.addEntity(enemyEntity);
        }
    }
    
    // Draw bullets
    Color bulletColor = Color(255, 255, 0); // Yellow
    Color enemyBulletColor = Color(255, 136, 0); // Orange
    
    if (useBrickFile) {
        if (brickLoader.hasEntity("bullet")) {
            const brick::Entity* bulletEntity = brickLoader.findEntity("bullet");
            bulletColor = Color::fromHex(bulletEntity->color);
        }
        if (brickLoader.hasEntity("enemy_bullet")) {
            const brick::Entity* enemyBulletEntity = brickLoader.findEntity("enemy_bullet");
            enemyBulletColor = Color::fromHex(enemyBulletEntity->color);
        }
    }
    
    for (int i = 0; i < bullets.size(); i++) {
        if (bullets[i].isActive) {
            std::string name = "bullet_" + std::to_string(i);
            Color color = bullets[i].isPlayerBullet ? bulletColor : enemyBulletColor;
            Entity bulletEntity(name, bullets[i].position, color);
            engine.addEntity(bulletEntity);
        }
    }
    
    // Draw explosions
    Color explosionColor = Color(255, 69, 0); // Orange-red
    if (useBrickFile && brickLoader.hasEntity("explosion")) {
        const brick::Entity* explosionEntity = brickLoader.findEntity("explosion");
        explosionColor = Color::fromHex(explosionEntity->color);
    }
    
    for (int i = 0; i < explosions.size(); i++) {
        if (explosions[i].isActive) {
            std::string name = "explosion_" + std::to_string(i);
            Entity explosionEntity(name, explosions[i].position, explosionColor);
            engine.addEntity(explosionEntity);
        }
    }
    
    // Ensure static entities are present
    ensureStaticEntities();
    
    engine.render();
}

void TankGame::handleGameOver() {
    if (victory) {
        std::cout << "\n=== VICTORY! ===" << std::endl;
        std::cout << "You achieved victory with score: " << score << std::endl;
        std::cout << "===============\n" << std::endl;
    } else {
        std::cout << "\n=== GAME OVER ===" << std::endl;
        std::cout << "Final Score: " << score << std::endl;
        std::cout << "================\n" << std::endl;
    }
}

void TankGame::loadEntitiesFromBrick() {
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
        
        // Skip dynamic entities - they are managed by the game systems
        if (name == "player_tank" || name == "enemy_tank" || name == "bullet" || 
            name == "enemy_bullet" || name == "explosion" || name == "powerup") {
            std::cout << "    Skipping " << name << " - managed dynamically" << std::endl;
            continue;
        }
        
        // Load static entities like walls
        if (name == "wall" || name == "destructible_wall") {
            std::cout << "    Skipping " << name << " - managed by wall system" << std::endl;
            continue;
        }
        
        // Load other static entities
        Color entityColor = Color::fromHex(brickEntity.color);
        Entity gameEntity(name, Point(brickEntity.spawn.x, brickEntity.spawn.y), entityColor);
        engine.addEntity(gameEntity);
        std::cout << "    Loaded " << name << " at (" << brickEntity.spawn.x << ", " << brickEntity.spawn.y << ")" << std::endl;
    }
}

void TankGame::clearGameEntities() {
    // Remove all dynamic entities
    engine.removeEntity("player_tank");
    
    // Remove enemy tanks
    for (int i = 0; i < 20; i++) { // Reasonable upper limit
        std::string name = "enemy_tank_" + std::to_string(i);
        engine.removeEntity(name);
    }
    
    // Remove bullets
    for (int i = 0; i < 100; i++) { // Reasonable upper limit
        std::string name = "bullet_" + std::to_string(i);
        engine.removeEntity(name);
    }
    
    // Remove explosions
    for (int i = 0; i < 50; i++) { // Reasonable upper limit
        std::string name = "explosion_" + std::to_string(i);
        engine.removeEntity(name);
    }
}

void TankGame::ensureStaticEntities() {
    // Draw walls
    Color wallColor = Color(139, 69, 19); // Brown
    if (useBrickFile && brickLoader.hasEntity("wall")) {
        const brick::Entity* wallEntity = brickLoader.findEntity("wall");
        wallColor = Color::fromHex(wallEntity->color);
    }
    
    for (int i = 0; i < walls.size(); i++) {
        std::string name = "wall_" + std::to_string(walls[i].x) + "_" + std::to_string(walls[i].y);
        Entity wallEntity(name, walls[i], wallColor);
        engine.addEntity(wallEntity);
    }
    
    // Draw destructible walls
    Color destructibleWallColor = Color(205, 133, 63); // Peru
    if (useBrickFile && brickLoader.hasEntity("destructible_wall")) {
        const brick::Entity* destructibleWallEntity = brickLoader.findEntity("destructible_wall");
        destructibleWallColor = Color::fromHex(destructibleWallEntity->color);
    }
    
    for (int i = 0; i < destructibleWalls.size(); i++) {
        std::string name = "destructible_wall_" + std::to_string(destructibleWalls[i].x) + "_" + std::to_string(destructibleWalls[i].y);
        Entity wallEntity(name, destructibleWalls[i], destructibleWallColor);
        engine.addEntity(wallEntity);
    }
    
    // Draw powerup if active
    if (powerupActive) {
        Color powerupColor = Color(0, 255, 255); // Cyan
        if (useBrickFile && brickLoader.hasEntity("powerup")) {
            const brick::Entity* powerupEntity = brickLoader.findEntity("powerup");
            powerupColor = Color::fromHex(powerupEntity->color);
        }
        Entity powerupEntity("powerup", powerupPosition, powerupColor);
        engine.addEntity(powerupEntity);
    }
}

void TankGame::update(float deltaTime) {
    handleInput();
    updatePlayerMovement(deltaTime);
    updateTanks(deltaTime);
    updateBullets(deltaTime);
    updateExplosions(deltaTime);
    updateEnemyAI(deltaTime);
    checkCollisions();
}

void TankGame::run() {
    const float FPS = 60.0f;
    const float FRAME_TIME = 1.0f / FPS;
    
    unsigned int lastTime = SDL_GetTicks();
    
    while (engine.isRunning() && !input.shouldQuit() && !gameOver && !victory) {
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
