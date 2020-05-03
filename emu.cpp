#include "emu.h"
#include "memory.h"
#include "cpu.h"
#include "iostream"
#include "fstream"
Emulator::Emulator() {
	load_cartridge();
	mem = new Memory(cartridge_mem);
	cpu = new CPU(mem);
}
void Emulator::tick() {
	unsigned int cycles = 0;
	while (cycles < MAX_CYCLES) {
		// Execute opcode and add cycles of opcode to counter
		cycles += cpu->execute_opcode();

		// Update timers
		update_timers(cycles);
		// Update graphics
		// Perform interrupts
	}
	// Render frame
}

void Emulator::load_cartridge() {
	// Clear mem
	memset(cartridge_mem, 0, sizeof(cartridge_mem));

	FILE* f;
	f = fopen("test1.gb", "rb");
	if (f == NULL) {
		printf("Error opening ROM\n");
	}
	fread(cartridge_mem, 1, 0x200000, f);
	fclose(f);
}

void Emulator::update_timers(int cycles) {
	update_divider(cycles);
    if (clock_enabled()) {
        timer_counter -= cycles;

        if (timer_counter <= 0) {

			set_clk_freq();

            // Timer overflow
            if (mem->read_mem(TIMA) == 0xFF) {
                mem->write_mem(TIMA, mem->read_mem(TMA));
            } else {
                mem->write_mem(TIMA, mem->read_mem(TIMA) + 1);
            }
        }
    }
}

void Emulator::update_divider(int cycles) {
	divide_counter += cycles;
	if (divide_counter >= 0xFF) {
		divide_counter = 0;
		mem->inc_divider_register();
	}
}
void Emulator::set_clk_freq() {
	timer_counter = mem->map_timer_counter(mem->read_mem(TMC));
}

bool Emulator::clock_enabled() {
	return BIT_CHECK(mem->read_mem(TMC), 2);
}