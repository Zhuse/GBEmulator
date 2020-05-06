#include "constants.h"
#include "stdio.h"

#define FLAG_Z 7
#define FLAG_N 6
#define FLAG_H 5
#define FLAG_C 4
#define BIT_SET(a,b) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b))))

union Register
{
    WORD val;
    struct
    {
        BYTE lo;
        BYTE hi;
    };
};

class Memory;
class CPU {
private:
    const unsigned int opcode_cycles[256] = {
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

    const unsigned int opcode_cycles_cb[256] = {
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

    Register reg_AF;
    Register reg_BC;
    Register reg_DE;
    Register reg_HL;
    BYTE* registers[8] = {&(reg_BC.hi), &(reg_BC.lo), &(reg_DE.hi),  &(reg_DE.lo),  &(reg_HL.hi),  &(reg_HL.lo), 0,  &(reg_AF.hi)};
    WORD PC;
    Register SP;
    Memory* RAM;
    FILE* trace;
    bool master_interrupt;
    bool pending_activation;
    bool pending_deactivation;
    bool halted;
    void req_interrupt(int idx);
    void serve_interrupt(int idx);
    void check_ie_activation();
    void check_ie_deactivation();

    unsigned int execute_opcode();
    unsigned int execute_next_opcode();
    void print_state();
    void LOAD_8BIT(BYTE* reg);
    void LOAD_16BIT(Register* reg);
    void LOAD_8BIT_REG(BYTE* r1, BYTE* r2);
    void LOAD_8BIT_FROM_MEM(BYTE* reg, WORD addr, WORD immediate);
    void LOAD_8BIT_FROM_MEM_IMMEDIATE(BYTE* reg);
    void LOAD_8BIT_INTO_MEM(WORD addr, BYTE* reg, WORD immediate);
    void LOAD_16BIT_TO_MEM(Register* reg_16, BYTE* reg);
    void LOAD_8BIT_IMMEDIATE(BYTE* reg);
    void LOAD_16BIT_IMMEDIATE(Register* reg);
    void LDD_A_HL();
    void LDD_HL_A();
    void LDI_A_HL();
    void LDI_HL_A();
    void LDH_N_A();
    void LDH_A_N();

    void LOAD_16BIT_REG(Register* r1, Register* r2);
    void LOAD_16BIT_REG_IMMEDIATE(Register* r1, Register* r2, WORD immediate);
    void LD_NN_SP(WORD immediate);
    void PUSH_NN(WORD addr);
    void POP_NN(WORD* reg);

    void ADD_N_N(BYTE* reg, BYTE val, bool immediate, bool carry);
    void SUB_N_N(BYTE* reg, BYTE val, bool immediate, bool carry);
    void AND_N_N(BYTE* reg, BYTE val, bool immediate);
    void OR_N_N(BYTE* reg, BYTE val, bool immediate);
    void XOR_N_N(BYTE* reg, BYTE val, bool immediate);
    void CP_N_N(BYTE* reg, BYTE val, bool immediate);
    void INC_N(BYTE* reg);
    void DEC_N(BYTE* reg);

    void ADD_HL(Register* reg);
    void ADD_SP();
    void INC_NN(Register* reg);
    void DEC_NN(Register* reg);
    void SWAP(BYTE* reg);
    void DAA();
    void CPL();
    void CCF();
    void SCF();
    void RLC(BYTE* reg);
    void RL(BYTE* reg);
    void RRC(BYTE* reg);
    void RR(BYTE* reg);
    void SLA(BYTE* reg);
    void SRA(BYTE* reg);
    void SRL(BYTE* reg);
    void BIT_HELPER(BYTE opcode);
    void BIT_B_R(BYTE* reg, unsigned int index);
    void RES_HELPER(BYTE opcode);
    void RES_B_R(BYTE* reg, unsigned int index);
    void SET_HELPER(BYTE opcode);
    void SET_B_R(BYTE* reg, unsigned int index);
    void JP_CC(bool nz, bool z, bool nc, bool c);
    void JR_CC(bool nz, bool z, bool nc, bool c);
    void CALL();
    void CALL_CC(bool nz, bool z, bool nc, bool c);
    void RST(BYTE immediate);
    void RET();
    void RET_CC(bool nz, bool z, bool nc, bool c);
public: 
    CPU(Memory* RAM_ptr);
    void init();
    unsigned int exec();
    void serve_interrupts();
};
