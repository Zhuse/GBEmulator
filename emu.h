#ifndef EMU_H
#define EMU_H

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
    Memory* mem;
    CPU* cpu;
    PPU* ppu;
    DividerCounter* div_reg;
    MainTimer* timer;
    void update_timers(int cycles);

public:
    uint8_t cartridge_mem[0x200000];
    Emulator();
    void tick();
    void load_cartridge();
    void register_keypress(uint8_t key);
    void unregister_keypress(uint8_t key);
    std::array<std::array<uint8_t, 3>, GraphicSpecs::SCREEN_W>* get_screen();
};

#endif