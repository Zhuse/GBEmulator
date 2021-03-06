#include "cpu.h"
#include "memory.h"
#include "iostream"

using namespace Addresses;

CPU::CPU(Memory* RAM_ptr) {
	init();
	RAM = RAM_ptr;
	trace = fopen("trace.txt", "w");
}

void CPU::init() {
	PC = 0x100;
	reg_AF.val = 0x01B0;
	reg_BC.val = 0x0013;
	reg_DE.val = 0x00D8;
	reg_HL.val = 0x014D;
	SP.val = 0xFFFE;

}	

unsigned int CPU::exec() {
	check_ie_activation();
	check_ie_deactivation();
	return execute_opcode();
}

void CPU::check_ie_activation() {
	if (pending_activation) {
		pending_activation = false;
		master_interrupt = true;
	}
}

void CPU::check_ie_deactivation() {
	if (pending_deactivation) {
		pending_deactivation = false;
		master_interrupt = false;
	}
}

void CPU::serve_interrupt(int idx) {
	master_interrupt = false;
	uint8_t req = RAM->read_mem(IF);
	BIT_CLEAR(req, idx);
	RAM->write_mem(IF, req);

	PUSH_NN(PC);

	switch (idx)
	{
	case 0: PC = 0x40; break;
	case 1: PC = 0x48; break;
	case 2: PC = 0x50; break;
	case 4: PC = 0x60; break;
	}
}

void CPU::req_interrupt(int idx)
{
	uint8_t req = RAM->read_mem(IF);
	BIT_SET(req, idx);
	RAM->write_mem(IF, req);
}

void CPU::serve_interrupts() {
	uint8_t reqs = RAM->read_mem(IF);
	uint8_t ie = RAM->read_mem(IE);

	if (master_interrupt || halted) {
		for (int i = 0; i < 5; i++) {
			bool curr = BIT_CHECK(reqs, i);
			if (curr && BIT_CHECK(ie, i)) {
				if (halted) {
					halted = false;
					PC++;
				}
				if (master_interrupt) {
					serve_interrupt(i);
				}
			}
		}
	}
}


