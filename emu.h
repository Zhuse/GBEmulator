#include "constants.h"

class Memory;
class CPU;
class Emulator
{
private:
    const unsigned int MAX_CYCLES = 69905;
    Memory* mem;
    CPU* cpu;
    BYTE screen[160][144][3];

public:
    BYTE cartridge_mem[0x200000];

    Emulator();
    void tick();
    void load_cartridge();
};