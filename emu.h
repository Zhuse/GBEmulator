#include "constants.h"

class Emulator
{
private:
    const unsigned int MAX_CYCLES = 69905;
    BYTE screen[160][144][3];

public:
    BYTE cartridge_mem[0x200000];

    Emulator();
    void tick();
    void init_mem();
    void load_cartridge();
};