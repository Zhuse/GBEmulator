#include "constants.h"
#include "timer.h"

class CPU;
class MainTimer : public Timer {
private:
	CPU* cpu;
	bool clock_enabled();
	void set_timer_freq();
	WORD map_timer_counter(BYTE freq);

public:
	MainTimer(Memory* mem, CPU* cpu_ptr);
	void on_overflow();
	void update_timer(int cycles);
};