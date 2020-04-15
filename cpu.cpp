#include "cpu.h"

CPU::CPU() {}

void CPU::init() {
	PC = 0x100;
	reg_AF.val = 0x01B0;
	reg_BC.val = 0x0013;
	reg_DE.val = 0x00D8;
	reg_HL.val = 0x014D;
	SP.val = 0xFFFE;
}