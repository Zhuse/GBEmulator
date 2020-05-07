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

#define SCREEN_H 144
#define SCREEN_W 160
#define CYCLES_PER_SCANLINE 456
#define LAST_SCANLINE 153
#define H_BLANK_MODE 0x0
#define V_BLANK_MODE 0x1
#define SPRITE_SEARCH_MODE 0x10
#define DATA_TRANSFER_MODE 0x11


typedef unsigned char BYTE;
typedef char SIGNED_BYTE;
typedef unsigned short WORD;
typedef signed short SIGNED_WORD;