unsigned int CPU::execute_opcode() {
	uint8_t opcode = 0x0;

	/* Fetch */

	opcode = RAM->read_mem(PC);
	PC++;

	/* Decode/Execute */
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
	case 0x70: LOAD_16BIT_TO_MEM(&reg_HL, &reg_BC.hi); break;
	case 0x71: LOAD_16BIT_TO_MEM(&reg_HL, &reg_BC.lo); break;
	case 0x72: LOAD_16BIT_TO_MEM(&reg_HL, &reg_DE.hi); break;
	case 0x73: LOAD_16BIT_TO_MEM(&reg_HL, &reg_DE.lo); break;
	case 0x74: LOAD_16BIT_TO_MEM(&reg_HL, &reg_HL.hi); break;
	case 0x75: LOAD_16BIT_TO_MEM(&reg_HL, &reg_HL.lo); break;
	case 0x36: {
		uint8_t data = RAM->read_mem(PC);
		LOAD_16BIT_TO_MEM(&reg_HL, &data);
		PC++;
		break;
	}


	/** LD A, n **/
	case 0x0A: LOAD_8BIT_FROM_MEM(&reg_AF.hi, reg_BC.val, 0x0); break;
	case 0x1A: LOAD_8BIT_FROM_MEM(&reg_AF.hi, reg_DE.val, 0x0); break;
	case 0xFA: {
		LOAD_8BIT_FROM_MEM_IMMEDIATE(&reg_AF.hi);
		break;
	}
	case 0x3E: LOAD_8BIT(&reg_AF.hi); break;


	/** LD n, A **/
	case 0x47: LOAD_8BIT_REG(&reg_BC.hi, &reg_AF.hi); break;
	case 0x4F: LOAD_8BIT_REG(&reg_BC.lo, &reg_AF.hi); break;
	case 0x57: LOAD_8BIT_REG(&reg_DE.hi, &reg_AF.hi); break;
	case 0x5F: LOAD_8BIT_REG(&reg_DE.lo, &reg_AF.hi); break;
	case 0x67: LOAD_8BIT_REG(&reg_HL.hi, &reg_AF.hi); break;
	case 0x6F: LOAD_8BIT_REG(&reg_HL.lo, &reg_AF.hi); break;
	case 0x02: LOAD_16BIT_TO_MEM(&reg_BC, &reg_AF.hi); break;
	case 0x12: LOAD_16BIT_TO_MEM(&reg_DE, &reg_AF.hi); break;
	case 0x77: LOAD_16BIT_TO_MEM(&reg_HL, &reg_AF.hi); break;
	case 0xEA: LOAD_8BIT_IMMEDIATE(&reg_AF.hi); break;

	/** LD A, (C) **/
	case 0xF2: LOAD_8BIT_FROM_MEM(&reg_AF.hi, (uint16_t)reg_BC.lo, 0xFF00); break;

	/** LD (C), A **/
	case 0xE2: LOAD_8BIT_INTO_MEM((uint16_t)reg_BC.lo, &reg_AF.hi, 0xFF00); break;
	
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
	case 0x01: LOAD_16BIT_IMMEDIATE(&reg_BC); break;
	case 0x11: LOAD_16BIT_IMMEDIATE(&reg_DE); break;
	case 0x21: LOAD_16BIT_IMMEDIATE(&reg_HL); break;
	case 0x31: LOAD_16BIT_IMMEDIATE(&SP); break;


	/** LD SP, HL **/
	case 0xF9: LOAD_16BIT_REG(&SP, &reg_HL); break;

	/** LDHL SP, n **/
	case 0xF8: LDHL_SP(); break;

	/** LD (nn), SP **/
	case 0x08: LD_NN_SP(0x0); break;

	/** PUSH nn **/
	case 0xF5: PUSH_NN(reg_AF.val); break;
	case 0xC5: PUSH_NN(reg_BC.val); break;
	case 0xD5: PUSH_NN(reg_DE.val); break;
	case 0xE5: PUSH_NN(reg_HL.val); break;

	/** POP nn **/
	case 0xF1: POP_NN(&(reg_AF.val)); break;
	case 0xC1: POP_NN(&(reg_BC.val)); break;
	case 0xD1: POP_NN(&(reg_DE.val)); break;
	case 0xE1: POP_NN(&(reg_HL.val)); break;

	/** ADD A,n **/
	case 0x87: ADD_N_N(&reg_AF.hi, reg_AF.hi, false, false); break;
	case 0x80: ADD_N_N(&reg_AF.hi, reg_BC.hi, false, false); break;
	case 0x81: ADD_N_N(&reg_AF.hi, reg_BC.lo, false, false); break;
	case 0x82: ADD_N_N(&reg_AF.hi, reg_DE.hi, false, false); break;
	case 0x83: ADD_N_N(&reg_AF.hi, reg_DE.lo, false, false); break;
	case 0x84: ADD_N_N(&reg_AF.hi, reg_HL.hi, false, false); break;
	case 0x85: ADD_N_N(&reg_AF.hi, reg_HL.lo, false, false); break;
	case 0x86: ADD_N_N(&reg_AF.hi, RAM->read_mem(reg_HL.val), false, false); break;
	case 0xC6: ADD_N_N(&reg_AF.hi, 0x0, true, false); break;

	/** ADC A,n **/
	case 0x8F: ADD_N_N(&reg_AF.hi, reg_AF.hi, false, true); break;
	case 0x88: ADD_N_N(&reg_AF.hi, reg_BC.hi, false, true); break;
	case 0x89: ADD_N_N(&reg_AF.hi, reg_BC.lo, false, true); break;
	case 0x8A: ADD_N_N(&reg_AF.hi, reg_DE.hi, false, true); break;
	case 0x8B: ADD_N_N(&reg_AF.hi, reg_DE.lo, false, true); break;
	case 0x8C: ADD_N_N(&reg_AF.hi, reg_HL.hi, false, true); break;
	case 0x8D: ADD_N_N(&reg_AF.hi, reg_HL.lo, false, true); break;
	case 0x8E: ADD_N_N(&reg_AF.hi, RAM->read_mem(reg_HL.val), false, true); break;
	case 0xCE: ADD_N_N(&reg_AF.hi, 0x0, true, true); break;

	/** SDD A,n **/
	case 0x97: SUB_N_N(&reg_AF.hi, reg_AF.hi, false, false); break;
	case 0x90: SUB_N_N(&reg_AF.hi, reg_BC.hi, false, false); break;
	case 0x91: SUB_N_N(&reg_AF.hi, reg_BC.lo, false, false); break;
	case 0x92: SUB_N_N(&reg_AF.hi, reg_DE.hi, false, false); break;
	case 0x93: SUB_N_N(&reg_AF.hi, reg_DE.lo, false, false); break;
	case 0x94: SUB_N_N(&reg_AF.hi, reg_HL.hi, false, false); break;
	case 0x95: SUB_N_N(&reg_AF.hi, reg_HL.lo, false, false); break;
	case 0x96: SUB_N_N(&reg_AF.hi, RAM->read_mem(reg_HL.val), false, false); break;
	case 0xD6: SUB_N_N(&reg_AF.hi, 0x0, true, false); break;

	/** SDC A,n **/
	case 0x9F: SUB_N_N(&reg_AF.hi, reg_AF.hi, false, true); break;
	case 0x98: SUB_N_N(&reg_AF.hi, reg_BC.hi, false, true); break;
	case 0x99: SUB_N_N(&reg_AF.hi, reg_BC.lo, false, true); break;
	case 0x9A: SUB_N_N(&reg_AF.hi, reg_DE.hi, false, true); break;
	case 0x9B: SUB_N_N(&reg_AF.hi, reg_DE.lo, false, true); break;
	case 0x9C: SUB_N_N(&reg_AF.hi, reg_HL.hi, false, true); break;
	case 0x9D: SUB_N_N(&reg_AF.hi, reg_HL.lo, false, true); break;
	case 0x9E: SUB_N_N(&reg_AF.hi, RAM->read_mem(reg_HL.val), false, true); break;
	case 0xDE: SUB_N_N(&reg_AF.hi, 0x0, true, true); break;

	/** AND n **/
	case 0xA7: AND_N_N(&reg_AF.hi, reg_AF.hi, false); break;
	case 0xA0: AND_N_N(&reg_AF.hi, reg_BC.hi, false); break;
	case 0xA1: AND_N_N(&reg_AF.hi, reg_BC.lo, false); break;
	case 0xA2: AND_N_N(&reg_AF.hi, reg_DE.hi, false); break;
	case 0xA3: AND_N_N(&reg_AF.hi, reg_DE.lo, false); break;
	case 0xA4: AND_N_N(&reg_AF.hi, reg_HL.hi, false); break;
	case 0xA5: AND_N_N(&reg_AF.hi, reg_HL.lo, false); break;
	case 0xA6: AND_N_N(&reg_AF.hi, RAM->read_mem(reg_HL.val), false); break;
	case 0xE6: AND_N_N(&reg_AF.hi, 0x0, true); break;

	/** OR n **/
	case 0xB7: OR_N_N(&reg_AF.hi, reg_AF.hi, false); break;
	case 0xB0: OR_N_N(&reg_AF.hi, reg_BC.hi, false); break;
	case 0xB1: OR_N_N(&reg_AF.hi, reg_BC.lo, false); break;
	case 0xB2: OR_N_N(&reg_AF.hi, reg_DE.hi, false); break;
	case 0xB3: OR_N_N(&reg_AF.hi, reg_DE.lo, false); break;
	case 0xB4: OR_N_N(&reg_AF.hi, reg_HL.hi, false); break;
	case 0xB5: OR_N_N(&reg_AF.hi, reg_HL.lo, false); break;
	case 0xB6: OR_N_N(&reg_AF.hi, RAM->read_mem(reg_HL.val), false); break;
	case 0xF6: OR_N_N(&reg_AF.hi, 0x0, true); break;

	/** XOR n **/
	case 0xAF: XOR_N_N(&reg_AF.hi, reg_AF.hi, false); break;
	case 0xA8: XOR_N_N(&reg_AF.hi, reg_BC.hi, false); break;
	case 0xA9: XOR_N_N(&reg_AF.hi, reg_BC.lo, false); break;
	case 0xAA: XOR_N_N(&reg_AF.hi, reg_DE.hi, false); break;
	case 0xAB: XOR_N_N(&reg_AF.hi, reg_DE.lo, false); break;
	case 0xAC: XOR_N_N(&reg_AF.hi, reg_HL.hi, false); break;
	case 0xAD: XOR_N_N(&reg_AF.hi, reg_HL.lo, false); break;
	case 0xAE: XOR_N_N(&reg_AF.hi, RAM->read_mem(reg_HL.val), false); break;
	case 0xEE: XOR_N_N(&reg_AF.hi, 0x0, true); break;

	/** CP n **/
	case 0xBF: CP_N_N(&reg_AF.hi, reg_AF.hi, false); break;
	case 0xB8: CP_N_N(&reg_AF.hi, reg_BC.hi, false); break;
	case 0xB9: CP_N_N(&reg_AF.hi, reg_BC.lo, false); break;
	case 0xBA: CP_N_N(&reg_AF.hi, reg_DE.hi, false); break;
	case 0xBB: CP_N_N(&reg_AF.hi, reg_DE.lo, false); break;
	case 0xBC: CP_N_N(&reg_AF.hi, reg_HL.hi, false); break;
	case 0xBD: CP_N_N(&reg_AF.hi, reg_HL.lo, false); break;
	case 0xBE: CP_N_N(&reg_AF.hi, RAM->read_mem(reg_HL.val), false); break;
	case 0xFE: CP_N_N(&reg_AF.hi, 0x0, true); break;

	/** INC n **/
	case 0x3C: INC_N(&reg_AF.hi); break;
	case 0x04: INC_N(&reg_BC.hi); break;
	case 0x0C: INC_N(&reg_BC.lo); break;
	case 0x14: INC_N(&reg_DE.hi); break;
	case 0x1C: INC_N(&reg_DE.lo); break;
	case 0x24: INC_N(&reg_HL.hi); break;
	case 0x2C: INC_N(&reg_HL.lo); break;
	case 0x34: {
		uint8_t b = RAM->read_mem(reg_HL.val);
		INC_N(&b);
		RAM->write_mem(reg_HL.val, b);
		break;
	}
	
	/** DEC n **/
	case 0x3D: DEC_N(&reg_AF.hi); break;
	case 0x05: DEC_N(&reg_BC.hi); break;
	case 0x0D: DEC_N(&reg_BC.lo); break;
	case 0x15: DEC_N(&reg_DE.hi); break;
	case 0x1D: DEC_N(&reg_DE.lo); break;
	case 0x25: DEC_N(&reg_HL.hi); break;
	case 0x2D: DEC_N(&reg_HL.lo); break;
	case 0x35: {
		uint8_t b = RAM->read_mem(reg_HL.val);
		DEC_N(&b);
		RAM->write_mem(reg_HL.val, b);
		break;
	}

	/** ADD HL, n **/
	case 0x09: ADD_HL(&reg_BC); break;
	case 0x19: ADD_HL(&reg_DE); break;
	case 0x29: ADD_HL(&reg_HL); break;
	case 0x39: ADD_HL(&SP); break;

	/** ADD SP, n **/
	case 0xE8: ADD_SP(); break;

	/** INC nn **/
	case 0x03: INC_NN(&reg_BC); break;
	case 0x13: INC_NN(&reg_DE); break;
	case 0x23: INC_NN(&reg_HL); break;
	case 0x33: INC_NN(&SP); break;

	/** DEC nn **/
	case 0x0B: DEC_NN(&reg_BC); break;
	case 0x1B: DEC_NN(&reg_DE); break;
	case 0x2B: DEC_NN(&reg_HL); break;
	case 0x3B: DEC_NN(&SP); break;

	/** CB **/
	case 0xCB: return opcode_cycles[opcode] + execute_next_opcode();

	/** DAA **/
	case 0x27: DAA(); break;

	/** CPL **/
	case 0x2F: CPL(); break;

	/** CCF **/
	case 0x3F: CCF(); break;

	/** CCF **/
	case 0x37: SCF(); break;

	/** NOP **/
	case 0x00: break;

	/** STOP/HALT **/
	case 0x10: PC--; break;
	case 0x76: {
		halted = true; 
		PC--;
		break;
	}

	/** RLCA **/
	case 0x07: RLC(&reg_AF.hi, true); break;
	
	/** RLA **/
	case 0x17: RL(&reg_AF.hi, true); break;
	
	/** RRCA **/
	case 0x0F: RRC(&reg_AF.hi, true); break;

	/** RRA **/
	case 0x1F: RR(&reg_AF.hi, true); break;

	/** JP **/
	case 0xC3: PC = (RAM->read_mem(PC + 1) << 8) | RAM->read_mem(PC); break;

	/** JP cc **/
	case 0xC2: return JP_CC(true, false, false, false) ? opcode_cycles_conditional[opcode] : opcode_cycles[opcode];
	case 0xCA: return JP_CC(false, true, false, false) ? opcode_cycles_conditional[opcode] : opcode_cycles[opcode];
	case 0xD2: return JP_CC(false, false, true, false) ? opcode_cycles_conditional[opcode] : opcode_cycles[opcode];
	case 0xDA: return JP_CC(false, false, false, true) ? opcode_cycles_conditional[opcode] : opcode_cycles[opcode];

	/** JP HL **/
	case 0xE9: PC = reg_HL.val; break;

	/** JR n **/
	case 0x18: {
		uint8_t result = RAM->read_mem(PC);
		if (result < 0x80) {
			PC += result + 1;
		}
		else {
			PC -= (0xFF - result);
		}
		break;
	}
	/** JR cc **/
	case 0x20: return JR_CC(true, false, false, false) ? opcode_cycles_conditional[opcode]: opcode_cycles[opcode];
	case 0x28: return JR_CC(false, true, false, false) ? opcode_cycles_conditional[opcode] : opcode_cycles[opcode];
	case 0x30: return JR_CC(false, false, true, false) ? opcode_cycles_conditional[opcode] : opcode_cycles[opcode];
	case 0x38: return JR_CC(false, false, false, true) ? opcode_cycles_conditional[opcode] : opcode_cycles[opcode];

	/** CALL nn **/
	case 0xCD: CALL(); break;
	
	/** CALL cc **/
	case 0xC4: return CALL_CC(true, false, false, false) ? opcode_cycles_conditional[opcode] : opcode_cycles[opcode];
	case 0xCC: return CALL_CC(false, true, false, false) ? opcode_cycles_conditional[opcode] : opcode_cycles[opcode];
	case 0xD4: return CALL_CC(false, false, true, false) ? opcode_cycles_conditional[opcode] : opcode_cycles[opcode];
	case 0xDC: return CALL_CC(false, false, false, true) ? opcode_cycles_conditional[opcode] : opcode_cycles[opcode];

	/** RST n **/
	case 0xC7: RST(0x00); break;
	case 0xCF: RST(0x08); break;
	case 0xD7: RST(0x10); break;
	case 0xDF: RST(0x18); break;
	case 0xE7: RST(0x20); break;
	case 0xEF: RST(0x28); break;
	case 0xF7: RST(0x30); break;
	case 0xFF: RST(0x38); break;

	/** RET **/
	case 0xC9: RET(); break;
	
	/** RET cc **/
	case 0xC0: return RET_CC(true, false, false, false) ? opcode_cycles_conditional[opcode] : opcode_cycles[opcode];
	case 0xC8: return RET_CC(false, true, false, false) ? opcode_cycles_conditional[opcode] : opcode_cycles[opcode];
	case 0xD0: return RET_CC(false, false, true, false) ? opcode_cycles_conditional[opcode] : opcode_cycles[opcode];
	case 0xD8: return RET_CC(false, false, false, true) ? opcode_cycles_conditional[opcode] : opcode_cycles[opcode];

	/** RETI **/
	case 0xD9: {
		RET();
		pending_activation = true;
		break;
	}

	/** EI **/
	case 0xFB: pending_activation = true; break;

	/** DI **/
	case 0xF3: pending_deactivation = true; break;
	}
	
	reg_AF.lo &= 0xF0;
	return opcode_cycles[opcode];
}

