#include "memory.h"
#include "iostream"

Memory::Memory(BYTE* cartridge_ptr) {
    cartridge = cartridge_ptr;
    joypad_state = 0xFF;
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
    else if (addr == DIVIDER_REG) 
    {
        RAM[addr] = 0;
    }
    else if (addr == CURR_SCANLINE) {
        RAM[addr] = 0;
    }
    else if (addr == DMA_TRANSFER) {
        dma_transfer(data);
    }
    else if (addr == JOYPAD) {
        BYTE joypad_data = RAM[addr];
        BIT_CHECK(data, 5) ? BIT_SET(joypad_data, 5) : BIT_CLEAR(joypad_data, 5);
        BIT_CHECK(data, 4) ? BIT_SET(joypad_data, 4) : BIT_CLEAR(joypad_data, 4);
        RAM[addr] = joypad_data;
        return;
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

void Memory::write_to_joypad(BYTE idx, bool pressed) {
    if (idx >= 0 && idx < 8) {
        pressed ? BIT_CLEAR(joypad_state, idx) : BIT_SET(joypad_state, idx);
    }
}

BYTE Memory::get_joypad_state() {
    return joypad_state;
}
BYTE Memory::write_mem_timer(BYTE data) {
    RAM[TMC] = data;
    BYTE new_freq = read_mem(TMC);

    return map_timer_counter(new_freq);
}

WORD Memory::map_timer_counter(BYTE freq) {
    WORD mapped_freq;
    switch (freq)
    {
    case 0x0: mapped_freq = 256; break;
    case 0x1: mapped_freq = 4; break;
    case 0x2: mapped_freq = 16; break;
    case 0x3: mapped_freq = 64; break;
    default: mapped_freq = 256; break;
    }

    return mapped_freq;
}

void Memory::inc_divider_register() {
    RAM[DIVIDER_REG]++;
}

void Memory::inc_scanline_register() {
    RAM[CURR_SCANLINE]++;
}

BYTE Memory::get_clk_freq() const {
    return read_mem(TMC) & 0x3;
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
    else if (addr == JOYPAD) {
        BYTE joypad = RAM[addr];
        bool select = BIT_CHECK(joypad, 5);
        bool directional = BIT_CHECK(joypad, 4);
        BYTE hi_nibble = (joypad & 0x30);
        BYTE lo_nibble;

        if (select && !directional) {
            lo_nibble = joypad_state & 0x0F;
        }
        else if (directional && !select) {
            lo_nibble = (joypad_state & 0xF0) >> 4;
        }
        else {
            return joypad;
        }
        return hi_nibble | lo_nibble;
    }
    // else return memory
    return RAM[addr];
}