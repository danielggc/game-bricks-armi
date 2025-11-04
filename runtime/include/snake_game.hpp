#ifndef SNAKE_GAME_HPP
#define SNAKE_GAME_HPP

#include "game_engine.hpp"
#include "input_manager.hpp"
#include "brick_loader.hpp"
#include <queue>
#include <ctime>

// ============================================================================
// Snake Game - Game logic and state
// ============================================================================

enum Direction {
    NONE = 0,
    UP = 1,
    DOWN = 2,
    LEFT = 3,
    RIGHT = 4
};

struct SnakeSegment {
    Point position;
    SnakeSegment(int x = 0, int y = 0) : position(x, y) {}
    SnakeSegment(const Point& pos) : position(pos) {}
};

class SnakeGame {
public:
    SnakeGame(int gridWidth, int gridHeight, int cellSize = 20);
    SnakeGame(SDL_Window* window, SDL_Renderer* renderer, int gridWidth, int gridHeight, int cellSize = 20);
    SnakeGame(SDL_Window* window, SDL_Renderer* renderer, const std::string& brickFile, int cellSize = 20);
    ~SnakeGame();
    
    bool initialize();
    void run();
    void update(float deltaTime);
    void handleInput();
    
    // Game state
    int getScore() const { return score; }
    int getLives() const { return lives; }
    bool isGameOver() const { return gameOver; }
    bool isPaused() const { return paused; }
    
    void setSpeed(int speed) { gameSpeed = speed; moveDelay = 1.0f / speed; }
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
    
    std::deque<SnakeSegment> snake;
    Point fruitPosition;
    Point superfruitPosition;
    Point obstaclePosition;
    Point wallPosition;
    
    int score;
    int lives;
    int gameSpeed;
    bool gameOver;
    bool paused;
    
    Direction currentDirection;
    Direction nextDirection;
    float moveTimer;
    float moveDelay;
    float obstacleTimer;
    float obstacleDelay;
    float wallTimer;
    float wallDelay;
    
    // Game logic
    void initializeGame();
    void updateSnake(float deltaTime);
    void spawnFruit();
    void spawnSuperFruit();
    void updateMovingObstacles(float deltaTime);
    void moveObstacle();
    void moveWall();
    void checkCollisions();
    void drawGame();
    void handleGameOver();
    void loadEntitiesFromBrick();
    
    // Utilities
    bool isValidPosition(const Point& pos) const;
    bool isSnakeBody(const Point& pos) const;
    bool isSolidEntity(const Point& pos) const;
    void clearSnakeEntities();
    void ensureStaticEntities();
};

#endif // SNAKE_GAME_HPP