unsigned int CPU::execute_next_opcode() {
	uint8_t opcode = 0x0;

	/* Fetch */
	opcode = RAM->read_mem(PC);
	PC++;

	/* Decode/Execute */
	switch (opcode) {
	case 0x37: SWAP(&reg_AF.hi); break;
	case 0x30: SWAP(&reg_BC.hi); break;
	case 0x31: SWAP(&reg_BC.lo); break;
	case 0x32: SWAP(&reg_DE.hi); break;
	case 0x33: SWAP(&reg_DE.lo); break;
	case 0x34: SWAP(&reg_HL.hi); break;
	case 0x35: SWAP(&reg_HL.lo); break;
	case 0x36: {
		uint8_t data = RAM->read_mem(reg_HL.val);
		SWAP(&data);
		RAM->write_mem(reg_HL.val, data);
		break;
	}

		/** RLC **/
	case 0x07: RLC(&reg_AF.hi, false); break;
	case 0x00: RLC(&reg_BC.hi, false); break;
	case 0x01: RLC(&reg_BC.lo, false); break;
	case 0x02: RLC(&reg_DE.hi, false); break;
	case 0x03: RLC(&reg_DE.lo, false); break;
	case 0x04: RLC(&reg_HL.hi, false); break;
	case 0x05: RLC(&reg_HL.lo, false); break;
	case 0x06: {
		uint8_t data = RAM->read_mem(reg_HL.val);
		RLC(&data, false);
		RAM->write_mem(reg_HL.val, data);
		break;
	}

		/** RL **/
	case 0x17: RL(&reg_AF.hi, false); break;
	case 0x10: RL(&reg_BC.hi, false); break;
	case 0x11: RL(&reg_BC.lo, false); break;
	case 0x12: RL(&reg_DE.hi, false); break;
	case 0x13: RL(&reg_DE.lo, false); break;
	case 0x14: RL(&reg_HL.hi, false); break;
	case 0x15: RL(&reg_HL.lo, false); break;
	case 0x16: {
		uint8_t data = RAM->read_mem(reg_HL.val);
		RL(&data, false);
		RAM->write_mem(reg_HL.val, data);
		break;
	}

		/** RRC **/
	case 0x0F: RRC(&reg_AF.hi, false); break;
	case 0x08: RRC(&reg_BC.hi, false); break;
	case 0x09: RRC(&reg_BC.lo, false); break;
	case 0x0A: RRC(&reg_DE.hi, false); break;
	case 0x0B: RRC(&reg_DE.lo, false); break;
	case 0x0C: RRC(&reg_HL.hi, false); break;
	case 0x0D: RRC(&reg_HL.lo, false); break;
	case 0x0E: {
		uint8_t data = RAM->read_mem(reg_HL.val);
		RRC(&data, false);
		RAM->write_mem(reg_HL.val, data);
		break;
	}
		/** RR **/
	case 0x1F: RR(&reg_AF.hi, false); break;
	case 0x18: RR(&reg_BC.hi, false); break;
	case 0x19: RR(&reg_BC.lo, false); break;
	case 0x1A: RR(&reg_DE.hi, false); break;
	case 0x1B: RR(&reg_DE.lo, false); break;
	case 0x1C: RR(&reg_HL.hi, false); break;
	case 0x1D: RR(&reg_HL.lo, false); break;
	case 0x1E: {
		uint8_t data = RAM->read_mem(reg_HL.val);
		RR(&data, false);
		RAM->write_mem(reg_HL.val, data);
		break;
	}

	/** SLA **/
	case 0x27: SLA(&reg_AF.hi); break;
	case 0x20: SLA(&reg_BC.hi); break;
	case 0x21: SLA(&reg_BC.lo); break;
	case 0x22: SLA(&reg_DE.hi); break;
	case 0x23: SLA(&reg_DE.lo); break;
	case 0x24: SLA(&reg_HL.hi); break;
	case 0x25: SLA(&reg_HL.lo); break;
	case 0x26: {
		uint8_t data = RAM->read_mem(reg_HL.val);
		SLA(&data);
		RAM->write_mem(reg_HL.val, data);
		break;
	}

	/** SRA **/
	case 0x2F: SRA(&reg_AF.hi); break;
	case 0x28: SRA(&reg_BC.hi); break;
	case 0x29: SRA(&reg_BC.lo); break;
	case 0x2A: SRA(&reg_DE.hi); break;
	case 0x2B: SRA(&reg_DE.lo); break;
	case 0x2C: SRA(&reg_HL.hi); break;
	case 0x2D: SRA(&reg_HL.lo); break;
	case 0x2E: {
		uint8_t data = RAM->read_mem(reg_HL.val);
		SRA(&data);
		RAM->write_mem(reg_HL.val, data);
		break;
	}

	/** SRL **/
	case 0x3F: SRL(&reg_AF.hi); break;
	case 0x38: SRL(&reg_BC.hi); break;
	case 0x39: SRL(&reg_BC.lo); break;
	case 0x3A: SRL(&reg_DE.hi); break;
	case 0x3B: SRL(&reg_DE.lo); break;
	case 0x3C: SRL(&reg_HL.hi); break;
	case 0x3D: SRL(&reg_HL.lo); break;
	case 0x3E: {
		uint8_t data = RAM->read_mem(reg_HL.val);
		SRL(&data);
		RAM->write_mem(reg_HL.val, data);
		break;
	}
	/** BIT **/
	case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47:
	case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4F:
	case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x57:
	case 0x58: case 0x59: case 0x5A: case 0x5B: case 0x5C: case 0x5D: case 0x5F:
	case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x67:
	case 0x68: case 0x69: case 0x6A: case 0x6B: case 0x6C: case 0x6D: case 0x6F:
	case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x77:
	case 0x78: case 0x79: case 0x7A: case 0x7B: case 0x7C: case 0x7D: case 0x7F: {
		BIT_HELPER(opcode);
		break;
	}
	case 0x46: case 0x4E: case 0x56: case 0x5E: case 0x66: case 0x6E: case 0x76: case 0x7E: {
		uint8_t hi_nibble = (opcode & 0xF0) >> 4;
		uint8_t lo_nibble = opcode & 0xF;
		uint8_t index_base = 2 * (hi_nibble - 0x4);
		uint8_t index_add = lo_nibble / (0x8);
		uint8_t data = RAM->read_mem(reg_HL.val);
		BIT_B_R(&data, index_base + index_add);
		break;
	}

	/** RES **/
	case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87:
	case 0x88: case 0x89: case 0x8A: case 0x8B: case 0x8C: case 0x8D: case 0x8F:
	case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x97:
	case 0x98: case 0x99: case 0x9A: case 0x9B: case 0x9C: case 0x9D: case 0x9F:
	case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5: case 0xA7:
	case 0xA8: case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAF:
	case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB7:
	case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBF: {
		RES_HELPER(opcode);
		break;
	}

	case 0x86: case 0x8E: case 0x96: case 0x9E: case 0xA6: case 0xAE: case 0xB6: case 0xBE: {
		uint8_t hi_nibble = (opcode & 0xF0) >> 4;
		uint8_t lo_nibble = opcode & 0xF;
		uint8_t index_base = 2 * (hi_nibble - 0x8);
		uint8_t index_add = lo_nibble / (0x8);
		uint8_t data = RAM->read_mem(reg_HL.val);
		RES_B_R(&data, index_base + index_add);
		RAM->write_mem(reg_HL.val, data);
		break;
	}

	/** SET **/
	case 0xC0: case 0xC1: case 0xC2: case 0xC3: case 0xC4: case 0xC5: case 0xC7:
	case 0xC8: case 0xC9: case 0xCA: case 0xCB: case 0xCC: case 0xCD: case 0xCF:
	case 0xD0: case 0xD1: case 0xD2: case 0xD3: case 0xD4: case 0xD5: case 0xD7:
	case 0xD8: case 0xD9: case 0xDA: case 0xDB: case 0xDC: case 0xDD: case 0xDF:
	case 0xE0: case 0xE1: case 0xE2: case 0xE3: case 0xE4: case 0xE5: case 0xE7:
	case 0xE8: case 0xE9: case 0xEA: case 0xEB: case 0xEC: case 0xED: case 0xEF:
	case 0xF0: case 0xF1: case 0xF2: case 0xF3: case 0xF4: case 0xF5: case 0xF7:
	case 0xF8: case 0xF9: case 0xFA: case 0xFB: case 0xFC: case 0xFD: case 0xFF: {
		SET_HELPER(opcode);
		break;
	}

	case 0xC6: case 0xCE: case 0xD6: case 0xDE: case 0xE6: case 0xEE: case 0xF6: case 0xFE: {
		uint8_t hi_nibble = (opcode & 0xF0) >> 4;
		uint8_t lo_nibble = opcode & 0xF;
		uint8_t index_base = 2 * (hi_nibble - 0xC);
		uint8_t index_add = lo_nibble / (0x8);
		uint8_t data = RAM->read_mem(reg_HL.val);
		SET_B_R(&data, index_base + index_add);
		RAM->write_mem(reg_HL.val, data);
		break;
	}
	default: break;
	}
	clear_unused_bits();
	return opcode_cycles_cb[opcode];
}
/** 8 bit load **/

