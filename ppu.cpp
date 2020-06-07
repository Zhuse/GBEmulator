#include "ppu.h"
#include "memory.h"
#include "cpu.h"

PPU::PPU(Memory* mem_ptr, CPU* cpu_ptr) {
	scanline_counter = 0;
	mem = mem_ptr;
	cpu = cpu_ptr;
}

void PPU::draw(int cycles) {
	bool lcd_enabled = BIT_CHECK(mem->read_mem(LCD_CONTROL_REG), 7);

	set_lcd_status();

	if (lcd_enabled) {
		scanline_counter += cycles;
	}
	else {
		return;
	}
	if (scanline_counter >= CYCLES_PER_SCANLINE) {
		BYTE curr_line = mem->read_mem(CURR_SCANLINE);
		scanline_counter = 0;
		if (curr_line >= SCREEN_H && curr_line < LAST_SCANLINE) {
			cpu->req_interrupt(0);
		}
		else if (curr_line >= LAST_SCANLINE) {
			mem->write_mem(CURR_SCANLINE, 0);
			return;
		}
		else {
			draw_scanline();
		}
		mem->inc_scanline_register();
	}
}

void PPU::draw_scanline() {
	BYTE lcd_status_reg = mem->read_mem(LCD_CONTROL_REG);
	bool background_status = BIT_CHECK(lcd_status_reg, 0);
	bool window_status = BIT_CHECK(lcd_status_reg, 5);
	bool sprite_status = BIT_CHECK(lcd_status_reg, 1);

	if (background_status) {
		draw_tiles(lcd_status_reg, false);
	}
	if (window_status) {
		draw_tiles(lcd_status_reg, true);
	}
	if (sprite_status) {
		draw_sprites();
	}
}

void PPU::draw_sprites() {
	BYTE curr_scanline = mem->read_mem(CURR_SCANLINE);
	BYTE lcd_status_reg = mem->read_mem(LCD_CONTROL_REG);
	BYTE spr_height = BIT_CHECK(lcd_status_reg, 2) ? 16 : 8;

	for (int spr = 0; spr < 40; spr++) {
		WORD spr_attr_addr = OAM_BASE + spr * 4;
		BYTE spr_y = mem->read_mem(spr_attr_addr) - 16;
		BYTE spr_x = mem->read_mem(spr_attr_addr + 1) - 8;
		BYTE spr_num = mem->read_mem(spr_attr_addr + 2);
		BYTE spr_attr = mem->read_mem(spr_attr_addr + 3);

		if (spr_height == 16 && spr_num % 2 == 1) {
			spr_num--;
		}

		if (curr_scanline >= spr_y && curr_scanline < spr_y + spr_height) {
			BYTE flip_x = BIT_CHECK(spr_attr, 5);
			BYTE flip_y = BIT_CHECK(spr_attr, 6);
			BYTE line_idx = flip_y ? spr_height - (curr_scanline - spr_y) : curr_scanline - spr_y;
			WORD spr_data_addr = TILE_DATA_1_BASE + (spr_num * SPRITE_SIZE_BYTES) + line_idx * 2;
			BYTE spr_data1 = mem->read_mem(spr_data_addr);
			BYTE spr_data2 = mem->read_mem(spr_data_addr + 1);

			for (int x = 7; x >= 0; x--) {
				SIGNED_WORD spr_pixel_idx = x;
				if (flip_x) {
					spr_pixel_idx -= 7;
					spr_pixel_idx *= -1;
				}
				SIGNED_WORD place_x = 0 - x;
				place_x += (spr_x + 7);
				WORD pixel_colourcode = (BIT_CHECK(spr_data2, spr_pixel_idx) << 1 | BIT_CHECK(spr_data1, spr_pixel_idx));
				if (pixel_colourcode != 0x0) {
					assign_colour(place_x, curr_scanline, pixel_colourcode);
				}
			}
		}
	}
}

