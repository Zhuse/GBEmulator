#ifndef TIMER_H_
#define TIMER_H_

#include "constants.h"

class Memory;
class Timer
{
protected:
    uint16_t timer_counter;
    uint16_t timer_init;
    uint16_t timer_limit;
    uint16_t timer_addr;
    uint16_t cycles_period;
    Memory* mem;
public:
    Timer(Memory* mem_ptr, uint16_t period, uint16_t limit, uint16_t init, uint16_t addr);
    uint16_t get_counter();
    void set_counter(uint16_t val);
    void set_period(uint16_t new_period);
    virtual void on_overflow();
    virtual void update_timer(int cycles);
};

#endif
