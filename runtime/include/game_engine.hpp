#ifndef GAME_ENGINE_HPP
#define GAME_ENGINE_HPP

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <map>

// ============================================================================
// Game Engine Core Components
// ============================================================================

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const Point& other) const {
        return !(*this == other);
    }
};

struct Color {
    Uint8 r, g, b, a;
    Color(Uint8 r = 0, Uint8 g = 0, Uint8 b = 0, Uint8 a = 255) 
        : r(r), g(g), b(b), a(a) {}
    
    static Color fromHex(unsigned int hex) {
        return Color((hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF);
    }
};

struct Entity {
    std::string name;
    Point position;
    Color color;
    int width, height;
    bool solid;
    bool visible;
    
    Entity(const std::string& name = "", const Point& pos = Point(0, 0),
           const Color& col = Color(255, 255, 255), int w = 1, int h = 1)
        : name(name), position(pos), color(col), width(w), height(h), 
          solid(true), visible(true) {}
};

// ============================================================================
// Game Engine
// ============================================================================

class GameEngine {
public:
    GameEngine(int gridWidth, int gridHeight, int cellSize = 20);
    ~GameEngine();
    
    bool initialize();
    void update(float deltaTime);
    void render();
    bool isRunning() const { return running; }
    void quit() { running = false; }
    
    // Entity management
    void addEntity(const Entity& entity);
    void removeEntity(const std::string& name);
    Entity* getEntity(const std::string& name);
    
    // Game settings
    void setGameSpeed(int speed) { gameSpeed = speed; }
    void setBackgroundColor(const Color& color) { backgroundColor = color; }
    
    // Properties
    int getGridWidth() const { return gridWidth; }
    int getGridHeight() const { return gridHeight; }
    int getCellSize() const { return cellSize; }
    
    SDL_Window* getWindow() { return window; }
    SDL_Renderer* getRenderer() { return renderer; }

private:
    int gridWidth, gridHeight;
    int cellSize;
    int gameSpeed;
    bool running;
    
    SDL_Window* window;
    SDL_Renderer* renderer;
    
    Color backgroundColor;
    std::map<std::string, Entity> entities;
    
    void renderGrid();
    void renderEntities();
};

#endif // GAME_ENGINE_HPP
