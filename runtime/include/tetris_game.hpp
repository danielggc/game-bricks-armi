#ifndef TETRIS_GAME_HPP
#define TETRIS_GAME_HPP

#include "game_engine.hpp"
#include "input_manager.hpp"

// ============================================================================
// Tetris Game - Game logic and state
// ============================================================================

// Tetromino types (classic 7-piece system)
enum TetrisBlockType {
    BLOCK_EMPTY = 0,
    BLOCK_I = 1,  // Cyan
    BLOCK_O = 2,  // Yellow
    BLOCK_T = 3,  // Purple
    BLOCK_S = 4,  // Green
    BLOCK_Z = 5,  // Red
    BLOCK_J = 6,  // Blue
    BLOCK_L = 7   // Orange
};

// Rotation states
enum RotationState {
    ROT_0 = 0,
    ROT_90 = 1,
    ROT_180 = 2,
    ROT_270 = 3
};

struct Tetromino {
    TetrisBlockType type;
    int x, y;                    // Position (grid coordinates)
    RotationState rotation;
    
    Tetromino(TetrisBlockType t = BLOCK_EMPTY, int px = 0, int py = 0)
        : type(t), x(px), y(py), rotation(ROT_0) {}
};

class TetrisGame {
public:
    TetrisGame(int gridWidth = 10, int gridHeight = 20, int cellSize = 20);
    ~TetrisGame();
    
    // Game loop
    bool initialize();
    void run();
    void update(float deltaTime);
    void handleInput();
    
    // Game state
    int getScore() const { return score; }
    int getLevel() const { return level; }
    int getLines() const { return linesCleared; }
    bool isGameOver() const { return gameOver; }
    
private:
    GameEngine engine;
    InputManager input;
    
    // Game board (0 = empty, 1-7 = block types)
    int** board;
    int boardWidth, boardHeight;
    
    // Game state
    Tetromino currentPiece;
    Tetromino nextPiece;
    
    int score;
    int level;
    int linesCleared;
    bool gameOver;
    
    float dropTimer;
    float dropDelay;
    float gravitySpeed;
    
    // Game logic
    void initializeGame();
    void spawnNewPiece();
    void generateRandomPiece();
    Tetromino getRandomTetromino() const;
    
    void updateDropTimer(float deltaTime);
    void dropPiece();
    void rotatePiece(int direction);  // 1 for CW, -1 for CCW
    void movePiece(int dx);
    
    bool canPlace(const Tetromino& piece, int dx = 0, int dy = 0) const;
    void placePiece(const Tetromino& piece);
    
    void checkAndClearLines();
    int clearLines();
    void updateScore(int linesCleared);
    
    void drawGame();
    void drawBoard();
    void drawCurrentPiece();
    void drawNextPiece();
    void handleGameOver();
    
    // Tetromino shape helpers
    void getPieceShape(TetrisBlockType type, RotationState rot, 
                      int shape[4][4]) const;
    
    // Utilities
    bool isValidPosition(int x, int y) const;
    void clearBoard();
};

#endif // TETRIS_GAME_HPP

