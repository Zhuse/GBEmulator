#include "constants.h"

class Memory;
class CPU;
class Emulator
{
private:
    const unsigned int MAX_CYCLES = 17476;
    Memory* mem;
    CPU* cpu;
    WORD timer_counter = 0;
    WORD divide_counter = 0;
    WORD scanline_counter = 0;
    WORD timer_limit = 256;
    void update_timers(int cycles);
    void update_divider(int cycles);
    bool clock_enabled();
    void set_timer_freq();
    void draw(int cycles);
    void draw_scanline();
    void draw_tiles(BYTE lcd_status_reg, bool window);
    void assign_colour(BYTE x, BYTE y, BYTE code);
    void draw_sprites();
    void set_lcd_status();
public:
    BYTE cartridge_mem[0x200000];
    BYTE screen[144][160][3];
    Emulator();
    void tick();
    void load_cartridge();
    void register_keypress(BYTE key);
    void unregister_keypress(BYTE key);
};