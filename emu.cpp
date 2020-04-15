#include "emu.h"
#include "iostream"
#include "fstream"
Emulator::Emulator() {
}
void Emulator::tick() {
	unsigned int cycles = 0;
	printf("Emulator ticked\n");
	while (cycles < MAX_CYCLES) {
		// Execute opcode and add cycles of opcode to counter
		// Update timers
		// Update graphics
		// Perform interrupts
	}
	// Render frame
}

void Emulator::init_mem() {
	// Init memory registers
}

void Emulator::load_cartridge() {
	// Clear mem
	memset(cartridge_mem, 0, sizeof(cartridge_mem));

	FILE* f;
	f = fopen("Tetris.gb", "rb");
	if (f == NULL) {
		printf("Error opening ROM\n");
	}
	std::cout << f;
	fread(cartridge_mem, 1, 0x200000, f);
	fclose(f);
}