void CPU::print_state(uint8_t opcode) {
	fprintf(trace, "\n===========================================\n");
	fprintf(trace, "OPCODE: %02X\n", opcode);
	fprintf(trace, "REG AF: %02X        FF40: %02X\n", reg_AF.val, RAM->read_mem(0xFF40));
	fprintf(trace, "REG BC: %02X        FF41: %02X\n", reg_BC.val, RAM->read_mem(0xFF41));
	fprintf(trace, "REG DE: %02X        FF42: %02X\n", reg_DE.val, RAM->read_mem(0xFF42));
	fprintf(trace, "REG HL: %02X        FF43: %02X\n", reg_HL.val, RAM->read_mem(0xFF43));
	fprintf(trace, "PC: %02X\n", PC);
	fprintf(trace, "SP: %02X        FF44: %02X\n", SP, RAM->read_mem(0xFF44));
	fprintf(trace, "FLAG Z: %d       FF45: %02X\n", BIT_CHECK(reg_AF.lo, FLAG_Z), RAM->read_mem(0xFF45));
	fprintf(trace, "FLAG N: %d       FF46: %02X\n", BIT_CHECK(reg_AF.lo, FLAG_N), RAM->read_mem(0xFF46));
	fprintf(trace, "FLAG H: %d       FF47: %02X\n", BIT_CHECK(reg_AF.lo, FLAG_H), RAM->read_mem(0xFF47));
	fprintf(trace, "FLAG C: %d       FF48: %02X\n", BIT_CHECK(reg_AF.lo, FLAG_C), RAM->read_mem(0xFF48));
	fprintf(trace, "FF4A: %02X        FF4B: %02X\n", RAM->read_mem(0xFF4A), RAM->read_mem(0xFF4B));
	fprintf(trace, "STACK TOP: %02X%02X\n", RAM->read_mem(SP.val + 1), RAM->read_mem(SP.val));
	fprintf(trace, "RAM[FF80]: %02X\n", RAM->read_mem(0xFF80));
	fprintf(trace, "RAM[FF40]: %02X, RAM[FF44]: %02X\n", RAM->read_mem(0xFF40), RAM->read_mem(0xFF44));
	fprintf(trace, "RAM[FF0F]: %02X, TIMA: %02X, TMA: %02X, TMC: %02X", RAM->read_mem(0xFF0F), RAM->read_mem(TIMA), RAM->read_mem(TMA), RAM->read_mem(TMC));
}

