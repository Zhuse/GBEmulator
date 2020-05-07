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
