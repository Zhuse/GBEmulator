 #include "emu.h"
#include "memory.h"
#include "cpu.h"
#include "ppu.h"
#include "iostream"
#include "fstream"

Emulator::Emulator() {
	load_cartridge();
	mem = new Memory(cartridge_mem);
	cpu = new CPU(mem);
	ppu = new PPU(mem, cpu);
}
void Emulator::tick() {
	unsigned int tick_cycles = 0;
	while (tick_cycles < MAX_CYCLES) {
		// Execute opcode and add cycles of opcode to counter
		BYTE cycles = cpu->exec();
		tick_cycles += cycles;

		// Update timers
		update_timers(cycles);

		// Update graphics
		ppu->draw(cycles);

		// Perform interrupts
		cpu->serve_interrupts();

	}
	// Render frame
}

void Emulator::load_cartridge() {
	// Clear mem
	memset(cartridge_mem, 0, sizeof(cartridge_mem));

	FILE* f;
	f = fopen("mario.gb", "rb");
	if (f == NULL) {
		printf("Error opening ROM\n");
	}
	fread(cartridge_mem, 1, 0x200000, f);
	fclose(f);
}

void Emulator::update_timers(int cycles) {
	update_divider(cycles);
	set_timer_freq();
    if (clock_enabled() && timer_limit) {

		timer_counter += cycles;

        while (timer_counter >= timer_limit) {

			timer_counter -= timer_limit;
			set_timer_freq();

            // Timer overflow
            if (mem->read_mem(TIMA) == 0xFF) {
                mem->write_mem(TIMA, mem->read_mem(TMA));
				cpu->req_interrupt(2);
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
void Emulator::set_timer_freq() {
	BYTE controller_reg = mem->read_mem(TMC);
	BYTE timer_freq_mask = 0x3;
	timer_limit = mem->map_timer_counter(controller_reg & timer_freq_mask);
}

bool Emulator::clock_enabled() {
	return BIT_CHECK(mem->read_mem(TMC), 2);
}

void Emulator::register_keypress(BYTE key) {
	BYTE joypad_reg = mem->read_mem(JOYPAD);
	BYTE joypad_state = mem->get_joypad_state();
	bool select = BIT_CHECK(joypad_reg, 5);
	bool directional = BIT_CHECK(joypad_reg, 4);

	if (select && key > 0x3) {
		if (BIT_CHECK(joypad_state, key)) {
			cpu->req_interrupt(4);
		}
		mem->write_to_joypad(key, true);
	}
	else if (directional && key <= 0x3) {
		if (BIT_CHECK(joypad_state, key)) {
			cpu->req_interrupt(4);
		}
		mem->write_to_joypad(key, true);
	}
}

void Emulator::unregister_keypress(BYTE key) {
	mem->write_to_joypad(key, false);
}

std::array<std::array<BYTE, 3>, SCREEN_W>* Emulator::get_screen() {
	return ppu->screen.data();
}
