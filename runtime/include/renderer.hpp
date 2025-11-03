#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL2/SDL.h>
#include <string>

// ============================================================================
// Renderer - Graphics rendering utilities
// ============================================================================

class Renderer {
public:
    static void drawRect(SDL_Renderer* renderer, int x, int y, int w, int h, 
                         Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
    
    static void fillRect(SDL_Renderer* renderer, int x, int y, int w, int h, 
                         Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
    
    static void drawLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2,
                         Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
    
    static void drawCircle(SDL_Renderer* renderer, int x, int y, int radius,
                           Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
    
    static void fillCircle(SDL_Renderer* renderer, int x, int y, int radius,
                           Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
    
    static void clear(SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b);
    
    static void present(SDL_Renderer* renderer);
};

#endif // RENDERER_HPP

