#include "divider_counter.h"
#include "memory.h"

DividerCounter::DividerCounter(Memory* mem) : Timer{ mem, 0xFF, 0xFF, 0, DIVIDER_REG } {}

void DividerCounter::on_overflow() {
    timer_counter -= cycles_period;
    mem->inc_divider_register();
}