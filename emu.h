#include "constants.h"

class Memory;
class CPU;
class Emulator
{
private:
    const unsigned int MAX_CYCLES = 17476;
    Memory* mem;
    CPU* cpu;
    BYTE screen[160][144][3];
    BYTE timer_counter;
    BYTE divide_counter;
    BYTE scanline_counter;
    void update_timers(int cycles);
    void update_divider(int cycles);
    bool clock_enabled();
    void set_clk_freq();
    void draw(int cycles);
    void draw_scanline();
    void draw_bg(BYTE lcd_status_reg);
    void assign_colour(BYTE x, BYTE y, BYTE code);
    void draw_window();
    void draw_sprites();
    void set_lcd_status();
public:
    BYTE cartridge_mem[0x200000];

    Emulator();
    void tick();
    void load_cartridge();
    
};