#include "emu.h"
#include "iostream"
#include "SDL.h"
#include "constants.h"
#include "map"

const int WINDOW_W = 160;
const int WINDOW_H = 144;
const int SCALE = 3;

std::map<SDL_Keycode, unsigned char> get_keyboard();

void draw_graphics(SDL_Renderer* renderer, BYTE screen[SCREEN_H][SCREEN_W][3]);

int main(int argc, char**argv) {
	Emulator *emu = new Emulator();

    SDL_Window* window = NULL;
    SDL_Event event;
    SDL_Keycode event_key;
    SDL_Renderer* renderer;

    std::map<SDL_Keycode, unsigned char> controller = get_keyboard();

    window = SDL_CreateWindow(
        "GBEmulator", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_W * SCALE,
        WINDOW_H * SCALE,
        SDL_WINDOW_SHOWN);

    // Setup renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	while (true) {
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                exit(0);
            }
            else if (event.type == SDL_KEYDOWN) { //Handle keyboard key press
                event_key = event.key.keysym.sym;
                if (controller.count(event_key) == 1) {
                    emu->register_keypress(controller[event_key]);
                }
            }
            else if (event.type == SDL_KEYUP) { //Handle keyboard key release
                event_key = event.key.keysym.sym;
                if (controller.count(event_key) == 1) {
                    emu->unregister_keypress(controller[event_key]);
                }
            }
        }

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

std::map<SDL_Keycode, unsigned char> get_keyboard() {
    std::map<SDL_Keycode, unsigned char> keyboard;
    SDL_Keycode controller[] = {
      SDLK_d,
      SDLK_a,
      SDLK_w,
      SDLK_s,
      SDLK_j,
      SDLK_k,
      SDLK_i,
      SDLK_u
    };
    unsigned char controller_mapped[] = {
      0x0,
      0x1,
      0x2,
      0x3,
      0x4,
      0x5,
      0x6,
      0x7
    };
    for (unsigned int i = 0; i < 8; i++) {
        keyboard.insert(std::make_pair(controller[i], controller_mapped[i]));
    }
    return keyboard;
}