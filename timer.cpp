#include "constants.h"
#include "timer.h"
#include "memory.h"

using namespace Addresses;

Timer::Timer(Memory* mem_ptr, uint16_t period, uint16_t limit, uint16_t init, uint16_t addr) {
	mem = mem_ptr;
	cycles_period = period;
	timer_limit = limit;
	timer_init = init;
	timer_counter = init;
	timer_addr = addr;
}

uint16_t Timer::get_counter() {
	return timer_counter;
}

void Timer::on_overflow() {
	uint8_t timer_val = mem->read_mem(timer_addr);
	timer_counter -= cycles_period;
	if (mem->read_mem(timer_addr) >= timer_limit) {
		mem->write_mem(timer_addr, timer_init);
	}
	else {
		mem->write_mem(timer_addr, timer_val + 1);
	}
}

void Timer::update_timer(int cycles) {
	timer_counter += cycles;
	while (timer_counter >= cycles_period) {
		on_overflow();
	}
}

void Timer::set_period(uint16_t new_period) {
	cycles_period = new_period;
}

// Avoid using this function if possible
void Timer::set_counter(uint16_t val) {
	timer_counter = val;
}