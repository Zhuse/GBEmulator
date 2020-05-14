#include "memory.h"
#include "iostream"

Memory::Memory(BYTE* cartridge_ptr) {
    cartridge = cartridge_ptr;
    init();
}

void Memory::init() {
    for (int i = 0; i < 0xFFFF; i++) {
        RAM[i] = 0x0;
    }
    for (int i = 0x0; i < 0x8000; i++) {
        RAM[i] = cartridge[i];
    }
    RAM[0xFF05] = 0x00;
    RAM[0xFF06] = 0x00;
    RAM[0xFF07] = 0x00;
    RAM[0xFF10] = 0x80;
    RAM[0xFF11] = 0xBF;
    RAM[0xFF12] = 0xF3;
    RAM[0xFF14] = 0xBF;
    RAM[0xFF16] = 0x3F;
    RAM[0xFF17] = 0x00;
    RAM[0xFF19] = 0xBF;
    RAM[0xFF1A] = 0x7F;
    RAM[0xFF1B] = 0xFF;
    RAM[0xFF1C] = 0x9F;
    RAM[0xFF1E] = 0xBF;
    RAM[0xFF20] = 0xFF;
    RAM[0xFF21] = 0x00;
    RAM[0xFF22] = 0x00;
    RAM[0xFF23] = 0xBF;
    RAM[0xFF24] = 0x77;
    RAM[0xFF25] = 0xF3;
    RAM[0xFF26] = 0xF1;
    RAM[0xFF40] = 0x91;
    RAM[0xFF42] = 0x00;
    RAM[0xFF43] = 0x00;
    RAM[0xFF45] = 0x00;
    RAM[0xFF47] = 0xFC;
    RAM[0xFF48] = 0xFF;
    RAM[0xFF49] = 0xFF;
    RAM[0xFF4A] = 0x00;
    RAM[0xFF4B] = 0x00;
    RAM[0xFFFF] = 0x00;
}
void Memory::write_mem(WORD addr, BYTE data) {

    if (addr == 0xFF02 && data == 0x81) {
        std::cout << read_mem(0xFF01);
        return;
    }

    if (addr < 0x8000)
    {
        return;
    }

    // Duplicate echo RAM
    else if ((addr >= 0xE000) && (addr < 0xFE00))
    {
        RAM[addr] = data;
        write_mem(addr - 0x2000, data);
    }

    else if ((addr >= 0xFEA0) && (addr < 0xFEFF))
    {
        return;
    }

    else if (addr == TMC)
    {
        return;
    }
    else if (addr == 0xFF04) 
    {
        RAM[addr] = 0;
    }
    else if (addr == CURR_SCANLINE) {
        RAM[addr] = 0;
    }
    else if (addr == DMA_TRANSFER) {
        dma_transfer(data);
    }
    else
    {
        RAM[addr] = data;
    }
}

void Memory::dma_transfer(BYTE data) {
    WORD spr_base = data << 8;
    for (int i = 0; i < 0xA0; i++) {
        write_mem(OAM_BASE + i, read_mem(spr_base + i));
    }
}

BYTE Memory::write_mem_timer(BYTE data) {
    RAM[TMC] = data;
    BYTE new_freq = read_mem(TMC);

    return map_timer_counter(new_freq);
}

BYTE Memory::map_timer_counter(BYTE freq) {
    switch (freq)
    {
        case 0x00: return 1024;
        case 0x01: return 16;
        case 0x10: return 64;
        case 0x11: return 256;
    }
}

void Memory::inc_divider_register() {
    RAM[0xFF04]++;
}

void Memory::inc_scanline_register() {
    RAM[0xFF44]++;
}

BYTE Memory::get_clk_freq() const {
    return read_mem(TMC) & 0x11;
}
// read memory should never modify member variables hence const
BYTE Memory::read_mem(WORD addr) const
{
    if ((addr >= 0x4000) && (addr <= 0x7FFF))
    {
    }

    else if ((addr >= 0xA000) && (addr <= 0xBFFF))
    {

    }

    // else return memory
    return RAM[addr];
}