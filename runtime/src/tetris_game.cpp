#include "tetris_game.hpp"
#include "../../brickc/src/symbols.hpp"
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <cstring>

// ============================================================================
// Tetris Game Implementation
// ============================================================================

TetrisGame::TetrisGame(int gridWidth, int gridHeight, int cellSize)
    : engine(gridWidth, gridHeight, cellSize), useExternalWindow(false), useBrickFile(false),
      boardWidth(10), boardHeight(20),
      score(0), level(1), linesCleared(0), gameOver(false),
      dropTimer(0.0f), dropDelay(0.5f), gravitySpeed(0.5f),
      inputTimer(0.0f), inputDelay(0.15f) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    board = new int*[boardHeight];
    for (int i = 0; i < boardHeight; i++) {
        board[i] = new int[boardWidth];
    }
}

TetrisGame::TetrisGame(SDL_Window* window, SDL_Renderer* renderer, int gridWidth, int gridHeight, int cellSize)
    : engine(window, renderer, gridWidth, gridHeight, cellSize), useExternalWindow(true), useBrickFile(false),
      boardWidth(gridWidth), boardHeight(gridHeight),
      score(0), level(1), linesCleared(0), gameOver(false),
      dropTimer(0.0f), dropDelay(0.5f), gravitySpeed(0.5f),
      inputTimer(0.0f), inputDelay(0.15f) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    board = new int*[boardHeight];
    for (int i = 0; i < boardHeight; i++) {
        board[i] = new int[boardWidth];
    }
}

TetrisGame::TetrisGame(SDL_Window* window, SDL_Renderer* renderer, const std::string& brickFile, int cellSize)
    : engine(window, renderer, 10, 20, cellSize), useExternalWindow(true), useBrickFile(true),
      boardWidth(10), boardHeight(20),
      score(0), level(1), linesCleared(0), gameOver(false),
      dropTimer(0.0f), dropDelay(0.5f), gravitySpeed(0.5f),
      inputTimer(0.0f), inputDelay(0.15f) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    if (brickLoader.loadBrickFile(brickFile)) {
        boardWidth = brickLoader.getGridWidth();
        boardHeight = brickLoader.getGridHeight();
        engine = GameEngine(window, renderer, boardWidth, boardHeight, cellSize);
        
        int brickSpeed = brickLoader.getSpeed();
        gravitySpeed = 1.0f / brickSpeed;
        dropDelay = gravitySpeed;
        score = brickLoader.getScore();
        
        std::cout << "Tetris configuration from .brick:" << std::endl;
        std::cout << "  Speed: " << brickSpeed << " (gravity: " << gravitySpeed << ")" << std::endl;
        std::cout << "  dropDelay: " << dropDelay << " seconds" << std::endl;
        std::cout << "  Grid: " << boardWidth << "x" << boardHeight << std::endl;
        std::cout << "  Score: " << score << std::endl;
        std::cout << "  Color: 0x" << std::hex << brickLoader.getGameColor() << std::dec << std::endl;
        
        Color bgColor = Color::fromHex(brickLoader.getGameColor());
        engine.setBackgroundColor(bgColor);
    }
    
    board = new int*[boardHeight];
    for (int i = 0; i < boardHeight; i++) {
        board[i] = new int[boardWidth];
    }
}

TetrisGame::~TetrisGame() {
    // Free board
    for (int i = 0; i < boardHeight; i++) {
        delete[] board[i];
    }
    delete[] board;
}

bool TetrisGame::initialize() {
    if (!engine.initialize()) {
        return false;
    }
    
    // Map controls
    input.mapKeyToAction(SDLK_LEFT, "left");
    input.mapKeyToAction(SDLK_a, "left");
    input.mapKeyToAction(SDLK_RIGHT, "right");
    input.mapKeyToAction(SDLK_d, "right");
    input.mapKeyToAction(SDLK_DOWN, "down");
    input.mapKeyToAction(SDLK_s, "down");
    input.mapKeyToAction(SDLK_UP, "rotate");
    input.mapKeyToAction(SDLK_w, "rotate");
    input.mapKeyToAction(SDLK_SPACE, "hardDrop");
    input.mapKeyToAction(SDLK_p, "pause");
    input.mapKeyToAction(SDLK_ESCAPE, "quit");
    
    initializeGame();
    loadEntitiesFromBrick();
    
    return true;
}

