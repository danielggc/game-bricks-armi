#include "tetris_game.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "════════════════════════════════════════" << std::endl;
    std::cout << "  Brick Tetris - Graphics Engine" << std::endl;
    std::cout << "════════════════════════════════════════\n" << std::endl;
    
    // Create Tetris game with a 10x20 grid and 30-pixel cells
    TetrisGame game(10, 20, 30);
    
    std::cout << "Initializing graphics engine..." << std::endl;
    if (!game.initialize()) {
        std::cerr << "Error: Failed to initialize game engine!" << std::endl;
        return 1;
    }
    
    std::cout << "Game initialized successfully!" << std::endl;
    std::cout << "\nControls:" << std::endl;
    std::cout << "  Left/Right:      Move left/right" << std::endl;
    std::cout << "  Down/Drop:       Soft drop / Hard drop" << std::endl;
    std::cout << "  Up/W or Rotate:  Rotate piece clockwise" << std::endl;
    std::cout << "  Space:           Hard drop (instant)" << std::endl;
    std::cout << "  ESC:             Exit game" << std::endl;
    std::cout << "\nGame Info:" << std::endl;
    std::cout << "  Board: 10x20 grid" << std::endl;
    std::cout << "  7 Tetromino types with different colors" << std::endl;
    std::cout << "  Classic Tetris scoring system" << std::endl;
    std::cout << "  Difficulty increases with level" << std::endl;
    std::cout << "\nColors:" << std::endl;
    std::cout << "  I: Cyan   | O: Yellow  | T: Purple" << std::endl;
    std::cout << "  S: Green  | Z: Red     | J: Blue   | L: Orange" << std::endl;
    std::cout << "\nStarting game...\n" << std::endl;
    
    // Run the game
    game.run();
    
    std::cout << "Thank you for playing Tetris!" << std::endl;
    
    return 0;
}

