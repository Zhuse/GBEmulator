#include "emu.h"
#include "memory.h"
#include "cpu.h"
#include "ppu.h"
#include "divider_counter.h"
#include "main_timer.h"
#include "iostream"
#include "fstream"

Emulator::Emulator() {
	load_cartridge();
	mem = new Memory(cartridge_mem);
	cpu = new CPU(mem);
	ppu = new PPU(mem, cpu);
	div_reg = new DividerCounter(mem);
	timer = new MainTimer(mem, cpu);
}
void Emulator::tick() {
	unsigned int tick_cycles = 0;
	while (tick_cycles < MAX_CYCLES) {
		// Execute opcode and add cycles of opcode to counter
		BYTE cycles = cpu->exec();
		tick_cycles += cycles;

		// Update timers
		update_timers(cycles);

		// Update graphics
		ppu->draw(cycles);

		// Perform interrupts
		cpu->serve_interrupts();

	}
	// Render frame
}

void Emulator::load_cartridge() {
	// Clear mem
	memset(cartridge_mem, 0, sizeof(cartridge_mem));

	FILE* f;
	f = fopen("castle.gb", "rb");
	if (f == NULL) {
		printf("Error opening ROM\n");
	}
	fread(cartridge_mem, 1, 0x200000, f);
	fclose(f);
}

void Emulator::update_timers(int cycles) {
	div_reg->update_timer(cycles);
	timer->update_timer(cycles);
}

void Emulator::register_keypress(BYTE key) {
	BYTE joypad_reg = mem->read_mem(JOYPAD);
	BYTE joypad_state = mem->get_joypad_state();
	bool select = BIT_CHECK(joypad_reg, 5);
	bool directional = BIT_CHECK(joypad_reg, 4);

	if (select && key > 0x3) {
		if (BIT_CHECK(joypad_state, key)) {
			cpu->req_interrupt(4);
		}
		mem->write_to_joypad(key, true);
	}
	else if (directional && key <= 0x3) {
		if (BIT_CHECK(joypad_state, key)) {
			cpu->req_interrupt(4);
		}
		mem->write_to_joypad(key, true);
	}
}

void Emulator::unregister_keypress(BYTE key) {
	mem->write_to_joypad(key, false);
}


std::array<std::array<BYTE, 3>, SCREEN_W>* Emulator::get_screen() {
	return ppu->screen.data();
}