#include "ppu.h"
#include "memory.h"
#include "cpu.h"

using namespace Addresses;
using namespace GraphicSpecs;

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
		uint8_t curr_line = mem->read_mem(CURR_SCANLINE);
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
	uint8_t lcd_status_reg = mem->read_mem(LCD_CONTROL_REG);
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
	uint8_t curr_scanline = mem->read_mem(CURR_SCANLINE);
	uint8_t lcd_status_reg = mem->read_mem(LCD_CONTROL_REG);
	uint8_t spr_height = BIT_CHECK(lcd_status_reg, 2) ? 16 : 8;

	/* Loop through sprite map and draw if scanline is located within each sprite */
	for (int spr = 0; spr < 40; spr++) {

		/* Determine sprite location and attributes */
		uint16_t spr_attr_addr = OAM_BASE + spr * 4;
		uint8_t spr_y = mem->read_mem(spr_attr_addr) - 16;
		uint8_t spr_x = mem->read_mem(spr_attr_addr + 1) - 8;
		uint8_t spr_num = mem->read_mem(spr_attr_addr + 2);
		uint8_t spr_attr = mem->read_mem(spr_attr_addr + 3);

		if (spr_height == 16 && spr_num % 2 == 1) {
			spr_num--;
		}

		if (curr_scanline >= spr_y && curr_scanline < spr_y + spr_height) {

			/* Parse sprite attributes */
			uint8_t flip_x = BIT_CHECK(spr_attr, 5);
			uint8_t flip_y = BIT_CHECK(spr_attr, 6);
			uint8_t line_idx = flip_y ? spr_height - (curr_scanline - spr_y) : curr_scanline - spr_y;
			uint16_t spr_data_addr = TILE_DATA_1_BASE + (spr_num * SPRITE_SIZE_IN_BYTES) + line_idx * 2;

			/* Retrieve sprite data */
			uint8_t spr_data1 = mem->read_mem(spr_data_addr);
			uint8_t spr_data2 = mem->read_mem(spr_data_addr + 1);

			/* Draw sprite pixels */
			for (int x = 7; x >= 0; x--) {
				int16_t spr_pixel_idx = x;
				if (flip_x) {
					spr_pixel_idx -= 7;
					spr_pixel_idx *= -1;
				}
				int16_t place_x = 0 - x;
				place_x += (spr_x + 7);
				uint16_t pixel_colourcode = (BIT_CHECK(spr_data2, spr_pixel_idx) << 1 | BIT_CHECK(spr_data1, spr_pixel_idx));
				if (pixel_colourcode != 0x0) {
					assign_colour(place_x, curr_scanline, pixel_colourcode);
				}
			}
		}
	}
}

void PPU::draw_tiles(uint8_t lcd_status_reg, bool window) {
	bool map_select = window ? BIT_CHECK(lcd_status_reg, 6) : BIT_CHECK(lcd_status_reg, 3);
	bool unsigned_select;

	uint8_t scroll_x = mem->read_mem(SCROLL_X);
	uint8_t scroll_y = mem->read_mem(SCROLL_Y);
	uint8_t window_x = mem->read_mem(WINDOW_X) - 7;
	uint8_t window_y = mem->read_mem(WINDOW_Y);
	uint8_t current_tile = 0x0;
	uint8_t curr_scanline = mem->read_mem(CURR_SCANLINE);
	uint8_t tile_y = window ? (curr_scanline - window_y) : (curr_scanline + scroll_y);

	uint16_t tile_map_base = map_select ? TILE_MAP_2_BASE : TILE_MAP_1_BASE;
	uint16_t tile_row = (tile_y / 8);
	uint16_t tile_data_base;

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

	/* Loop through the tile map and draw the associated tiles */
	for (int pxl = window ? window_x : 0; pxl < SCREEN_W; pxl++) {
		uint8_t tile_x;

		if (window) {
			tile_x = pxl - window_x;
		}
		else {
			tile_x = pxl + scroll_x;
		}

		uint16_t tile_col = tile_x / 8;
		uint16_t tile_addr = (tile_row * 32) + tile_col;

		int8_t tile_num = mem->read_mem(tile_map_base + tile_addr);

		uint16_t tile_loc = tile_data_base;

		if (unsigned_select) {
			tile_loc += (uint8_t)tile_num * 16;
		}
		else {
			tile_loc += tile_num * 16;
		}

		uint8_t tile_line = tile_y % 8;

		uint8_t tile_data1 = mem->read_mem(tile_loc + tile_line * 2);
		uint8_t tile_data2 = mem->read_mem(tile_loc + (tile_line * 2) + 1);

		uint8_t pixel_idx = tile_x % 8;
		uint8_t pixel_colourcode = (BIT_CHECK(tile_data2, 7 - pixel_idx) << 1 | BIT_CHECK(tile_data1, 7 - pixel_idx));
		assign_colour(pxl, curr_scanline, pixel_colourcode);
	}
}

void PPU::assign_colour(uint8_t x, uint8_t y, uint8_t code) {
	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
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
	uint8_t curr_scanline = mem->read_mem(CURR_SCANLINE);
	uint8_t status = mem->read_mem(LCD_STATUS_REG);
	uint8_t old_mode = status & 0x3;
	uint8_t new_mode = old_mode;

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

			/* Mode cycle bounds */
			uint8_t sprite_search_bounds = 20;
			uint8_t data_transfer_bounds = sprite_search_bounds + 43;

			/* Switch modes */
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
