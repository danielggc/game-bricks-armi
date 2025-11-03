#include "menu.hpp"
#include "snake_game.hpp"
#include "tetris_game.hpp"
#include <iostream>
#include <cstdlib>

void displayWelcome() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════╗\n";
    std::cout << "║                                            ║\n";
    std::cout << "║          🎮 BRICK GAMES ENGINE 🎮          ║\n";
    std::cout << "║                                            ║\n";
    std::cout << "║      Motor de Juegos de Ladrillos         ║\n";
    std::cout << "║                                            ║\n";
    std::cout << "║   🐍 Snake    |    🟦 Tetris             ║\n";
    std::cout << "║                                            ║\n";
    std::cout << "╚════════════════════════════════════════════╝\n";
    std::cout << "\n";
}

int main(int argc, char* argv[]) {
    displayWelcome();
    
    std::cout << "Initializing menu system...\n" << std::endl;
    
    // Create and initialize menu
    MainMenu menu;
    
    if (!menu.initialize()) {
        std::cerr << "Error: Failed to initialize menu!" << std::endl;
        return 1;
    }
    
    std::cout << "Menu initialized successfully!" << std::endl;
    std::cout << "\nSelect a game using arrow keys or WASD" << std::endl;
    std::cout << "Press SPACE or ENTER to select" << std::endl;
    std::cout << "Press ESC to exit\n" << std::endl;
    
    // Show menu and get selection
    GameSelection selected = menu.run();
    
    // Clear console output
    std::cout << "\n\n";
    
    // Execute selected game
    switch (selected) {
        case GAME_SNAKE: {
            std::cout << "Starting Snake Game...\n" << std::endl;
            SnakeGame snakeGame(30, 30, 20);
            
            if (!snakeGame.initialize()) {
                std::cerr << "Error: Failed to initialize Snake game!" << std::endl;
                return 1;
            }
            
            std::cout << "🐍 Snake Game Controls:\n";
            std::cout << "  Arrow Keys / WASD - Move\n";
            std::cout << "  SPACE - Pause\n";
            std::cout << "  ESC - Return to Menu\n\n";
            
            snakeGame.run();
            
            std::cout << "\nReturning to menu...\n" << std::endl;
            
            // Restart menu
            return main(argc, argv);
        }
        break;
        
        case GAME_TETRIS: {
            std::cout << "Starting Tetris Game...\n" << std::endl;
            TetrisGame tetrisGame(10, 20, 30);
            
            if (!tetrisGame.initialize()) {
                std::cerr << "Error: Failed to initialize Tetris game!" << std::endl;
                return 1;
            }
            
            std::cout << "🟦 Tetris Game Controls:\n";
            std::cout << "  Arrow Keys / ASDF - Move\n";
            std::cout << "  Up / W - Rotate\n";
            std::cout << "  SPACE - Hard Drop\n";
            std::cout << "  ESC - Return to Menu\n\n";
            
            tetrisGame.run();
            
            std::cout << "\nReturning to menu...\n" << std::endl;
            
            // Restart menu
            return main(argc, argv);
        }
        break;
        
        case GAME_EXIT:
        default: {
            std::cout << "Thank you for playing Brick Games!" << std::endl;
            std::cout << "\n🎮 Goodbye! 🎮\n" << std::endl;
            return 0;
        }
        break;
    }
}

