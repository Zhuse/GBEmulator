#include "constants.h"

class Memory {
private:
	BYTE RAM[0x10000];

public:
	Memory();
	void init();
	void write_mem(WORD addr, BYTE data);
};