void CPU::clear_unused_bits() {
	reg_AF.lo &= 0xF0;
}
void CPU::LOAD_8BIT(uint8_t* reg) {
	*reg = RAM->read_mem(PC);
	PC += 1;
}

void CPU::LOAD_8BIT_IMMEDIATE(uint8_t* reg) {
	RAM->write_mem((RAM->read_mem(PC + 1) << 8 | RAM->read_mem(PC)), *reg);
	PC += 2;
}

void CPU::LOAD_8BIT_REG(uint8_t* r1, uint8_t* r2) {
	*r1 = *r2;
}

void CPU::LOAD_8BIT_FROM_MEM(uint8_t* reg, uint16_t addr, uint16_t immediate) {
	*reg = RAM->read_mem(addr + immediate);
}

void CPU::LOAD_8BIT_FROM_MEM_IMMEDIATE(uint8_t* reg) {
	*reg = RAM->read_mem((RAM->read_mem(PC + 1) << 8 | RAM->read_mem(PC)));
	PC += 2;
}

void CPU::LOAD_8BIT_INTO_MEM(uint16_t addr, uint8_t* reg, uint16_t immediate) {
	RAM->write_mem(addr + immediate, *reg);
}

void CPU::LDD_A_HL() {
	reg_AF.hi = RAM->read_mem(reg_HL.val);
	reg_HL.val--;
}

void CPU::LDD_HL_A() {
	RAM->write_mem(reg_HL.val, reg_AF.hi);
	reg_HL.val--;
}

