#include "emu.h"
#include "memory.h"
#include "cpu.h"
#include "iostream"
#include "fstream"
Emulator::Emulator() {
	load_cartridge();
	mem = new Memory(cartridge_mem);
	cpu = new CPU(mem);
}
void Emulator::tick() {
	unsigned int cycles = 0;
	while (cycles < MAX_CYCLES) {
		// Execute opcode and add cycles of opcode to counter
		cycles += cpu->exec();

		// Update timers
		update_timers(cycles);

		// Update graphics

		// Perform interrupts
		cpu->serve_interrupts();

	}
	// Render frame
}

void Emulator::load_cartridge() {
	// Clear mem
	memset(cartridge_mem, 0, sizeof(cartridge_mem));

	FILE* f;
	f = fopen("test1.gb", "rb");
	if (f == NULL) {
		printf("Error opening ROM\n");
	}
	fread(cartridge_mem, 1, 0x200000, f);
	fclose(f);
}

void Emulator::update_timers(int cycles) {
	update_divider(cycles);
    if (clock_enabled()) {
        timer_counter -= cycles;

        if (timer_counter <= 0) {

			set_clk_freq();

            // Timer overflow
            if (mem->read_mem(TIMA) == 0xFF) {
                mem->write_mem(TIMA, mem->read_mem(TMA));
            } else {
                mem->write_mem(TIMA, mem->read_mem(TIMA) + 1);
            }
        }
    }
}

void Emulator::update_divider(int cycles) {
	divide_counter += cycles;
	if (divide_counter >= 0xFF) {
		divide_counter = 0;
		mem->inc_divider_register();
	}
}
void Emulator::set_clk_freq() {
	timer_counter = mem->map_timer_counter(mem->read_mem(TMC));
}

bool Emulator::clock_enabled() {
	return BIT_CHECK(mem->read_mem(TMC), 2);
}

void Emulator::draw(int cycles) {
	bool lcd_enabled = BIT_CHECK(mem->read_mem(LCD_CONTROL_REG), 7);

	if (lcd_enabled) {
		scanline_counter += cycles;
	}
	else {
		return;
	}
	if (scanline_counter >= CYCLES_PER_SCANLINE) {
		BYTE curr_line = mem->read_mem(CURR_SCANLINE);
		curr_line++;
		mem->write_mem(CURR_SCANLINE, curr_line);

		// V-blank interrupt
		if (curr_line == SCREEN_H) {
			cpu->req_interrupt(0);
		}
		else if (curr_line > LAST_SCANLINE) {
			mem->write_mem(CURR_SCANLINE, 0);
		}
		else {
			// draw line
		}
		scanline_counter = 0;
	}
}

void Emulator::draw_scanline() {
	BYTE lcd_status_reg = mem->read_mem(LCD_STATUS_REG);
	bool background_status = BIT_CHECK(lcd_status_reg, 0);
	bool window_status = BIT_CHECK(lcd_status_reg, 5);
	bool sprite_status = BIT_CHECK(lcd_status_reg, 1);
	// Draw background
	// Draw window
	// Draw sprites
	if (background_status) {
		draw_bg(lcd_status_reg);
	}
	if (window_status) {
		draw_window();
	}
	if (sprite_status) {
		draw_sprites();
	}
}

void Emulator::draw_sprites() {

}

void Emulator::draw_bg(BYTE lcd_status_reg) {
	BYTE scroll_x = mem->read_mem(SCROLL_X);
	BYTE scroll_y = mem->read_mem(SCROLL_Y);
	bool map_select = BIT_CHECK(lcd_status_reg, 3);
	bool unsigned_select;
	WORD tile_map_base = map_select ? TILE_MAP_2_BASE : TILE_MAP_1_BASE;
	BYTE current_tile = 0x0;
	BYTE curr_scanline = mem->read_mem(CURR_SCANLINE);
	BYTE tile_y = curr_scanline + scroll_y;
	WORD tile_row = (tile_y / 8);
	WORD tile_data_base;

	if (BIT_CHECK(lcd_status_reg, 4)) {
		tile_data_base = TILE_DATA_1_BASE;
		unsigned_select = true;
	}
	else {
		tile_data_base = TILE_DATA_2_BASE;
		unsigned_select = false;
	}

	for (int pxl = 0; pxl < SCREEN_W; pxl++) {
		BYTE tile_x = pxl + scroll_x;
		WORD tile_col = tile_x / 8;
		BYTE tile_addr = (tile_row * 32) + tile_col;

		WORD tile_num = mem->read_mem(tile_map_base + tile_addr);
		
		WORD tile_loc = tile_data_base;

		if (unsigned_select) {
			tile_loc += tile_num * 16;
		}
		else {
			if (tile_num >= 0x80){
				tile_loc -= (tile_num) * 16;
			}
			else {
				tile_loc += (tile_num * 16);
			}
		}

		BYTE tile_line = tile_y % 8;

		BYTE tile_data1 = mem->read_mem(tile_data_base + tile_line * 2);
		BYTE tile_data2 = mem->read_mem(tile_data_base + (tile_line * 2) + 1);

		BYTE pixel_idx = tile_x % 8;
		BYTE pixel_colourcode = (BIT_CHECK(tile_data2, pixel_idx) << 1 | BIT_CHECK(tile_data1, pixel_idx));
	}
}

void Emulator::assign_colour(BYTE x, BYTE y, BYTE code) {
	BYTE r = 0;
	BYTE g = 0;
	BYTE b = 0;
	switch (code) {
	case 0x00: {
		r = 0;
		g = 0;
		b = 0;
		break;
	}
	case 0x01: {
		r = 0x60;
		g = 0x60;
		b = 0x60;
		break;
	}
	case 0x10: {
		r = 0xB0;
		g = 0xB0;
		b = 0xB0;
		break;
	}
	case 0x11: {
		r = 0xFF;
		g = 0xFF;
		b = 0xFF;
		break;
	}
	default: {
		r = 0;
		g = 0;
		b = 0;
	}
	}
	screen[x][y][0] = r;
	screen[x][y][1] = g;
	screen[x][y][2] = b;
}
void Emulator::draw_window() {

}

void Emulator::set_lcd_status() {
	bool lcd_enabled = BIT_CHECK(mem->read_mem(LCD_CONTROL_REG), 7);
	BYTE curr_scanline = mem->read_mem(CURR_SCANLINE);
	BYTE status = mem->read_mem(LCD_STATUS_REG);
	BYTE old_mode = status & 0x3;
	BYTE new_mode = V_BLANK_MODE;

	if (!lcd_enabled) {
		mem->write_mem(CURR_SCANLINE, 0);
		scanline_counter = 0;
	}
	else {
		if (curr_scanline >= SCREEN_W) {
			new_mode = V_BLANK_MODE;
		}
		else {
			BYTE sprite_search_bounds = 80;
			BYTE data_transfer_bounds = sprite_search_bounds + 172;
			if (scanline_counter <= sprite_search_bounds) {
				new_mode = SPRITE_SEARCH_MODE;
			} else if (scanline_counter <= data_transfer_bounds) {
				new_mode = DATA_TRANSFER_MODE;
			} else {
				new_mode = H_BLANK_MODE;
			}
		}
	}

	if (new_mode != old_mode) {
		cpu->req_interrupt(1);
	}
}
