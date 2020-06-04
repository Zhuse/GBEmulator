#define JOYPAD 0xFF00
#define DIVIDER_REG 0xFF04
#define TIMA 0xFF05
#define TMA 0xFF06
#define TMC 0xFF07
#define IF 0xFF0F
#define IE 0xFFFF
#define CLOCK_SPEED 1048576
#define CURR_SCANLINE 0xFF44
#define LCD_CONTROL_REG 0xFF40
#define LCD_STATUS_REG 0xFF41
#define DMA_TRANSFER 0xFF46
#define OAM_BASE 0xFE00
#define SCROLL_Y 0xFF42
#define SCROLL_X 0xFF43
#define WINDOW_Y 0xFF4A
#define WINDOW_X 0xFF4B
#define TILE_MAP_1_BASE 0x9800
#define TILE_MAP_2_BASE 0x9C00
#define TILE_DATA_1_BASE 0x8000
#define TILE_DATA_2_BASE 0x9000


#define SCREEN_H 144
#define SCREEN_W 160
#define BACKGROUND_H 256
#define BACKGROUND_W 256
#define CYCLES_PER_SCANLINE 114
#define LAST_SCANLINE 153
#define H_BLANK_MODE 0x0
#define V_BLANK_MODE 0x1
#define SPRITE_SEARCH_MODE 0x2
#define DATA_TRANSFER_MODE 0x3

#define SPRITE_SIZE_BYTES 16
#define ROM_BANK_SIZE 16384
#define RAM_BANK_SIZE 8192
typedef unsigned char BYTE;
typedef char SIGNED_BYTE;
typedef unsigned short WORD;
typedef signed short SIGNED_WORD;

#define BIT_SET(a,b) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b))))
