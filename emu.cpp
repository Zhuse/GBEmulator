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