void TetrisGame::initializeGame() {
    clearBoard();
    score = 0;
    level = 1;
    linesCleared = 0;
    gameOver = false;
    dropTimer = 0.0f;
    
    if (useBrickFile && brickLoader.hasEntity("piece")) {
        const brick::Entity* pieceEntity = brickLoader.findEntity("piece");
        currentPiece = Tetromino(BLOCK_I, pieceEntity->spawn.x, pieceEntity->spawn.y);
        std::cout << "Tetris piece initialized at position from .brick: (" << pieceEntity->spawn.x << ", " << pieceEntity->spawn.y << ")" << std::endl;
    } else {
        currentPiece = getRandomTetromino();
        currentPiece.x = boardWidth / 2;
        currentPiece.y = 0;
    }
    
    nextPiece = getRandomTetromino();
}

void TetrisGame::clearBoard() {
    for (int y = 0; y < boardHeight; y++) {
        for (int x = 0; x < boardWidth; x++) {
            board[y][x] = BLOCK_EMPTY;
        }
    }
}

Tetromino TetrisGame::getRandomTetromino() const {
    int type = (std::rand() % 7) + 1;
    return Tetromino(static_cast<TetrisBlockType>(type), 3, 0);
}

void TetrisGame::spawnNewPiece() {
    currentPiece = nextPiece;
    nextPiece = getRandomTetromino();
    
    if (!canPlace(currentPiece)) {
        gameOver = true;
        handleGameOver();
    }
}

bool TetrisGame::isValidPosition(int x, int y) const {
    return x >= 0 && x < boardWidth && y >= 0 && y < boardHeight;
}

bool TetrisGame::canPlace(const Tetromino& piece, int dx, int dy) const {
    int shape[4][4] = {};
    getPieceShape(piece.type, piece.rotation, shape);
    
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (shape[y][x] == 0) continue;
            
            int checkX = piece.x + x + dx;
            int checkY = piece.y + y + dy;
            
            if (!isValidPosition(checkX, checkY)) {
                return false;
            }
            
            if (board[checkY][checkX] != BLOCK_EMPTY) {
                return false;
            }
        }
    }
    
    return true;
}

void TetrisGame::placePiece(const Tetromino& piece) {
    int shape[4][4] = {};
    getPieceShape(piece.type, piece.rotation, shape);
    
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (shape[y][x] == 0) continue;
            
            int boardX = piece.x + x;
            int boardY = piece.y + y;
            
            if (isValidPosition(boardX, boardY)) {
                board[boardY][boardX] = piece.type;
            }
        }
    }
}

void TetrisGame::rotatePiece(int direction) {
    Tetromino rotated = currentPiece;
    rotated.rotation = (RotationState)((rotated.rotation + direction + 4) % 4);
    
    if (canPlace(rotated)) {
        currentPiece = rotated;
    }
}

void TetrisGame::movePiece(int dx) {
    if (canPlace(currentPiece, dx, 0)) {
        currentPiece.x += dx;
    }
}

void TetrisGame::dropPiece() {
    if (canPlace(currentPiece, 0, 1)) {
        currentPiece.y++;
    } else {
        placePiece(currentPiece);
        checkAndClearLines();
        
        currentPiece = nextPiece;
        currentPiece.x = boardWidth / 2;
        currentPiece.y = 0;
        nextPiece = getRandomTetromino();
        
        if (!canPlace(currentPiece, 0, 0)) {
            gameOver = true;
        }
    }
}

void TetrisGame::updateDropTimer(float deltaTime) {
    dropTimer += deltaTime;
    
    while (dropTimer >= dropDelay) {
        dropTimer -= dropDelay;
        dropPiece();
        
        if (gameOver) break;
    }
}

void TetrisGame::checkAndClearLines() {
    int cleared = clearLines();
    if (cleared > 0) {
        updateScore(cleared);
    }
}

