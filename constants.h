#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "cstdint"

#define BIT_SET(a,b) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b))))

namespace Addresses {
	const int JOYPAD = 0xFF00;
	const int DIVIDER_REG = 0xFF04;
	const int TIMA = 0xFF05;
	const int TMA = 0xFF06;
	const int TMC = 0xFF07;
	const int IF = 0xFF0F;
	const int IE = 0xFFFF;
	const int CURR_SCANLINE = 0xFF44;
	const int LCD_CONTROL_REG = 0xFF40;
	const int LCD_STATUS_REG = 0xFF41;
	const int DMA_TRANSFER = 0xFF46;
	const int OAM_BASE = 0xFE00;
	const int SCROLL_Y = 0xFF42;
	const int SCROLL_X = 0xFF43;
	const int WINDOW_Y = 0xFF4A;
	const int WINDOW_X = 0xFF4B;
	const int TILE_MAP_1_BASE = 0x9800;
	const int TILE_MAP_2_BASE = 0x9C00;
	const int TILE_DATA_1_BASE = 0x8000;
	const int TILE_DATA_2_BASE = 0x9000;
}

namespace Frequencies {
	const int CLOCK_SPEED = 1048576;
	const int CYCLES_PER_SCANLINE = 114;
	const int MAX_CYCLES_PER_TICK = 17476;
}

namespace GraphicSpecs {
	const int SCREEN_H = 144;
	const int SCREEN_W = 160;
	const int BACKGROUND_H = 256;
	const int BACKGROUND_W = 256;
	const int CYCLES_PER_SCANLINE = 114;
	const int LAST_SCANLINE = 153;
	const int SPRITE_SIZE_IN_BYTES = 16;
	const int H_BLANK_MODE = 0x0;
	const int V_BLANK_MODE = 0x1;
	const int SPRITE_SEARCH_MODE = 0x2;
	const int DATA_TRANSFER_MODE = 0x3;
}

namespace MemorySpecs {
	const int ROM_BANK_SIZE = 0x4000;
	const int RAM_BANK_SIZE = 0x2000;
}




#endif