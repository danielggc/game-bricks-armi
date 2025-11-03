#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include <SDL2/SDL.h>
#include <string>
#include <map>
#include <functional>

// ============================================================================
// Input Manager - Handles keyboard and other input events
// ============================================================================

class InputManager {
public:
    InputManager();
    ~InputManager();
    
    // Update input state
    void update();
    
    // Query key state
    bool isKeyPressed(SDL_Keycode key) const;
    bool isKeyDown(SDL_Keycode key) const;
    bool isKeyReleased(SDL_Keycode key) const;
    
    // Action mapping
    void mapKeyToAction(SDL_Keycode key, const std::string& action);
    bool isActionActive(const std::string& action) const;
    
    // Event handling
    bool handleEvent(const SDL_Event& event);
    
    // Query
    bool shouldQuit() const { return quit; }
    
private:
    const Uint8* currentKeyState;
    Uint8* previousKeyState;
    int numKeys;
    bool quit;
    
    std::map<SDL_Keycode, std::string> keyToActionMap;
};

#endif // INPUT_MANAGER_HPP

