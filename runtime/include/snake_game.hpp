#ifndef SNAKE_GAME_HPP
#define SNAKE_GAME_HPP

#include "game_engine.hpp"
#include "input_manager.hpp"
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
    ~SnakeGame();
    
    // Game loop
    bool initialize();
    void run();
    void update(float deltaTime);
    void handleInput();
    
    // Game state
    int getScore() const { return score; }
    int getLives() const { return lives; }
    bool isGameOver() const { return gameOver; }
    bool isPaused() const { return paused; }
    
private:
    GameEngine engine;
    InputManager input;
    
    // Game state
    std::deque<SnakeSegment> snake;
    Point fruitPosition;
    Point superfruitPosition;
    
    int score;
    int lives;
    int gameSpeed;
    bool gameOver;
    bool paused;
    
    Direction currentDirection;
    Direction nextDirection;
    float moveTimer;
    float moveDelay;
    
    // Game logic
    void initializeGame();
    void updateSnake(float deltaTime);
    void spawnFruit();
    void spawnSuperFruit();
    void checkCollisions();
    void drawGame();
    void handleGameOver();
    
    // Utilities
    bool isValidPosition(const Point& pos) const;
    bool isSnakeBody(const Point& pos) const;
};

#endif // SNAKE_GAME_HPP
