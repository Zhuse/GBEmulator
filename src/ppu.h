#ifndef PPU_H
#define PPU_H

#include "constants.h"
#include "array"

class CPU;
class Memory;
class PPU {
    private:
        Memory* mem;
        CPU* cpu;
        uint16_t scanline_counter;
        void draw_scanline();
        void draw_tiles(uint8_t lcd_status_reg, bool window);
        void assign_colour(uint8_t x, uint8_t y, uint8_t code);
        void draw_sprites();
        void set_lcd_status();

    public: 
        std::array<std::array<std::array<uint8_t, 3>, GraphicSpecs::SCREEN_W>, GraphicSpecs::SCREEN_H> screen;

        PPU(Memory* mem_ptr, CPU* cpu_ptr);
        void draw(int cycles);
};

#endif

