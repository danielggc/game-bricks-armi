#include "renderer.hpp"
#include <cmath>

// ============================================================================
// Renderer Implementation
// ============================================================================

void Renderer::drawRect(SDL_Renderer* renderer, int x, int y, int w, int h,
                        Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderDrawRect(renderer, &rect);
}

void Renderer::fillRect(SDL_Renderer* renderer, int x, int y, int w, int h,
                        Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
}

void Renderer::drawLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2,
                        Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void Renderer::drawCircle(SDL_Renderer* renderer, int x, int y, int radius,
                          Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    int d = (5 - radius * 4) / 8;
    int cx = 0, cy = radius;
    
    while (cx <= cy) {
        SDL_RenderDrawPoint(renderer, x + cx, y + cy);
        SDL_RenderDrawPoint(renderer, x - cx, y + cy);
        SDL_RenderDrawPoint(renderer, x + cx, y - cy);
        SDL_RenderDrawPoint(renderer, x - cx, y - cy);
        SDL_RenderDrawPoint(renderer, x + cy, y + cx);
        SDL_RenderDrawPoint(renderer, x - cy, y + cx);
        SDL_RenderDrawPoint(renderer, x + cy, y - cx);
        SDL_RenderDrawPoint(renderer, x - cy, y - cx);
        
        if (d < 0) {
            d = d + 2 * cx + 1;
        } else {
            d = d + 2 * (cx - cy) + 1;
            cy--;
        }
        cx++;
    }
}

void Renderer::fillCircle(SDL_Renderer* renderer, int x, int y, int radius,
                          Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            if (dx*dx + dy*dy <= radius*radius) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

void Renderer::clear(SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderClear(renderer);
}

void Renderer::present(SDL_Renderer* renderer) {
    SDL_RenderPresent(renderer);
}

