#include "brick_loader.hpp"
#include "snake_game.hpp"
#include "tetris_game.hpp"
#include "tank_game.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>

struct BrickGame {
    std::string filename;
    std::string displayName;
};

class SimpleMenu {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::vector<BrickGame> games;
    int selectedIndex;
    bool running;
    TTF_Font* font;
    
public:
    SimpleMenu() : window(nullptr), renderer(nullptr), selectedIndex(0), running(false), font(nullptr) {}
    
    ~SimpleMenu() {
        if (font) TTF_CloseFont(font);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
    }
    
    bool initialize() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL Init failed: " << SDL_GetError() << std::endl;
            return false;
        }
        
        if (TTF_Init() == -1) {
            std::cerr << "TTF Init failed: " << TTF_GetError() << std::endl;
            SDL_Quit();
            return false;
        }
        
        window = SDL_CreateWindow("Brick Games",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            800, 600, SDL_WINDOW_SHOWN);
            
        if (!window) {
            std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
            TTF_Quit();
            SDL_Quit();
            return false;
        }
        
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            TTF_Quit();
            SDL_Quit();
            return false;
        }
        
        font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", 24);
        if (!font) {
            font = TTF_OpenFont("/System/Library/Fonts/Arial.ttf", 24);
        }
        if (!font) {
            font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
        }
        if (!font) {
            std::cerr << "Font loading failed: " << TTF_GetError() << std::endl;
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            TTF_Quit();
            SDL_Quit();
            return false;
        }
        
        scanForGames();
        return true;
    }
    
    void scanForGames() {
        games.clear();
        
        std::string searchPath = "../../brickc/examples/";
        
        struct stat info;
        if (stat(searchPath.c_str(), &info) == 0 && (info.st_mode & S_IFDIR)) {
            DIR* dir = opendir(searchPath.c_str());
            if (dir) {
                struct dirent* entry;
                while ((entry = readdir(dir)) != nullptr) {
                    std::string filename = entry->d_name;
                    
                    if (filename == "." || filename == "..") continue;
                    
                    if (filename.length() > 6 && 
                        filename.substr(filename.length() - 6) == ".brick") {
                        
                        std::string filepath = searchPath + filename;
                        BrickLoader loader;
                        if (loader.loadBrickFile(filepath)) {
                            BrickGame game;
                            game.filename = filepath;
                            game.displayName = loader.getGameName();
                            games.push_back(game);
                        }
                    }
                }
                closedir(dir);
            }
        }
        
        std::sort(games.begin(), games.end(), 
            [](const BrickGame& a, const BrickGame& b) -> bool {
                return a.displayName < b.displayName;
            });
    }
    
    void renderText(const std::string& text, int x, int y, SDL_Color color, SDL_Texture** texture, SDL_Rect* rect) {
        SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text.c_str(), color);
        if (!surfaceMessage) {
            std::cerr << "Text surface creation failed: " << TTF_GetError() << std::endl;
            return;
        }
        
        *texture = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
        if (!*texture) {
            std::cerr << "Text texture creation failed: " << SDL_GetError() << std::endl;
            SDL_FreeSurface(surfaceMessage);
            return;
        }
        
        rect->x = x;
        rect->y = y;
        rect->w = surfaceMessage->w;
        rect->h = surfaceMessage->h;
        
        SDL_FreeSurface(surfaceMessage);
    }
    
    void render() {
        SDL_SetRenderDrawColor(renderer, 20, 30, 50, 255);
        SDL_RenderClear(renderer);
        
        SDL_Color white = {255, 255, 255, 255};
        SDL_Color green = {0, 255, 100, 255};
        SDL_Color gray = {180, 180, 180, 255};
        
        SDL_Texture* titleTexture;
        SDL_Rect titleRect;
        renderText("BRICK GAMES MENU", 250, 50, white, &titleTexture, &titleRect);
        SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
        SDL_DestroyTexture(titleTexture);
        
        for (size_t i = 0; i < games.size(); i++) {
            int y = 150 + i * 60;
            bool isSelected = (i == static_cast<size_t>(selectedIndex));
            
            if (isSelected) {
                SDL_SetRenderDrawColor(renderer, 0, 100, 50, 100);
                SDL_Rect highlight = {50, y - 10, 700, 50};
                SDL_RenderFillRect(renderer, &highlight);
            }
            
            std::string displayText = (isSelected ? "> " : "  ") + games[i].displayName;
            SDL_Color textColor = isSelected ? green : white;
            
            SDL_Texture* gameTexture;
            SDL_Rect gameRect;
            renderText(displayText, 70, y, textColor, &gameTexture, &gameRect);
            SDL_RenderCopy(renderer, gameTexture, NULL, &gameRect);
            SDL_DestroyTexture(gameTexture);
        }
        
        SDL_Texture* instrTexture;
        SDL_Rect instrRect;
        renderText("Use Arrow Keys to Navigate, ENTER to Select, ESC to Exit", 100, 500, gray, &instrTexture, &instrRect);
        SDL_RenderCopy(renderer, instrTexture, NULL, &instrRect);
        SDL_DestroyTexture(instrTexture);
        
        SDL_RenderPresent(renderer);
    }
    
    void handleEvents() {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                    
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            running = false;
                            break;
                            
                        case SDLK_UP:
                            if (!games.empty()) {
                                selectedIndex = (selectedIndex - 1 + games.size()) % games.size();
                            }
                            break;
                            
                        case SDLK_DOWN:
                            if (!games.empty()) {
                                selectedIndex = (selectedIndex + 1) % games.size();
                            }
                            break;
                            
                        case SDLK_RETURN:
                            if (!games.empty() && selectedIndex < static_cast<int>(games.size())) {
                                playGame(games[selectedIndex]);
                            }
                            break;
                    }
                    break;
            }
        }
    }
    
    void playGame(const BrickGame& game) {
        std::cout << "Loading: " << game.filename << std::endl;
        
        BrickLoader loader;
        if (!loader.loadBrickFile(game.filename)) {
            std::cerr << "Error loading brick file!" << std::endl;
            return;
        }
        
        std::string gameName = loader.getGameName();
        std::cout << "Game type: " << gameName << std::endl;
        
        if (gameName.find("snake") != std::string::npos) {
            SnakeGame snakeGame(window, renderer, game.filename, 20);
            std::cout << "Initializing Snake game..." << std::endl;
            if (snakeGame.initialize()) {
                std::cout << "Snake game initialized, starting..." << std::endl;
                snakeGame.run();
                std::cout << "Snake game finished, returning to menu..." << std::endl;
            } else {
                std::cerr << "Failed to initialize Snake game!" << std::endl;
            }
        } else if (gameName.find("tetris") != std::string::npos) {
            TetrisGame tetrisGame(window, renderer, game.filename, 30);
            std::cout << "Initializing Tetris game..." << std::endl;
            if (tetrisGame.initialize()) {
                std::cout << "Tetris game initialized, starting..." << std::endl;
                tetrisGame.run();
                std::cout << "Tetris game finished, returning to menu..." << std::endl;
            } else {
                std::cerr << "Failed to initialize Tetris game!" << std::endl;
            }
        } else if (gameName.find("tank") != std::string::npos) {
            TankGame tankGame(window, renderer, game.filename, 25);
            std::cout << "Initializing Tank game..." << std::endl;
            if (tankGame.initialize()) {
                std::cout << "Tank game initialized, starting..." << std::endl;
                tankGame.run();
                std::cout << "Tank game finished, returning to menu..." << std::endl;
            } else {
                std::cerr << "Failed to initialize Tank game!" << std::endl;
            }
        } else {
            std::cout << "Unknown game type '" << gameName << "', using Snake as default..." << std::endl;
            SnakeGame snakeGame(window, renderer, game.filename, 20);
            if (snakeGame.initialize()) {
                snakeGame.run();
            }
        }
    }
    
    void run() {
        if (!initialize()) return;
        if (games.empty()) {
            std::cout << "No .brick files found!" << std::endl;
            return;
        }
        
        std::cout << "Found " << games.size() << " brick games:" << std::endl;
        for (const auto& game : games) {
            std::cout << "  - " << game.displayName << std::endl;
        }
        std::cout << "\nUse arrow keys to navigate, ENTER to select, ESC to exit" << std::endl;
        
        running = true;
        while (running) {
            handleEvents();
            render();
            SDL_Delay(16);
        }
    }
};

int main() {
    SimpleMenu menu;
    menu.run();
    return 0;
}