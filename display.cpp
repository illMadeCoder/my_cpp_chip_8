#include "display.hpp"
#include <SDL.h>
#include <cstdlib>
#undef main
#define WINDOW_WIDTH 64
#define WINDOW_HEIGHT 32

void output_display(char * display) {
    SDL_Event event;
    SDL_Renderer* renderer;
    SDL_Window* window;
    int i;
    int j;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH*10, WINDOW_HEIGHT*10, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderSetScale(renderer, 10, 10);
    for (int i = 0; i < WINDOW_HEIGHT; i++) {
        for (int j = 0; j < WINDOW_WIDTH; j++) {
            /*The original implementation of the Chip - 8 language used a 64x32 - pixel monochrome display with this format:
            (0, 0)	(63, 0)
            (0, 31)	(63, 31)*/
            /*char display[32 * 8];*/
            // xxxx xxxx .. xxxx xxxx
            // ..   
            // xxxx ..
            char bit_position = j % 8;
            int byte_position = j / 8;
            // 128 = 1000 0000
            char bitmask = (0b10000000) >> bit_position;
            // divide windowwidth by 8 to normalize back into byte form
            int index = i * (WINDOW_WIDTH/8) + byte_position;
            bool filled = (display[index] & bitmask) != 0;
            int x = byte_position * 8 + bit_position;
            int y = i;

            if (filled) {
                // white
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
            }
            else {
                // black
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            }
            
            // draw
            SDL_RenderDrawPoint(renderer, x, y);
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