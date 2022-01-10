#include "display.hpp"
#include <SDL.h>
#include <cstdlib>
#undef main
#define WINDOW_WIDTH 600

void display() {
    SDL_Event event;
    SDL_Renderer* renderer;
    SDL_Window* window;
    int i;
    int j;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_WIDTH, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderSetScale(renderer, 10, 10);
    for (i = 0; i < WINDOW_WIDTH; i++)
    {
        for (j = 0; j < WINDOW_WIDTH; j++) {
            if ((i + (j % 2)) % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
            }
            else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            }

            SDL_RenderDrawPoint(renderer, i, j);
        }
    }

    SDL_RenderPresent(renderer);
    while (1) {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}