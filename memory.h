#include "constants.h"

class Memory {
private:
	BYTE RAM[0x10000];
	BYTE* cartridge;
	BYTE joypad_state;
	bool reset_timer_flag;
	void dma_transfer(BYTE data);
public:
	Memory(BYTE* cartridge_ptr);
	void init();
	void write_mem(WORD addr, BYTE data);
	void inc_divider_register();
	void inc_scanline_register();
	void write_to_joypad(BYTE idx, bool pressed);
	void disable_timer_flag();
	bool get_timer_flag();
	BYTE get_joypad_state();
	BYTE write_mem_timer(BYTE data);
	BYTE get_clk_freq() const;
	WORD map_timer_counter(BYTE freq);
	BYTE Memory::read_mem(WORD addr) const;
};