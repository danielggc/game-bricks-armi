#ifndef TANK_GAME_HPP
#define TANK_GAME_HPP

#include "game_engine.hpp"
#include "input_manager.hpp"
#include "brick_loader.hpp"
#include <vector>
#include <ctime>

// ============================================================================
// Tank Game - Game logic and state
// ============================================================================

enum TankDirection {
    TANK_NONE = 0,
    TANK_UP = 1,
    TANK_DOWN = 2,
    TANK_LEFT = 3,
    TANK_RIGHT = 4
};

struct Tank {
    Point position;
    TankDirection direction;
    bool isPlayer;
    float shootCooldown;
    bool isAlive;
    
    Tank(int x = 0, int y = 0, bool player = false) 
        : position(x, y), direction(TANK_UP), isPlayer(player), 
          shootCooldown(0.0f), isAlive(true) {}
    Tank(const Point& pos, bool player = false) 
        : position(pos), direction(TANK_UP), isPlayer(player), 
          shootCooldown(0.0f), isAlive(true) {}
};

struct Bullet {
    Point position;
    TankDirection direction;
    bool isPlayerBullet;
    float speed;
    bool isActive;
    float moveTimer;
    float moveDelay;
    
    Bullet(const Point& pos, TankDirection dir, bool playerBullet = true)
        : position(pos), direction(dir), isPlayerBullet(playerBullet), 
          speed(3.0f), isActive(true), moveTimer(0.0f), moveDelay(0.15f) {}
};

struct Explosion {
    Point position;
    float timer;
    bool isActive;
    
    Explosion(const Point& pos) : position(pos), timer(0.5f), isActive(true) {}
};

class TankGame {
public:
    TankGame(int gridWidth, int gridHeight, int cellSize = 20);
    TankGame(SDL_Window* window, SDL_Renderer* renderer, int gridWidth, int gridHeight, int cellSize = 20);
    TankGame(SDL_Window* window, SDL_Renderer* renderer, const std::string& brickFile, int cellSize = 20);
    ~TankGame();
    
    bool initialize();
    void run();
    void update(float deltaTime);
    void handleInput();
    
    // Game state
    int getScore() const { return score; }
    int getLives() const { return lives; }
    bool isGameOver() const { return gameOver; }
    bool isPaused() const { return paused; }
    bool isVictory() const { return victory; }
    
    void setSpeed(int speed) { gameSpeed = speed; }
    void setLives(int newLives) { lives = newLives; }
    void setScore(int newScore) { score = newScore; }
    void setBackgroundColor(unsigned int color) { 
        Color bgColor = Color::fromHex(color);
        engine.setBackgroundColor(bgColor);
    }
    
private:
    GameEngine engine;
    InputManager input;
    BrickLoader brickLoader;
    bool useExternalWindow;
    bool useBrickFile;
    
    Tank playerTank;
    std::vector<Tank> enemyTanks;
    std::vector<Bullet> bullets;
    std::vector<Explosion> explosions;
    std::vector<Point> walls;
    std::vector<Point> destructibleWalls;
    Point powerupPosition;
    bool powerupActive;
    
    int score;
    int lives;
    int gameSpeed;
    bool gameOver;
    bool victory;
    bool paused;
    
    float enemySpawnTimer;
    float enemySpawnDelay;
    float enemyMoveTimer;
    float enemyMoveDelay;
    float enemyShootTimer;
    float enemyShootDelay;
    float playerMoveTimer;
    float playerMoveDelay;
    
    // Game logic
    void initializeGame();
    void updateTanks(float deltaTime);
    void updatePlayerMovement(float deltaTime);
    void updateBullets(float deltaTime);
    void updateExplosions(float deltaTime);
    void updateEnemyAI(float deltaTime);
    void checkCollisions();
    void drawGame();
    void handleGameOver();
    void loadEntitiesFromBrick();
    
    // Tank operations
    void moveTank(Tank& tank, TankDirection direction);
    void shootBullet(const Tank& tank);
    void spawnEnemyTank();
    void spawnPowerup();
    void createExplosion(const Point& position);
    
    // Collision detection
    bool isValidPosition(const Point& pos) const;
    bool isWall(const Point& pos) const;
    bool isDestructibleWall(const Point& pos) const;
    bool isTankAt(const Point& pos) const;
    void destroyDestructibleWall(const Point& pos);
    
    // Utilities
    void clearGameEntities();
    void ensureStaticEntities();
    TankDirection getRandomDirection() const;
    Point getDirectionOffset(TankDirection direction) const;
};

#endif // TANK_GAME_HPP
