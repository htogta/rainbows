#include "rainbows.h"

int rb_init(Rainbows* rb, uint8_t* ram, uint8_t* program, RbConsole* console) {
  if (!ram || !program || !console) return 0;

  rb->ram = ram;
  rb->program = program;
  rb->console = console;

  return 1;
}

uint8_t rb_read(Rainbows* rb, uint32_t address) {
  if (address >= RB_CONSOLE_BASE && address < RB_CONSOLE_BASE + RB_CONSOLE_SIZE) 
    return rb_console_read(rb->console, address - RB_CONSOLE_BASE);

  return rb->ram[address];
}

void rb_write(Rainbows* rb, uint32_t address, uint8_t value) {
  if (address >= RB_CONSOLE_BASE && address < RB_CONSOLE_BASE + RB_CONSOLE_SIZE) {
    rb_console_write(rb->console, address - RB_CONSOLE_BASE, value);
    return;
  }

  rb->ram[address] = value; 
}
