#include "snake_game.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "================================" << std::endl;
    std::cout << "  Brick Snake - Graphics Engine" << std::endl;
    std::cout << "================================\n" << std::endl;
    
    // Create snake game with a 30x30 grid and 20-pixel cells
    SnakeGame game(30, 30, 20);
    
    std::cout << "Initializing graphics engine..." << std::endl;
    if (!game.initialize()) {
        std::cerr << "Error: Failed to initialize game engine!" << std::endl;
        return 1;
    }
    
    std::cout << "Game initialized successfully!" << std::endl;
    std::cout << "\nControls:" << std::endl;
    std::cout << "  Arrow Keys or WASD - Move snake" << std::endl;
    std::cout << "  SPACE - Pause/Resume" << std::endl;
    std::cout << "  ESC - Exit game" << std::endl;
    std::cout << "\nGame Info:" << std::endl;
    std::cout << "  Red squares - Regular fruit (+10 points)" << std::endl;
    std::cout << "  Yellow squares - Super fruit (+50 points, +3 length)" << std::endl;
    std::cout << "  Lives: 3" << std::endl;
    std::cout << "\nStarting game...\n" << std::endl;
    
    // Run the game
    game.run();
    
    std::cout << "Thank you for playing!" << std::endl;
    
    return 0;
}

