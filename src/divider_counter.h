#ifndef DIVIDER_COUNTER_H
#define DIVIDER_COUNTER_H

#include "constants.h"
#include "timer.h"

class DividerCounter : public Timer {
public:
	DividerCounter(Memory* mem);
	void on_overflow();
};

#endif