#include "constants.h"
#include "timer.h"

class DividerCounter : public Timer {
public:
	DividerCounter(Memory* mem);
	void on_overflow();
};