void CPU::LDI_A_HL() {
	reg_AF.hi = RAM->read_mem(reg_HL.val);
	reg_HL.val++;
}

void CPU::LDI_HL_A() {
	RAM->write_mem(reg_HL.val, reg_AF.hi);
	reg_HL.val++;
}

void CPU::LDH_N_A() {
	RAM->write_mem(0xFF00 + RAM->read_mem(PC), reg_AF.hi);
	PC += 1;
}

void CPU::LDH_A_N() {
	reg_AF.hi = RAM->read_mem(0xFF00 + RAM->read_mem(PC));
	PC += 1;
}


/** 16 bit load **/

void CPU::LOAD_16BIT_IMMEDIATE(Register* reg) {
	reg->val = (RAM->read_mem(PC + 1) << 8) | RAM->read_mem(PC);
	PC += 2;
}


void CPU::LOAD_16BIT(Register* reg) {
	reg->val = ((RAM->read_mem(PC + 1) << 8) | RAM->read_mem(PC));
	PC += 2;
}


void CPU::LOAD_16BIT_TO_MEM(Register* reg_16, uint8_t* reg) {
	 RAM->write_mem(reg_16->val, *reg);
}

void CPU::LOAD_16BIT_REG(Register* r1, Register* r2) {
	r1->val = r2->val;
}

void CPU::LOAD_16BIT_REG_IMMEDIATE(Register* r1, Register* r2, uint16_t immediate) {
	r1->val = r2->val + immediate;
}

void CPU::LD_NN_SP(uint16_t immediate) {
	uint8_t hi_nibble = (SP.val & 0xFF00) >> 8;
	uint8_t lo_nibble = (SP.val & 0x00FF);
	RAM->write_mem((RAM->read_mem(PC + 1) << 8) | (RAM->read_mem(PC)), lo_nibble);
	RAM->write_mem(((RAM->read_mem(PC + 1) << 8) | (RAM->read_mem(PC))) + 1, hi_nibble);
	PC += 2;
}

void CPU::LDHL_SP() {
	int result = SP.val;
	int8_t data = RAM->read_mem(PC);
	
	reg_AF.lo = 0;
	
	result += data;
	int no_carry_result = SP.val ^ data;
	int carry_info = result ^ no_carry_result;

	if ((carry_info & 0x10) == 0x10)
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_H);

	if ((carry_info & 0x100) == 0x100)
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_C);

	reg_HL.val = (uint16_t)result;
	PC++;
}
void CPU::PUSH_NN(uint16_t addr) {
	uint8_t hi_nibble = (addr & 0xFF00) >> 8;
	uint8_t lo_nibble = (addr & 0x00FF);
	SP.val--;
	RAM->write_mem(SP.val, hi_nibble);
	SP.val--;
	RAM->write_mem(SP.val, lo_nibble);
}

void CPU::POP_NN(uint16_t* reg) {
	uint16_t addr = (RAM->read_mem(SP.val + 1) << 8) | (RAM->read_mem(SP.val));
	*reg = addr;
	SP.val += 2;
}

/** 8-bit arithmetic **/
void CPU::ADD_N_N(uint8_t* reg, uint8_t val, bool immediate, bool carry) {
	int result = (int)*reg;
	int adding = 0;
	if (immediate)
	{
		uint8_t n = RAM->read_mem(PC);
		PC++;
		adding = n;
	}
	else
	{
		adding = val;
	}

	if (carry)
	{
		if (BIT_CHECK(reg_AF.lo, FLAG_C))
			result++;
	}

	result += adding;
	reg_AF.lo = 0;

	int no_carry_sum = *reg ^ adding;
	int carry_info = result ^ no_carry_sum;

	if ((carry_info & 0x10) == 0x10)
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_H);

	if ((carry_info & 0x100) == 0x100)
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_C);

	*reg = (uint8_t)result;

	if (*reg == 0) {
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_Z);
	}
}

void CPU::SUB_N_N(uint8_t* reg, uint8_t val, bool immediate, bool borrow) {
	int result = (int)*reg;
	int subbing = 0;
	if (immediate)
	{
		uint8_t n = RAM->read_mem(PC);
		PC++;
		subbing = n;
	}
	else
	{
		subbing = val;
	}

	if (borrow)
	{
		if (BIT_CHECK(reg_AF.lo, FLAG_C))
			result--;
	}

	reg_AF.lo = 0;
	reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_N);

	result -= subbing;
	int no_borrow_dif = *reg ^ subbing;
	int carry_info = result ^ no_borrow_dif;

	if ((carry_info & 0x10) == 0x10)
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_H);

	if ((carry_info & 0x100) == 0x100)
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_C);

	*reg = (uint8_t)result;

	if (*reg == 0) {
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_Z);
	}
}

void CPU::AND_N_N(uint8_t* reg, uint8_t val, bool immediate) {
	uint8_t old = *reg;
	uint8_t anding = 0;

	if (immediate)
	{
		uint8_t n = RAM->read_mem(PC);
		PC++;
		anding = n;
	}
	else {
		anding = val;
	}

	*reg &= anding;

	reg_AF.lo = 0;

	if (*reg == 0) {
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_Z);
	}
	reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_H);

}

void CPU::OR_N_N(uint8_t* reg, uint8_t val, bool immediate) {
	uint8_t old = *reg;
	uint8_t oring = 0;

	if (immediate)
	{
		uint8_t n = RAM->read_mem(PC);
		PC++;
		oring = n;
	}
	else {
		oring = val;
	}

	*reg |= oring;

	reg_AF.lo = 0;

	if (*reg == 0) {
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_Z);
	}
}

void CPU::XOR_N_N(uint8_t* reg, uint8_t val, bool immediate) {
	uint8_t old = *reg;
	uint8_t xoring = 0;

	if (immediate)
	{
		uint8_t n = RAM->read_mem(PC);
		PC++;
		xoring = n;
	}
	else {
		xoring = val;
	}

	*reg ^= xoring;

	reg_AF.lo = 0;

	if (*reg == 0) {
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_Z);
	}
}

void CPU::CP_N_N(uint8_t* reg, uint8_t val, bool immediate) {
	uint8_t old = *reg;
	uint8_t result = 0;
	uint8_t sub = 0;

	if (immediate)
	{
		uint8_t n = RAM->read_mem(PC);
		PC++;
		sub = n;
	}
	else
	{
		sub = val;
	}

	result = old - sub;
	reg_AF.lo = 0;

	if (result == 0) {
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_Z);
	}

	reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_N);

	if (old < sub)
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_C);

	int16_t htest = (old & 0xF);
	htest -= (sub & 0xF);

	if (htest < 0)
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_H);
}

