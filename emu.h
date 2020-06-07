#include "constants.h"
#include "array"
class Memory;
class CPU;
class PPU;
class Emulator
{
private:
    const unsigned int MAX_CYCLES = 17476;
    Memory* mem;
    CPU* cpu;
    PPU* ppu;
    WORD timer_counter = 0;
    WORD divide_counter = 0;
    WORD timer_limit = 256;
    void update_timers(int cycles);
    void update_divider(int cycles);
    bool clock_enabled();
    void set_timer_freq();
public:
    BYTE cartridge_mem[0x200000];
    Emulator();
    void tick();
    void load_cartridge();
    void register_keypress(BYTE key);
    void unregister_keypress(BYTE key);
    std::array<std::array<BYTE, 3>, SCREEN_W>*  get_screen();
};