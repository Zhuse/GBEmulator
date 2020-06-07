#include "constants.h"
#include "array"

class CPU;
class Memory;
class PPU {
    private:
        Memory* mem;
        CPU* cpu;
        WORD scanline_counter;
        void draw_scanline();
        void draw_tiles(BYTE lcd_status_reg, bool window);
        void assign_colour(BYTE x, BYTE y, BYTE code);
        void draw_sprites();
        void set_lcd_status();

    public: 
        std::array<std::array<std::array<BYTE, 3>, SCREEN_W>, SCREEN_H> screen;

        PPU(Memory* mem_ptr, CPU* cpu_ptr);
        void draw(int cycles);
};

