#ifndef MAIN_TIMER_H
#define MAIN_TIMER_H

#include "constants.h"
#include "timer.h"

class CPU;
class MainTimer : public Timer {
private:
	CPU* cpu;
	bool clock_enabled();
	void set_timer_freq();
	uint16_t map_timer_counter(uint8_t freq);

public:
	MainTimer(Memory* mem, CPU* cpu_ptr);
	void on_overflow();
	void update_timer(int cycles);
};

#endif