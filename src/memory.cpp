#include "memory.h"
#include "iostream"
#include "cstdint"

using namespace Addresses;
using namespace MemorySpecs;

Memory::Memory(uint8_t* cartridge_ptr) {
    cartridge = cartridge_ptr;
    joypad_state = 0xFF;
    reset_timer_flag = false;
    ram_enable = false;
    rom_bank_num = 0x1;
    ram_bank_num = 0x0;
    ram_select = true;
    init();
}

void Memory::init() {
    for (int i = 0; i < sizeof(RAM); i++) {
        RAM[i] = 0x0;
    }
    for (int i = 0x0; i < 0x8000; i++) {
        RAM[i] = cartridge[i];
    }

    /* Get ROM banking type */
    set_bank_type();

    /* Set initial memory state documented after BOOT ROM */
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

void Memory::set_bank_type() {
    uint8_t type = cartridge[0x147];

    switch (type) {
    case 0x5:
    case 0x6: bank_type = 2; break;
    case 0x1:
    case 0x2:
    case 0x3:
    default: bank_type = 1; break;
    }
}
void Memory::write_mem(uint16_t addr, uint8_t data) {

    /* For specific test ROMS that print results via I/O */
    if (addr == 0xFF02 && data == 0x81) {
        std::cout << read_mem(0xFF01);
        return;
    }

    /* Swap ROM/RAM banks depending on input */
    if (addr < 0x8000)
    {
        bank_swap(addr, data);
    }

    /* Echo RAM */
    else if ((addr >= 0xE000) && (addr < 0xFE00))
    {
        RAM[addr] = data;
        write_mem(addr - 0x2000, data);
    }

    /* Invalid write address */
    else if ((addr >= 0xFEA0) && (addr < 0xFEFF))
    {
        return;
    }

    /* Reset divider register if written to */
    else if (addr == DIVIDER_REG) 
    {
        RAM[addr] = 0;
    }

    /* Reset current scanline register if written to */
    else if (addr == CURR_SCANLINE) {
        RAM[addr] = 0;
    }
    else if (addr == TIMA) {
        reset_timer_flag = true;
        RAM[addr] = data;
    }
    else if (addr == DMA_TRANSFER) {
        dma_transfer(data);
    }

    /* Determine type of button pressed and parse joypad input */
    else if (addr == JOYPAD) {
        uint8_t joypad_data = RAM[addr];
        BIT_CHECK(data, 5) ? BIT_SET(joypad_data, 5) : BIT_CLEAR(joypad_data, 5);
        BIT_CHECK(data, 4) ? BIT_SET(joypad_data, 4) : BIT_CLEAR(joypad_data, 4);
        RAM[addr] = joypad_data;
        return;
    }

    /* Else write the data */
    else
    {
        RAM[addr] = data;
    }
}

void Memory::bank_swap(uint16_t addr, uint8_t data) {
    if (bank_type == 1) {
        if (addr >= 0x0 && addr < 0x2000) {
            if (data == 0xA) {
                ram_enable = true;
            }
            else {
                ram_enable = false;
            }
        }
        else if (addr >= 0x2000 && addr < 0x4000) {
            uint8_t lo_five_mask = 0b11111;
            uint8_t new_bank_num = data & lo_five_mask;
            if (new_bank_num == 0x0 || new_bank_num == 0x20 || new_bank_num == 0x40 || new_bank_num == 0x60) {
                new_bank_num++;
            }
            rom_bank_num = new_bank_num;
            write_rom_bank();
        }
        else if (addr >= 0x4000 && addr < 0x6000) {
            uint8_t new_bank_num = data & 0b11;
            if (ram_select) {
                swap_ram_bank(new_bank_num);
            }
            else {
                write_rom_bank();
            }
        }
        else if (addr >= 0x6000 && addr < 0x8000) {
            uint8_t new_data = data & 0x1;
            if (new_data) {
                ram_select = true;
                write_ram_bank();
            }
            else {
                ram_select = false;
                write_rom_bank();
            }
        }
    }
}

void Memory::write_rom_bank() {
    uint8_t new_rom_bank = rom_bank_num;
    if (!ram_select) {
        new_rom_bank &= (ram_bank_num << 5);
    }
    
    unsigned int new_bank_addr = new_rom_bank * ROM_BANK_SIZE;
    for (unsigned int idx = 0; idx < ROM_BANK_SIZE; idx++) {
        RAM[0x4000 + idx] = cartridge[new_bank_addr + idx];
    }
}

void Memory::swap_ram_bank(uint8_t new_bank_num) {
    uint8_t old_ram_bank = ram_bank_num;

    unsigned int old_bank_addr = old_ram_bank * RAM_BANK_SIZE;

    if (ram_enable && ram_select) {
        for (unsigned int idx = 0; idx < RAM_BANK_SIZE; idx++) {
            RAM_BANKS[old_bank_addr + idx] = RAM[0xA000 + idx];
        }
        ram_bank_num = new_bank_num;
        write_ram_bank();
    }
}

void Memory::write_ram_bank() {
    unsigned int new_bank_addr = ram_bank_num * RAM_BANK_SIZE;

    if (ram_enable && ram_select) {
        for (unsigned int idx = 0; idx < RAM_BANK_SIZE; idx++) {
            RAM[0xA000 + idx] = RAM_BANKS[new_bank_addr + idx];
        }
    }
}

void Memory::dma_transfer(uint8_t data) {
    uint16_t spr_base = data << 8;
    for (int i = 0; i < 0xA0; i++) {
        write_mem(OAM_BASE + i, read_mem(spr_base + i));
    }
}

void Memory::write_to_joypad(uint8_t idx, bool pressed) {
    if (idx >= 0 && idx < 8) {
        pressed ? BIT_CLEAR(joypad_state, idx) : BIT_SET(joypad_state, idx);
    }
}

void Memory::disable_timer_flag() {
    reset_timer_flag = false;
}

bool Memory::get_timer_flag() {
    return reset_timer_flag;
}

uint8_t Memory::get_joypad_state() {
    return joypad_state;
}

void Memory::inc_divider_register() {
    RAM[DIVIDER_REG]++;
}

void Memory::inc_scanline_register() {
    RAM[CURR_SCANLINE]++;
}

uint8_t Memory::get_clk_freq() const {
    return read_mem(TMC) & 0x3;
}

uint8_t Memory::read_mem(uint16_t addr) const
{
    if (addr == JOYPAD) {
        uint8_t joypad = RAM[addr];
        bool select = BIT_CHECK(joypad, 5);
        bool directional = BIT_CHECK(joypad, 4);
        uint8_t hi_nibble = (joypad & 0x30);
        uint8_t lo_nibble;

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
    return RAM[addr];
}