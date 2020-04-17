#include "cpu.h"
#include "memory.h"

CPU::CPU(Memory* RAM_ptr) {
	RAM = RAM_ptr;
}

void CPU::init() {
	PC = 0x100;
	reg_AF.val = 0x01B0;
	reg_BC.val = 0x0013;
	reg_DE.val = 0x00D8;
	reg_HL.val = 0x014D;
	SP.val = 0xFFFE;
}

unsigned int CPU::execute_opcode() {
	BYTE opcode = 0x0;

	//Fetch opCode
	opcode = (RAM->read_mem(PC) << 8 | (RAM->read_mem(PC + 1)));
	switch (opcode) {

	// LD nn,n
	case 0x06: LOAD_8BIT(&reg_BC.hi); break;
	case 0x0E: LOAD_8BIT(&reg_BC.lo); break;
	case 0x16: LOAD_8BIT(&reg_DE.hi); break;
	case 0x1E: LOAD_8BIT(&reg_DE.lo); break;
	case 0x26: LOAD_8BIT(&reg_HL.hi); break;
	case 0x2E: LOAD_8BIT(&reg_HL.lo); break;

	/** LD r1, r2 **/

		// A
	case 0x7F: LOAD_8BIT_REG(&reg_AF.hi, &reg_AF.hi); break;
	case 0x78: LOAD_8BIT_REG(&reg_AF.hi, &reg_BC.hi); break;
	case 0x79: LOAD_8BIT_REG(&reg_AF.hi, &reg_BC.lo); break;
	case 0x7A: LOAD_8BIT_REG(&reg_AF.hi, &reg_DE.hi); break;
	case 0x7B: LOAD_8BIT_REG(&reg_AF.hi, &reg_DE.lo); break;
	case 0x7C: LOAD_8BIT_REG(&reg_AF.hi, &reg_HL.hi); break;
	case 0x7D: LOAD_8BIT_REG(&reg_AF.hi, &reg_HL.lo); break;
	case 0x7E: LOAD_8BIT_FROM_MEM(&reg_AF.hi, reg_HL.val, 0x0); break;

		// B
	case 0x40: LOAD_8BIT_REG(&reg_BC.hi, &reg_BC.hi); break;
	case 0x41: LOAD_8BIT_REG(&reg_BC.hi, &reg_BC.lo); break;
	case 0x42: LOAD_8BIT_REG(&reg_BC.hi, &reg_DE.hi); break;
	case 0x43: LOAD_8BIT_REG(&reg_BC.hi, &reg_DE.lo); break;
	case 0x44: LOAD_8BIT_REG(&reg_BC.hi, &reg_HL.hi); break;
	case 0x45: LOAD_8BIT_REG(&reg_BC.hi, &reg_HL.lo); break;
	case 0x46: LOAD_8BIT_FROM_MEM(&reg_BC.hi, reg_HL.val, 0x0); break;

		// C
	case 0x48: LOAD_8BIT_REG(&reg_BC.lo, &reg_BC.hi); break;
	case 0x49: LOAD_8BIT_REG(&reg_BC.lo, &reg_BC.lo); break;
	case 0x4A: LOAD_8BIT_REG(&reg_BC.lo, &reg_DE.hi); break;
	case 0x4B: LOAD_8BIT_REG(&reg_BC.lo, &reg_DE.lo); break;
	case 0x4C: LOAD_8BIT_REG(&reg_BC.lo, &reg_HL.hi); break;
	case 0x4D: LOAD_8BIT_REG(&reg_BC.lo, &reg_HL.lo); break;
	case 0x4E: LOAD_8BIT_FROM_MEM(&reg_BC.lo, reg_HL.val, 0x0); break;
		
		// D
	case 0x50: LOAD_8BIT_REG(&reg_DE.hi, &reg_BC.hi); break;
	case 0x51: LOAD_8BIT_REG(&reg_DE.hi, &reg_BC.lo); break;
	case 0x52: LOAD_8BIT_REG(&reg_DE.hi, &reg_DE.hi); break;
	case 0x53: LOAD_8BIT_REG(&reg_DE.hi, &reg_DE.lo); break;
	case 0x54: LOAD_8BIT_REG(&reg_DE.hi, &reg_HL.hi); break;
	case 0x55: LOAD_8BIT_REG(&reg_DE.hi, &reg_HL.lo); break;
	case 0x56: LOAD_8BIT_FROM_MEM(&reg_DE.hi, reg_HL.val, 0x0); break;

		// E
	case 0x58: LOAD_8BIT_REG(&reg_DE.lo, &reg_BC.hi); break;
	case 0x59: LOAD_8BIT_REG(&reg_DE.lo, &reg_BC.lo); break;
	case 0x5A: LOAD_8BIT_REG(&reg_DE.lo, &reg_DE.hi); break;
	case 0x5B: LOAD_8BIT_REG(&reg_DE.lo, &reg_DE.lo); break;
	case 0x5C: LOAD_8BIT_REG(&reg_DE.lo, &reg_HL.hi); break;
	case 0x5D: LOAD_8BIT_REG(&reg_DE.lo, &reg_HL.lo); break;
	case 0x5E: LOAD_8BIT_FROM_MEM(&reg_DE.lo, reg_HL.val, 0x0); break;

		// H
	case 0x60: LOAD_8BIT_REG(&reg_HL.hi, &reg_BC.hi); break;
	case 0x61: LOAD_8BIT_REG(&reg_HL.hi, &reg_BC.lo); break;
	case 0x62: LOAD_8BIT_REG(&reg_HL.hi, &reg_DE.hi); break;
	case 0x63: LOAD_8BIT_REG(&reg_HL.hi, &reg_DE.lo); break;
	case 0x64: LOAD_8BIT_REG(&reg_HL.hi, &reg_HL.hi); break;
	case 0x65: LOAD_8BIT_REG(&reg_HL.hi, &reg_HL.lo); break;
	case 0x66: LOAD_8BIT_FROM_MEM(&reg_HL.hi, reg_HL.val, 0x0); break;

		// L
	case 0x68: LOAD_8BIT_REG(&reg_HL.lo, &reg_BC.hi); break;
	case 0x69: LOAD_8BIT_REG(&reg_HL.lo, &reg_BC.lo); break;
	case 0x6A: LOAD_8BIT_REG(&reg_HL.lo, &reg_DE.hi); break;
	case 0x6B: LOAD_8BIT_REG(&reg_HL.lo, &reg_DE.lo); break;
	case 0x6C: LOAD_8BIT_REG(&reg_HL.lo, &reg_HL.hi); break;
	case 0x6D: LOAD_8BIT_REG(&reg_HL.lo, &reg_HL.lo); break;
	case 0x6E: LOAD_8BIT_FROM_MEM(&reg_HL.lo, reg_HL.val, 0x0); break;

		// HL
	case 0x70: LOAD_16BIT_FROM_MEM(&reg_HL, &reg_BC.hi, 0x0); break;
	case 0x71: LOAD_16BIT_FROM_MEM(&reg_HL, &reg_BC.lo, 0x0); break;
	case 0x72: LOAD_16BIT_FROM_MEM(&reg_HL, &reg_DE.hi, 0x0); break;
	case 0x73: LOAD_16BIT_FROM_MEM(&reg_HL, &reg_DE.lo, 0x0); break;
	case 0x74: LOAD_16BIT_FROM_MEM(&reg_HL, &reg_HL.hi, 0x0); break;
	case 0x75: LOAD_16BIT_FROM_MEM(&reg_HL, &reg_HL.lo, 0x0); break;
	case 0x36: LOAD_16BIT(&reg_HL); break;


	/** LD A, n **/
	case 0x0A: LOAD_8BIT_FROM_MEM(&reg_AF.hi, reg_BC.val, 0x0); break;
	case 0x1A: LOAD_8BIT_FROM_MEM(&reg_AF.hi, reg_DE.val, 0x0); break;
	case 0xFA: LOAD_8BIT(&reg_AF.hi); break;
	case 0x3E: LOAD_8BIT(&reg_AF.hi); break;


	/** LD n, A **/
	case 0x47: LOAD_8BIT_REG(&reg_BC.hi, &reg_AF.hi); break;
	case 0x4F: LOAD_8BIT_REG(&reg_BC.lo, &reg_AF.hi); break;
	case 0x57: LOAD_8BIT_REG(&reg_DE.hi, &reg_AF.hi); break;
	case 0x5F: LOAD_8BIT_REG(&reg_DE.lo, &reg_AF.hi); break;
	case 0x67: LOAD_8BIT_REG(&reg_HL.hi, &reg_AF.hi); break;
	case 0x6F: LOAD_8BIT_REG(&reg_HL.lo, &reg_AF.hi); break;
	case 0x02: LOAD_16BIT_FROM_MEM(&reg_BC, &reg_AF.hi, 0x0); break;
	case 0x12: LOAD_16BIT_FROM_MEM(&reg_DE, &reg_AF.hi, 0x0); break;
	case 0x77: LOAD_16BIT_FROM_MEM(&reg_HL, &reg_AF.hi, 0x0); break;
	case 0xEA: LOAD_8BIT_IMMEDIATE(&reg_AF.hi); break;

	/** LD A, (C) **/
	case 0xF2: LOAD_8BIT_FROM_MEM(&reg_AF.hi, (WORD)reg_BC.lo, 0xFF00); break;

	/** LD (C), A **/
	case 0xE2: LOAD_8BIT_INTO_MEM((WORD)reg_BC.lo, &reg_AF.hi, 0xFF00); break;
	
	/** LDD A,(HL) **/
	case 0x3A: LDD_A_HL(); break;

	/** LDD (HL), A **/
	case 0x32: LDD_HL_A(); break;

	/** LDI A, (HL) **/
	case 0x2A: LDI_A_HL(); break;

	/** LDI (HL), A **/
	case 0x22: LDI_HL_A(); break;

	/** LDH (n), A **/
	case 0xE0: LDH_N_A(); break;

	/** LDH A, (n) **/
	case 0xF0: LDH_A_N(); break;

	/** LD n, nn **/
	case 0x01: LOAD_16BIT_IMMEDIATE(&reg_BC);
	case 0x11: LOAD_16BIT_IMMEDIATE(&reg_DE);
	case 0x21: LOAD_16BIT_IMMEDIATE(&reg_HL);
	case 0x31: LOAD_16BIT_IMMEDIATE(&SP);


	/** LD SP, HL**/
	case 0xF9: LOAD_16BIT_REG(&SP, &reg_HL);
	}
	return opcode_cycles[opcode];
}

