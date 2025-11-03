#ifndef MENU_HPP
#define MENU_HPP

#include "game_engine.hpp"
#include "input_manager.hpp"

// ============================================================================
// Main Menu - Game Selection Interface
// ============================================================================

enum GameSelection {
    GAME_NONE = 0,
    GAME_SNAKE = 1,
    GAME_TETRIS = 2,
    GAME_EXIT = 3
};

class MainMenu {
public:
    MainMenu();
    ~MainMenu();
    
    bool initialize();
    GameSelection run();  // Returns selected game
    
private:
    GameEngine engine;
    InputManager input;
    
    int selectedOption;  // 0 = Snake, 1 = Tetris, 2 = Exit
    bool menuActive;
    
    void drawMenu();
    void drawTitle();
    void drawOptions();
    void drawInstructions();
    void handleInput();
    
    // UI helpers
    void drawCenteredText(int y, const std::string& text, const Color& color, 
                         int textWidth = 1, int charHeight = 1);
};

#endif // MENU_HPP