int TetrisGame::clearLines() {
    int linesCount = 0;
    int y = boardHeight - 1;
    
    while (y >= 0) {
        bool fullLine = true;
        for (int x = 0; x < boardWidth; x++) {
            if (board[y][x] == BLOCK_EMPTY) {
                fullLine = false;
                break;
            }
        }
        
        if (fullLine) {
            linesCount++;
            
            // Move everything down
            for (int moveY = y; moveY > 0; moveY--) {
                std::memcpy(board[moveY], board[moveY - 1], boardWidth * sizeof(int));
            }
            std::memset(board[0], BLOCK_EMPTY, boardWidth * sizeof(int));
            
            // Don't increment y, check this line again
        } else {
            y--;
        }
    }
    
    linesCleared += linesCount;
    return linesCount;
}

void TetrisGame::updateScore(int lines) {
    // Classic Tetris scoring
    int points[5] = {0, 100, 300, 500, 800};
    score += points[lines] * level;
    
    // Level increases every 10 lines
    int newLevel = 1 + (linesCleared / 10);
    if (newLevel != level) {
        level = newLevel;
        dropDelay = std::max(0.1f, 0.5f - (level - 1) * 0.05f);
    }
}

void TetrisGame::handleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        input.handleEvent(event);
    }
    
    input.update();
    
    if (inputTimer >= inputDelay) {
        if (input.isActionActive("left")) {
            movePiece(-1);
            inputTimer = 0.0f;
        } else if (input.isActionActive("right")) {
            movePiece(1);
            inputTimer = 0.0f;
        } else if (input.isActionActive("down")) {
            dropPiece();
            dropTimer = 0.0f;
            inputTimer = 0.0f;
        } else if (input.isActionActive("rotate")) {
            rotatePiece(1);
            inputTimer = 0.0f;
        }
    }
    
    if (input.isActionActive("hardDrop")) {
        while (canPlace(currentPiece, 0, 1)) {
            currentPiece.y++;
        }
        dropPiece();
    }
    
    if (input.isActionActive("quit")) {
        if (useExternalWindow) {
            engine.quit();
        } else {
            gameOver = true;
        }
    }
}

void TetrisGame::getPieceShape(TetrisBlockType type, RotationState rot, 
                               int shape[4][4]) const {
    // Initialize as empty
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            shape[i][j] = 0;
        }
    }
    
    // Define piece shapes for each rotation
    switch (type) {
        case BLOCK_I:  // Cyan I-piece
            if (rot == ROT_0 || rot == ROT_180) {
                shape[1][0] = shape[1][1] = shape[1][2] = shape[1][3] = 1;
            } else {
                shape[0][1] = shape[1][1] = shape[2][1] = shape[3][1] = 1;
            }
            break;
            
        case BLOCK_O:  // Yellow O-piece (square)
            shape[0][0] = shape[0][1] = shape[1][0] = shape[1][1] = 1;
            break;
            
        case BLOCK_T:  // Purple T-piece
            if (rot == ROT_0) {
                shape[0][1] = 1;
                shape[1][0] = shape[1][1] = shape[1][2] = 1;
            } else if (rot == ROT_90) {
                shape[0][1] = shape[1][1] = shape[2][1] = 1;
                shape[1][2] = 1;
            } else if (rot == ROT_180) {
                shape[1][0] = shape[1][1] = shape[1][2] = 1;
                shape[2][1] = 1;
            } else {
                shape[0][1] = 1;
                shape[1][0] = shape[1][1] = shape[1][2] = 1;
            }
            break;
            
        case BLOCK_S:  // Green S-piece
            if (rot == ROT_0 || rot == ROT_180) {
                shape[0][1] = shape[0][2] = 1;
                shape[1][0] = shape[1][1] = 1;
            } else {
                shape[0][1] = shape[1][1] = 1;
                shape[1][2] = shape[2][2] = 1;
            }
            break;
            
        case BLOCK_Z:  // Red Z-piece
            if (rot == ROT_0 || rot == ROT_180) {
                shape[0][0] = shape[0][1] = 1;
                shape[1][1] = shape[1][2] = 1;
            } else {
                shape[0][2] = shape[1][1] = 1;
                shape[1][2] = shape[2][1] = 1;
            }
            break;
            
        case BLOCK_J:  // Blue J-piece
            if (rot == ROT_0) {
                shape[0][0] = 1;
                shape[1][0] = shape[1][1] = shape[1][2] = 1;
            } else if (rot == ROT_90) {
                shape[0][1] = shape[0][2] = 1;
                shape[1][1] = 1;
                shape[2][1] = 1;
            } else if (rot == ROT_180) {
                shape[1][0] = shape[1][1] = shape[1][2] = 1;
                shape[2][2] = 1;
            } else {
                shape[0][1] = 1;
                shape[1][1] = 1;
                shape[2][0] = shape[2][1] = 1;
            }
            break;
            
        case BLOCK_L:  // Orange L-piece
            if (rot == ROT_0) {
                shape[0][2] = 1;
                shape[1][0] = shape[1][1] = shape[1][2] = 1;
            } else if (rot == ROT_90) {
                shape[0][1] = 1;
                shape[1][1] = 1;
                shape[2][1] = shape[2][2] = 1;
            } else if (rot == ROT_180) {
                shape[1][0] = shape[1][1] = shape[1][2] = 1;
                shape[2][0] = 1;
            } else {
                shape[0][0] = shape[0][1] = 1;
                shape[1][1] = 1;
                shape[2][1] = 1;
            }
            break;
            
        default:
            break;
    }
}