void PPU::draw_tiles(BYTE lcd_status_reg, bool window) {
	bool map_select = window ? BIT_CHECK(lcd_status_reg, 6) : BIT_CHECK(lcd_status_reg, 3);
	bool unsigned_select;
	BYTE scroll_x = mem->read_mem(SCROLL_X);
	BYTE scroll_y = mem->read_mem(SCROLL_Y);
	BYTE window_x = mem->read_mem(WINDOW_X) - 7;
	BYTE window_y = mem->read_mem(WINDOW_Y);
	WORD tile_map_base = map_select ? TILE_MAP_2_BASE : TILE_MAP_1_BASE;
	BYTE current_tile = 0x0;
	BYTE curr_scanline = mem->read_mem(CURR_SCANLINE);
	BYTE tile_y = window ? (curr_scanline - window_y) : (curr_scanline + scroll_y);
	WORD tile_row = (tile_y / 8);
	WORD tile_data_base;

	if (window & (window_y >= curr_scanline)) {
		return;
	}

	if (BIT_CHECK(lcd_status_reg, 4)) {
		tile_data_base = TILE_DATA_1_BASE;
		unsigned_select = true;
	}
	else {
		tile_data_base = TILE_DATA_2_BASE;
		unsigned_select = false;
	}

	for (int pxl = window ? window_x : 0; pxl < SCREEN_W; pxl++) {
		BYTE tile_x;

		if (window) {
			tile_x = pxl - window_x;
		}
		else {
			tile_x = pxl + scroll_x;
		}

		WORD tile_col = tile_x / 8;
		WORD tile_addr = (tile_row * 32) + tile_col;

		SIGNED_BYTE tile_num = mem->read_mem(tile_map_base + tile_addr);

		WORD tile_loc = tile_data_base;

		if (unsigned_select) {
			tile_loc += (BYTE)tile_num * 16;
		}
		else {
			tile_loc += tile_num * 16;
		}

		BYTE tile_line = tile_y % 8;

		BYTE tile_data1 = mem->read_mem(tile_loc + tile_line * 2);
		BYTE tile_data2 = mem->read_mem(tile_loc + (tile_line * 2) + 1);

		BYTE pixel_idx = tile_x % 8;
		BYTE pixel_colourcode = (BIT_CHECK(tile_data2, 7 - pixel_idx) << 1 | BIT_CHECK(tile_data1, 7 - pixel_idx));
		assign_colour(pxl, curr_scanline, pixel_colourcode);
	}
}

void PPU::assign_colour(BYTE x, BYTE y, BYTE code) {
	BYTE r = 0;
	BYTE g = 0;
	BYTE b = 0;
	switch (code) {
	case 0x0: {
		r = 0xFF;
		g = 0xFF;
		b = 0xFF;
		break;
	}
	case 0x1: {
		r = 0xB0;
		g = 0xB0;
		b = 0xB0;
		break;
	}
	case 0x2: {
		r = 0x60;
		g = 0x60;
		b = 0x60;
		break;
	}
	case 0x3: {
		r = 0x0;
		g = 0x0;
		b = 0x0;
		break;
	}
	default: {
		r = 0;
		g = 0;
		b = 0;
	}
	}
	if (y >= 0 && y < SCREEN_H && x >= 0 && x < SCREEN_W) {
		screen[y][x][0] = r;
		screen[y][x][1] = g;
		screen[y][x][2] = b;
	}
	
}

void PPU::set_lcd_status() {
	bool lcd_enabled = BIT_CHECK(mem->read_mem(LCD_CONTROL_REG), 7);
	BYTE curr_scanline = mem->read_mem(CURR_SCANLINE);
	BYTE status = mem->read_mem(LCD_STATUS_REG);
	BYTE old_mode = status & 0x3;
	BYTE new_mode = old_mode;

	if (!lcd_enabled) {
		mem->write_mem(CURR_SCANLINE, 0);
		scanline_counter = 0;
		status = BIT_CLEAR(status, 1);
		status = BIT_CLEAR(status, 0);
	}
	else {
		if (curr_scanline >= SCREEN_W) {
			new_mode = V_BLANK_MODE;
			status = BIT_CLEAR(status, 1);
			status = BIT_SET(status, 0);
		}
		else {
			BYTE sprite_search_bounds = 20;
			BYTE data_transfer_bounds = sprite_search_bounds + 43;
			if (scanline_counter <= sprite_search_bounds) {
				new_mode = SPRITE_SEARCH_MODE;
				status = BIT_SET(status, 1);
				status = BIT_CLEAR(status, 0);
			}
			else if (scanline_counter <= data_transfer_bounds) {
				new_mode = DATA_TRANSFER_MODE;
				status = BIT_SET(status, 1);
				status = BIT_SET(status, 0);
			}
			else {
				new_mode = H_BLANK_MODE;
				status = BIT_CLEAR(status, 1);
				status = BIT_CLEAR(status, 0);
			}
		}
	}

	if (new_mode != old_mode) {
		cpu->req_interrupt(1);
	}

	mem->write_mem(LCD_STATUS_REG, status);
}