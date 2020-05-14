#include "emu.h"
#include "iostream"
#include "SDL.h"

const int WINDOW_W = 160;
const int WINDOW_H = 144;
const int SCALE = 3;
void draw_graphics(SDL_Renderer* renderer, BYTE screen[SCREEN_H][SCREEN_W][3]);

int main(int argc, char**argv) {
	Emulator *emu = new Emulator();

    SDL_Window* window = NULL;
    SDL_Event event;
    SDL_Keycode eventKey;
    SDL_Renderer* renderer;

    window = SDL_CreateWindow(
        "GBEmulator", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_W * SCALE,
        WINDOW_H * SCALE,
        SDL_WINDOW_SHOWN);

    // Setup renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	while (true) {
		emu->tick();
        draw_graphics(renderer, emu->screen);
	}
}

void draw_graphics(SDL_Renderer* renderer, BYTE screen[SCREEN_H][SCREEN_W][3])
{
    SDL_Rect rect;
    rect.w = SCALE;
    rect.h = SCALE;
    for (unsigned int y = 0; y < WINDOW_H; y++)
    {
        for (unsigned int x = 0; x < WINDOW_W; x++)
        {
            BYTE r = screen[y][x][0];
            BYTE g = screen[y][x][1];
            BYTE b = screen[y][x][2];
            rect.x = x * SCALE;
            rect.y = y * SCALE;
            SDL_SetRenderDrawColor(renderer, r, g, b, 100);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    SDL_RenderPresent(renderer);
}