#include "constants.h"
#include "array"
class Memory;
class CPU;
class PPU;
class DividerCounter;
class MainTimer;
class Emulator
{
private:
    const unsigned int MAX_CYCLES = 17476;
    Memory* mem;
    CPU* cpu;
    PPU* ppu;
    DividerCounter* div_reg;
    MainTimer* timer;
    WORD timer_counter = 0;
    WORD divide_counter = 0;
    WORD timer_limit = 256;
    void update_timers(int cycles);
public:
    BYTE cartridge_mem[0x200000];
    Emulator();
    void tick();
    void load_cartridge();
    void register_keypress(BYTE key);
    void unregister_keypress(BYTE key);
    std::array<std::array<BYTE, 3>, SCREEN_W>* get_screen();
};