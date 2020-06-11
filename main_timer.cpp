#include "main_timer.h"
#include "memory.h"
#include "cpu.h"

MainTimer::MainTimer(Memory* mem, CPU* cpu_ptr) : Timer{ mem, 0xFF, 0xFF, 0, TIMA }, cpu{ cpu_ptr } {}

bool MainTimer::clock_enabled() {
	return BIT_CHECK(mem->read_mem(TMC), 2);
}

void MainTimer::update_timer(int cycles) {
    set_timer_freq();
    if (clock_enabled() && cycles_period) {

        timer_counter += cycles;

        while (timer_counter >= cycles_period) {
            on_overflow();
        }
    }
}

void MainTimer::set_timer_freq() {
    BYTE controller_reg = mem->read_mem(TMC);
    BYTE timer_freq_mask = 0x3;
    cycles_period = map_timer_counter(controller_reg & timer_freq_mask);
}

void MainTimer::on_overflow() {
    timer_counter -= cycles_period;

    // Timer overflow
    if (mem->read_mem(TIMA) == timer_limit) {
        mem->write_mem(TIMA, mem->read_mem(TMA));
        cpu->req_interrupt(2);
    }
    else {
        mem->write_mem(TIMA, mem->read_mem(TIMA) + 1);
    }
}

WORD MainTimer::map_timer_counter(BYTE freq) {
    WORD mapped_freq;
    switch (freq)
    {
    case 0x0: mapped_freq = 256; break;
    case 0x1: mapped_freq = 4; break;
    case 0x2: mapped_freq = 16; break;
    case 0x3: mapped_freq = 64; break;
    default: mapped_freq = 256; break;
    }

    return mapped_freq;
}