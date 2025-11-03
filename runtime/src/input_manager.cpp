#include "input_manager.hpp"
#include <cstring>

// ============================================================================
// Input Manager Implementation
// ============================================================================

InputManager::InputManager() 
    : currentKeyState(nullptr), previousKeyState(nullptr), numKeys(0), quit(false) {
    currentKeyState = SDL_GetKeyboardState(&numKeys);
    previousKeyState = new Uint8[numKeys];
    std::memset(previousKeyState, 0, numKeys);
}

InputManager::~InputManager() {
    delete[] previousKeyState;
}

void InputManager::update() {
    std::memcpy(previousKeyState, currentKeyState, numKeys);
    SDL_PumpEvents();
}

bool InputManager::isKeyPressed(SDL_Keycode key) const {
    int scancode = SDL_GetScancodeFromKey(key);
    return currentKeyState[scancode] && !previousKeyState[scancode];
}

bool InputManager::isKeyDown(SDL_Keycode key) const {
    int scancode = SDL_GetScancodeFromKey(key);
    return currentKeyState[scancode];
}

bool InputManager::isKeyReleased(SDL_Keycode key) const {
    int scancode = SDL_GetScancodeFromKey(key);
    return !currentKeyState[scancode] && previousKeyState[scancode];
}

void InputManager::mapKeyToAction(SDL_Keycode key, const std::string& action) {
    keyToActionMap[key] = action;
}

bool InputManager::isActionActive(const std::string& action) const {
    for (const auto& pair : keyToActionMap) {
        if (pair.second == action && isKeyDown(pair.first)) {
            return true;
        }
    }
    return false;
}

bool InputManager::handleEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_QUIT:
            quit = true;
            return true;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                quit = true;
            }
            return true;
        default:
            return false;
    }
}

