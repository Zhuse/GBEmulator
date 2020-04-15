#include "constants.h"

#define FLAG_Z 7
#define FLAG_N 6
#define FLAG_H 5
#define FLAG_C 4

union Register
{
    WORD val;
    struct
    {
        BYTE lo;
        BYTE hi;
    };
};

class CPU {
private:
    Register reg_AF;
    Register reg_BC;
    Register reg_DE;
    Register reg_HL;
    WORD PC;
    Register SP;
public: 
    CPU();
    void init();
};
