#ifndef CPU_H
#define CPU_H

#include "constants.h"
#include "stdio.h"

enum FlagIndices {
    FLAG_Z = 7,
    FLAG_N = 6,
    FLAG_H = 5,
    FLAG_C = 4,
};

union Register
{
    uint16_t val;
    struct
    {
        uint8_t lo;
        uint8_t hi;
    };
};

class Memory;
class CPU {
private:
    const uint8_t opcode_cycles[256] = {
        1, 3, 2, 2, 1, 1, 2, 1, 5, 2, 2, 2, 1, 1, 2, 1,
        1, 3, 2, 2, 1, 1, 2, 1, 3, 2, 2, 2, 1, 1, 2, 1,
        2, 3, 2, 2, 1, 1, 2, 1, 2, 2, 2, 2, 1, 1, 2, 1,
        2, 3, 2, 2, 3, 3, 3, 1, 2, 2, 2, 2, 1, 1, 2, 1,
        1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
        1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
        1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
        2, 2, 2, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1,
        1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
        1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
        1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
        1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
        2, 3, 3, 4, 3, 4, 2, 4, 2, 4, 3, 0, 3, 6, 2, 4,
        2, 3, 3, 0, 3, 4, 2, 4, 2, 4, 3, 0, 3, 0, 2, 4,
        3, 3, 2, 0, 0, 4, 2, 4, 4, 1, 4, 0, 0, 0, 2, 4,
        3, 3, 2, 1, 0, 4, 2, 4, 3, 2, 4, 1, 0, 0, 2, 4
    };

    const uint8_t opcode_cycles_cb[256] = {
        2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
        2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
        2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
        2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
        2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,
        2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,
        2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,
        2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,
        2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
        2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
        2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
        2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
        2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
        2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
        2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
        2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2
    };

    const uint8_t opcode_cycles_conditional[256] = {
        1, 3, 2, 2, 1, 1, 2, 1, 5, 2, 2, 2, 1, 1, 2, 1,
        0, 3, 2, 2, 1, 1, 2, 1, 3, 2, 2, 2 ,1 ,1 ,2 ,1,
        3, 3, 2, 2, 1, 1, 2, 1, 3, 2, 2, 2, 1, 1, 2, 1,
        3, 3, 2, 2, 3, 3, 3, 1, 3, 2, 2, 2, 1, 1, 2, 1,
        1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
        1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
        1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
        2, 2, 2, 2, 2, 2, 0, 2, 1, 1, 1, 1, 1, 1, 2, 1,
        1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
        1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
        1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
        1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
        5, 3, 4, 4, 6, 4, 2, 4, 5, 4, 4, 0, 6, 6, 2, 4,
        5, 3, 4, 0, 6, 4, 2, 4, 5, 4, 4, 0, 6, 0, 2, 4,
        3, 3, 2, 0, 0, 4, 2, 4, 4, 1, 4, 0, 0, 0, 2, 4,
        3, 3, 2, 1, 0, 4, 2, 4, 3, 2, 4, 1, 0, 0, 2, 4
    };

    Register reg_AF;
    Register reg_BC;
    Register reg_DE;
    Register reg_HL;
    Register SP;
    uint8_t* registers[8] = { &(reg_BC.hi), &(reg_BC.lo), &(reg_DE.hi),  &(reg_DE.lo),  &(reg_HL.hi),  &(reg_HL.lo), 0,  &(reg_AF.hi) };
    uint16_t PC;
    Memory* RAM;
    FILE* trace;
    bool master_interrupt;
    bool pending_activation = false;
    bool pending_deactivation;
    bool halted = false;

    void serve_interrupt(int idx);
    void check_ie_activation();
    void check_ie_deactivation();

    unsigned int execute_opcode();
    unsigned int execute_next_opcode();
    void print_state(uint8_t opcode);
    void clear_unused_bits();
    void LOAD_8BIT(uint8_t* reg);
    void LOAD_16BIT(Register* reg);
    void LOAD_8BIT_REG(uint8_t* r1, uint8_t* r2);
    void LOAD_8BIT_FROM_MEM(uint8_t* reg, uint16_t addr, uint16_t immediate);
    void LOAD_8BIT_FROM_MEM_IMMEDIATE(uint8_t* reg);
    void LOAD_8BIT_INTO_MEM(uint16_t addr, uint8_t* reg, uint16_t immediate);
    void LOAD_16BIT_TO_MEM(Register* reg_16, uint8_t* reg);
    void LOAD_8BIT_IMMEDIATE(uint8_t* reg);
    void LOAD_16BIT_IMMEDIATE(Register* reg);
    void LDD_A_HL();
    void LDD_HL_A();
    void LDI_A_HL();
    void LDI_HL_A();
    void LDH_N_A();
    void LDH_A_N();
    void LDHL_SP();
    void LOAD_16BIT_REG(Register* r1, Register* r2);
    void LOAD_16BIT_REG_IMMEDIATE(Register* r1, Register* r2, uint16_t immediate);
    void LD_NN_SP(uint16_t immediate);
    void PUSH_NN(uint16_t addr);
    void POP_NN(uint16_t* reg);

    void ADD_N_N(uint8_t* reg, uint8_t val, bool immediate, bool carry);
    void SUB_N_N(uint8_t* reg, uint8_t val, bool immediate, bool carry);
    void AND_N_N(uint8_t* reg, uint8_t val, bool immediate);
    void OR_N_N(uint8_t* reg, uint8_t val, bool immediate);
    void XOR_N_N(uint8_t* reg, uint8_t val, bool immediate);
    void CP_N_N(uint8_t* reg, uint8_t val, bool immediate);
    void INC_N(uint8_t* reg);
    void DEC_N(uint8_t* reg);

    void ADD_HL(Register* reg);
    void ADD_SP();
    void INC_NN(Register* reg);
    void DEC_NN(Register* reg);
    void SWAP(uint8_t* reg);
    void DAA();
    void CPL();
    void CCF();
    void SCF();
    void RLC(uint8_t* reg, bool regA);
    void RL(uint8_t* reg, bool is_RLA);
    void RRC(uint8_t* reg, bool regA);
    void RR(uint8_t* reg, bool is_RRA);
    void SLA(uint8_t* reg);
    void SRA(uint8_t* reg);
    void SRL(uint8_t* reg);
    void BIT_HELPER(uint8_t opcode);
    void BIT_B_R(uint8_t* reg, unsigned int index);
    void RES_HELPER(uint8_t opcode);
    void RES_B_R(uint8_t* reg, unsigned int index);
    void SET_HELPER(uint8_t opcode);
    void SET_B_R(uint8_t* reg, unsigned int index);
    void CALL();
    void RST(uint8_t immediate);
    void RET();

    bool CALL_CC(bool nz, bool z, bool nc, bool c);
    bool JP_CC(bool nz, bool z, bool nc, bool c);
    bool JR_CC(bool nz, bool z, bool nc, bool c);
    bool RET_CC(bool nz, bool z, bool nc, bool c);
public: 
    CPU(Memory* RAM_ptr);
    void init();
    unsigned int exec();
    void serve_interrupts();
    void req_interrupt(int idx);
};

#endif