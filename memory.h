#include "constants.h"

class Memory {
private:
	BYTE RAM[0x10000];
	BYTE* cartridge;
public:
	Memory(BYTE* cartridge_ptr);
	void init();
	void write_mem(WORD addr, BYTE data);
	BYTE Memory::read_mem(WORD addr) const;
};