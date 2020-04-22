#include "emu.h"
#include "iostream"

int main() {
	Emulator *emu = new Emulator();
	while (true) {
		emu->tick();
	}
	return 0;
}