/** 8 bit **/

void CPU::LOAD_8BIT(BYTE* reg) {
	*reg = RAM->read_mem(PC);
	PC += 1;
}

void CPU::LOAD_8BIT_IMMEDIATE(BYTE* reg) {
	*reg = RAM->read_mem((RAM->read_mem(PC) | RAM->read_mem(PC + 1)));
	PC += 2;
}

void CPU::LOAD_8BIT_REG(BYTE* r1, BYTE* r2) {
	*r1 = *r2;
}

void CPU::LOAD_8BIT_FROM_MEM(BYTE* reg, WORD addr, WORD immediate) {
	*reg = RAM->read_mem(addr + immediate);
}

void CPU::LOAD_8BIT_INTO_MEM(WORD addr, BYTE* reg, WORD immediate) {
	RAM->write_mem(addr + immediate, *reg);
}

void CPU::LDD_A_HL() {
	reg_AF.hi = RAM->read_mem(reg_HL.val);
	reg_HL.val--;
	PC += 1;
}

void CPU::LDD_HL_A() {
	RAM->write_mem(reg_HL.val, reg_AF.hi);
	reg_HL.val--;
	PC += 1;
}

void CPU::LDI_A_HL() {
	reg_AF.hi = RAM->read_mem(reg_HL.val);
	reg_HL.val++;
	PC += 1;
}

void CPU::LDI_HL_A() {
	RAM->write_mem(reg_HL.val, reg_AF.hi);
	reg_HL.val++;
	PC += 1;
}

void CPU::LDH_A_N() {
	RAM->write_mem(0xFF00 + RAM->read_mem(PC), reg_AF.hi);
	PC += 3;
}

void CPU::LDH_N_A() {
	reg_AF.hi = RAM->read_mem(0xFF00 + RAM->read_mem(PC));
	PC += 3;
}


/** 16 bit **/

void CPU::LOAD_16BIT_IMMEDIATE(Register* reg) {
	reg->val = RAM->read_mem((RAM->read_mem(PC) | RAM->read_mem(PC + 1)));
	PC += 2;
}


void CPU::LOAD_16BIT(Register* reg) {
	reg->val = (RAM->read_mem(PC) | RAM->read_mem(PC + 1));
	PC += 2;
}


void CPU::LOAD_16BIT_FROM_MEM(Register* reg_16, BYTE* reg, WORD immediate) {
	reg_16->val = RAM->read_mem(*reg + immediate);
}

void CPU::LOAD_16BIT_REG(Register* r1, Register* r2) {
	r1->val = r2->val;
}