void CPU::INC_N(uint8_t* reg) {
	uint8_t old = *reg;

	*reg += 1;

	BIT_CLEAR(reg_AF.lo, FLAG_N);

	if (*reg == 0) {
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_Z);
	}
	else {
		reg_AF.lo = BIT_CLEAR(reg_AF.lo, FLAG_Z);
	}

	uint16_t htest = (old & 0xF);
	htest += 0x1;

	if (htest > 0xF)
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_H);
	else
		reg_AF.lo = BIT_CLEAR(reg_AF.lo, FLAG_H);
}

void CPU::DEC_N(uint8_t* reg) {
	uint8_t old = *reg;

	*reg -= 1;

	BIT_SET(reg_AF.lo, FLAG_N);

	if (*reg == 0) {
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_Z);
	}
	else {
		reg_AF.lo = BIT_CLEAR(reg_AF.lo, FLAG_Z);
	}

	uint16_t htest = (old & 0xF);
	htest -= 0x1;

	if (htest > 0xF)
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_H);
	else
		reg_AF.lo = BIT_CLEAR(reg_AF.lo, FLAG_H);
}

/** 16 bit arithmetic **/

void CPU::ADD_HL(Register* reg) {
	int c_test = reg_HL.val + reg->val;

	BIT_CLEAR(reg_AF.lo, FLAG_N);
	BIT_CLEAR(reg_AF.lo, FLAG_H);
	BIT_CLEAR(reg_AF.lo, FLAG_C);

	if ((reg_HL.val ^ reg->val ^ (c_test & 0xFFFF)) & 0x1000)
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_H);

	if (c_test >= 0x10000)
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_C);

	reg_HL.val = (uint16_t)c_test;
}

void CPU::ADD_SP() {
	int result = SP.val;
	int8_t data = RAM->read_mem(PC);

	reg_AF.lo = 0;

	result += data;

	int no_carry_result = SP.val ^ data;
	int carry_info = result ^ no_carry_result;

	if ((carry_info & 0x10) == 0x10)
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_H);

	if ((carry_info & 0x100) == 0x100)
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_C);

	SP.val = (uint16_t)result;
	PC++;
}

void CPU::INC_NN(Register* reg) {
	reg->val++;
}

void CPU::DEC_NN(Register* reg) {
	reg->val--;
}

void CPU::SWAP(uint8_t* reg) {
	uint8_t old = *reg;

	reg_AF.lo = 0;
	*reg = ((old & 0x0F) << 4 | (old & 0xF0) >> 4);

	if (*reg == 0) {
		reg_AF.lo = BIT_SET(reg_AF.lo, FLAG_Z);
	}
}

void CPU::DAA() {
	int a = reg_AF.hi;
	if (!BIT_CHECK(reg_AF.lo, FLAG_N)) {
		if (BIT_CHECK(reg_AF.lo, FLAG_C) || a > 0x99) {
			a += 0x60;
			BIT_SET(reg_AF.lo, FLAG_C);
		}
		if (BIT_CHECK(reg_AF.lo, FLAG_H) || (a & 0x0f) > 0x09) {
			a += 0x6;
		}
	}
	else {
		if (BIT_CHECK(reg_AF.lo, FLAG_C)) {
			a -= 0x60;
			BIT_SET(reg_AF.lo, FLAG_C);
		}
		if (BIT_CHECK(reg_AF.lo, FLAG_H)) {
			a -= 0x6;
		}
	}

	if (a == 0) {
		BIT_SET(reg_AF.lo, FLAG_Z);
	}
	else {
		BIT_CLEAR(reg_AF.lo, FLAG_Z);
	}

	BIT_CLEAR(reg_AF.lo, FLAG_H);

	reg_AF.hi = a;
}

void CPU::CPL() {
	reg_AF.hi = ~reg_AF.hi;
	BIT_SET(reg_AF.lo, FLAG_N);
	BIT_SET(reg_AF.lo, FLAG_H);
}

void CPU::CCF() {
	BIT_FLIP(reg_AF.lo, FLAG_C);
	BIT_CLEAR(reg_AF.lo, FLAG_H);
	BIT_CLEAR(reg_AF.lo, FLAG_N);
}

void CPU::SCF() {
	BIT_CLEAR(reg_AF.lo, FLAG_N);
	BIT_CLEAR(reg_AF.lo, FLAG_H);
	BIT_SET(reg_AF.lo, FLAG_C);
}

void CPU::RLC(uint8_t* reg, bool regA) {
	uint8_t result = *reg;

	reg_AF.lo = 0;
	if (BIT_CHECK(*reg, 7)) {
		BIT_SET(reg_AF.lo, FLAG_C);
		result = (result << 1) | 0x1;
	}
	else {
		result <<= 1;
	}

	*reg = result;

	if (!regA) {
		if (result == 0) {
			BIT_SET(reg_AF.lo, FLAG_Z);
		}
	}
}

void CPU::RL(uint8_t* reg, bool is_RLA) {
	uint8_t result = *reg;
	uint8_t old_flag = BIT_CHECK(reg_AF.lo, FLAG_C);

	reg_AF.lo = 0;
	if (BIT_CHECK(*reg, 7)) {
		BIT_SET(reg_AF.lo, FLAG_C);
	}
	
	result = (result << 1) | old_flag;
	*reg = result;

	if (!is_RLA) {
		if (result == 0) {
			BIT_SET(reg_AF.lo, FLAG_Z);
		}
	}
}

void CPU::RRC(uint8_t* reg, bool regA) {
	uint8_t result = *reg;

	reg_AF.lo = 0;
	if (BIT_CHECK(*reg, 0)) {
		BIT_SET(reg_AF.lo, FLAG_C);
		result = (result >> 1) | 0x80;
	}
	else {
		result >>= 1;
	}

	*reg = result;

	if (!regA) {
		if (result == 0) {
			BIT_SET(reg_AF.lo, FLAG_Z);
		}
	}
}

void CPU::RR(uint8_t* reg, bool is_RRA) {

	uint8_t result = *reg;
	uint8_t old_flag = BIT_CHECK(reg_AF.lo, FLAG_C);

	reg_AF.lo = 0;
	if (BIT_CHECK(*reg, 0)) {
		BIT_SET(reg_AF.lo, FLAG_C);
	}

	result = (result >> 1) | (old_flag << 7);
	*reg = result;

	if (!is_RRA) {
		if (result == 0) {
			BIT_SET(reg_AF.lo, FLAG_Z);
		}
	}

}

void CPU::SLA(uint8_t* reg) {
	uint8_t old = *reg;

	reg_AF.lo = 0;

	if (BIT_CHECK(*reg, 7))
		BIT_SET(reg_AF.lo, FLAG_C);
	else
		BIT_CLEAR(reg_AF.lo, FLAG_C);

	*reg = (old << 1);

	if (*reg == 0)
		BIT_SET(reg_AF.lo, FLAG_Z);
}

