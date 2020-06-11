#ifndef TIMER_H_
#define TIMER_H_

#include "constants.h"

class Memory;
class Timer
{
protected:
    WORD timer_counter;
    WORD timer_init;
    WORD timer_limit;
    WORD timer_addr;
    WORD cycles_period;
    Memory* mem;
public:
    Timer(Memory* mem_ptr, WORD period, WORD limit, WORD init, WORD addr);
    WORD get_counter();
    void set_counter(WORD val);
    void set_period(WORD new_period);
    virtual void on_overflow();
    virtual void update_timer(int cycles);
};

#endif
