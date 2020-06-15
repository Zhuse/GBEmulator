#ifndef MEMORY_H
#define MEMORY_H

#include "constants.h"

class Memory {
private:
	uint8_t RAM[0x10000];
	uint8_t RAM_BANKS[0x8000];
	uint8_t* cartridge;
	uint8_t joypad_state;
	uint8_t bank_type;
	uint8_t rom_bank_num;
	uint8_t ram_bank_num;
	bool reset_timer_flag;
	bool ram_enable;
	bool ram_select;
	void dma_transfer(uint8_t data);
	void set_bank_type();
	void bank_swap(uint16_t addr, uint8_t data);
	void write_rom_bank();
	void swap_ram_bank(uint8_t new_bank_num);
	void write_ram_bank();

public:
	Memory(uint8_t* cartridge_ptr);
	void init();
	void write_mem(uint16_t addr, uint8_t data);
	void inc_divider_register();
	void inc_scanline_register();
	void write_to_joypad(uint8_t idx, bool pressed);
	void disable_timer_flag();
	bool get_timer_flag();
	uint8_t get_joypad_state();
	uint8_t get_clk_freq() const;
	uint8_t Memory::read_mem(uint16_t addr) const;
};

#endif