void CPU::SRA(uint8_t* reg) {
	uint8_t old = *reg;

	reg_AF.lo = 0;

	if (BIT_CHECK(*reg, 0))
		BIT_SET(reg_AF.lo, FLAG_C);
	else
		BIT_CLEAR(reg_AF.lo, FLAG_C);

	*reg = (old >> 1) | (old & 0x80);

	if (*reg == 0)
		BIT_SET(reg_AF.lo, FLAG_Z);
}

void CPU::SRL(uint8_t* reg) {
	uint8_t old = *reg;

	reg_AF.lo = 0;

	if (BIT_CHECK(*reg, 0))
		BIT_SET(reg_AF.lo, FLAG_C);
	else
		BIT_CLEAR(reg_AF.lo, FLAG_C);

	*reg = old >> 1;

	if (*reg == 0)
		BIT_SET(reg_AF.lo, FLAG_Z);

}

void CPU::BIT_HELPER(uint8_t opcode) {
	uint8_t hi_nibble = (opcode & 0xF0) >> 4;
	uint8_t lo_nibble = opcode & 0xF;
	uint8_t* reg = registers[lo_nibble % 8];
	uint8_t index_base = 2*(hi_nibble - 0x4);
	uint8_t index_add = lo_nibble / (0x8);
	BIT_B_R(reg, index_base + index_add);
}

void CPU::BIT_B_R(uint8_t* reg, unsigned int index) {
	if (!BIT_CHECK(*reg, index))
		BIT_SET(reg_AF.lo, FLAG_Z);
	else
		BIT_CLEAR(reg_AF.lo, FLAG_Z);

	BIT_CLEAR(reg_AF.lo, FLAG_N);
	BIT_SET(reg_AF.lo, FLAG_H);
}

void CPU::RES_HELPER(uint8_t opcode) {
	uint8_t hi_nibble = (opcode & 0xF0) >> 4;
	uint8_t lo_nibble = opcode & 0xF;
	uint8_t* reg = registers[lo_nibble % 8];
	uint8_t index_base = 2 * (hi_nibble - 0x8);
	uint8_t index_add = lo_nibble / (0x8);
	RES_B_R(reg, index_base + index_add);
}

void CPU::RES_B_R(uint8_t* reg, unsigned int index) {
	BIT_CLEAR(*reg, index);
}

void CPU::SET_HELPER(uint8_t opcode) {
	uint8_t hi_nibble = (opcode & 0xF0) >> 4;
	uint8_t lo_nibble = opcode & 0xF;
	uint8_t* reg = registers[lo_nibble % 0x8];
	uint8_t index_base = 2 * (hi_nibble - 0xC);
	uint8_t index_add = lo_nibble / (0x8);
	SET_B_R(reg, index_base + index_add);
}

void CPU::SET_B_R(uint8_t* reg, unsigned int index) {
	BIT_SET(*reg, index);
}

bool CPU::JP_CC(bool nz, bool z, bool nc, bool c) {
	if (nz) {
		if (!BIT_CHECK(reg_AF.lo, FLAG_Z)) {
			PC = ((RAM->read_mem(PC + 1) << 8) | RAM->read_mem(PC));
			return true;
		}
	} else if (z) {
		if (BIT_CHECK(reg_AF.lo, FLAG_Z)) {
			PC = ((RAM->read_mem(PC + 1) << 8) | RAM->read_mem(PC));
			return true;
		}
	} else if (nc) {
		if (!BIT_CHECK(reg_AF.lo, FLAG_C)) {
			PC = ((RAM->read_mem(PC + 1) << 8) | RAM->read_mem(PC));
			return true;
		}
	} else if (c) {
		if (BIT_CHECK(reg_AF.lo, FLAG_C)) {
			PC = ((RAM->read_mem(PC + 1) << 8) | RAM->read_mem(PC));
			return true;
		}
	}
	PC += 2;
	return false;
}

bool CPU::JR_CC(bool nz, bool z, bool nc, bool c) {
	uint8_t add = RAM->read_mem(PC);
	if (nz) {
		if (!BIT_CHECK(reg_AF.lo, FLAG_Z)) {
			if (add < 0x80)
				PC += add + 1;
			else
				PC -= (uint8_t)(0xFF - add);
			return true;
		}
	}
	else if (z) {
		if (BIT_CHECK(reg_AF.lo, FLAG_Z)) {
			if (add < 0x7F)
				PC += add + 1;
			else
				PC -= (uint8_t)(0xFF - add);

			return true;
		}
	}
	else if (nc) {
		if (!BIT_CHECK(reg_AF.lo, FLAG_C)) {
			if (add < 0x7F)
				PC += add + 1;
			else
				PC -= (uint8_t)(0xFF - add);

			return true;
		}
	}
	else if (c) {
		if (BIT_CHECK(reg_AF.lo, FLAG_C)) {
			if (add < 0x7F)
				PC += add + 1;
			else
				PC -= (uint8_t)(0xFF - add);

			return true;
		}
	}

	PC++;
	return false;
}

void CPU::CALL() {
	uint16_t immediate = ((RAM->read_mem(PC + 1) << 8) | RAM->read_mem(PC));
	PUSH_NN(PC + 2);
	PC = immediate;
}

bool CPU::CALL_CC(bool nz, bool z, bool nc, bool c) {
	if (nz) {
		if (!BIT_CHECK(reg_AF.lo, FLAG_Z)) {
			CALL();
			return true;
		}
	}
	else if (z) {
		if (BIT_CHECK(reg_AF.lo, FLAG_Z)) {
			CALL();
			return true;
		}
	}
	else if (nc) {
		if (!BIT_CHECK(reg_AF.lo, FLAG_C)) {
			CALL();
			return true;
		}
	}
	else if (c) {
		if (BIT_CHECK(reg_AF.lo, FLAG_C)) {
			CALL();
			return true;
		}
	}
	PC += 2;
	return false;
}

void CPU::RST(uint8_t immediate) {
	PUSH_NN(PC);
	PC = immediate;
}

bool CPU::RET_CC(bool nz, bool z, bool nc, bool c) {
	if (nz) {
		if (!BIT_CHECK(reg_AF.lo, FLAG_Z)) {
			RET();
			return true;
		}
	}
	else if (z) {
		if (BIT_CHECK(reg_AF.lo, FLAG_Z)) {
			RET();
			return true;
		}
	}
	else if (nc) {
		if (!BIT_CHECK(reg_AF.lo, FLAG_C)) {
			RET();
			return true;
		}
	}
	else if (c) {
		if (BIT_CHECK(reg_AF.lo, FLAG_C)) {
			RET();
			return true;
		}
	}
	return false;
}

void CPU::RET() {
	uint16_t new_addr;
	POP_NN(&new_addr);
	PC = new_addr;
}