void TetrisGame::drawGame() {
    engine.update(0.016f);
    
    drawBoard();
    drawCurrentPiece();
    
    engine.render();
}

void TetrisGame::drawBoard() {
    // Clear entities
    for (int y = 0; y < boardHeight; y++) {
        for (int x = 0; x < boardWidth; x++) {
            if (board[y][x] != BLOCK_EMPTY) {
                std::string name = "block_" + std::to_string(y * boardWidth + x);
                
                Color colors[] = {
                    Color(0, 0, 0),           // Empty
                    Color(0, 255, 255),       // Cyan I
                    Color(255, 255, 0),       // Yellow O
                    Color(200, 0, 200),       // Purple T
                    Color(0, 255, 0),         // Green S
                    Color(255, 0, 0),         // Red Z
                    Color(0, 0, 255),         // Blue J
                    Color(255, 165, 0)        // Orange L
                };
                
                Entity block(name, Point(x, y), colors[board[y][x]]);
                engine.addEntity(block);
            }
        }
    }
}

void TetrisGame::drawCurrentPiece() {
    int shape[4][4] = {};
    getPieceShape(currentPiece.type, currentPiece.rotation, shape);
    
    Color colors[] = {
        Color(0, 0, 0),           // Empty
        Color(0, 255, 255),       // Cyan I
        Color(255, 255, 0),       // Yellow O
        Color(200, 0, 200),       // Purple T
        Color(0, 255, 0),         // Green S
        Color(255, 0, 0),         // Red Z
        Color(0, 0, 255),         // Blue J
        Color(255, 165, 0)        // Orange L
    };
    
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (shape[y][x] == 0) continue;
            
            int boardX = currentPiece.x + x;
            int boardY = currentPiece.y + y;
            
            if (isValidPosition(boardX, boardY)) {
                std::string name = "current_" + std::to_string(y * 4 + x);
                Entity piece(name, Point(boardX, boardY), colors[currentPiece.type]);
                engine.addEntity(piece);
            }
        }
    }
}

void TetrisGame::drawNextPiece() {

}

void TetrisGame::handleGameOver() {
    std::cout << "\n=== GAME OVER ===" << std::endl;
    std::cout << "Final Score: " << score << std::endl;
    std::cout << "Level: " << level << std::endl;
    std::cout << "Lines Cleared: " << linesCleared << std::endl;
    std::cout << "================\n" << std::endl;
}

void TetrisGame::loadEntitiesFromBrick() {
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
        
        Color entityColor = Color::fromHex(brickEntity.color);
        Entity gameEntity(name, Point(brickEntity.spawn.x, brickEntity.spawn.y), entityColor);
        engine.addEntity(gameEntity);
    }
}

void TetrisGame::update(float deltaTime) {
    inputTimer += deltaTime;
    handleInput();
    updateDropTimer(deltaTime);
}

void TetrisGame::run() {
    const float FPS = 60.0f;
    const float FRAME_TIME = 1.0f / FPS;
    
    unsigned int lastTime = SDL_GetTicks();
    
    while (engine.isRunning() && !input.shouldQuit() && !gameOver) {
        unsigned int currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        if (deltaTime > FRAME_TIME) {
            deltaTime = FRAME_TIME;
        }
        
        update(deltaTime);
        drawGame();
    }
    
    if (!useExternalWindow) {
        handleGameOver();
    }
}

