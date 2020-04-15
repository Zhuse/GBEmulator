#include "emu.h"
#include "iostream"

int main() {
	Emulator *emu = new Emulator();
	emu->load_cartridge();
	emu->tick();
	return 0;
}