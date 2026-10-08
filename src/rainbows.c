#include "rainbows.h"

int rb_init(
  Rainbows* rb, 
  uint8_t* ram, 
  uint8_t* program, 
  RbConsole* console, 
  RbArith* arith,
  RbDisk* disk,
  RbDatetime* dt
) {
  if (!ram || !program || !console || !arith || !disk || !dt) return 0;

  rb->ram = ram;
  rb->program = program;
  rb->console = console;
  rb->arith = arith;
  rb->disk = disk;
  rb->datetime = dt;
  
  return 1;
}

uint8_t rb_read(Rainbows* rb, uint32_t address) {
  if (address >= RB_CONSOLE_BASE && address < RB_CONSOLE_BASE + RB_CONSOLE_SIZE) 
    return rb_console_read(rb->console, address - RB_CONSOLE_BASE);

  if (address >= RB_ARITH_BASE && address < RB_ARITH_BASE + RB_ARITH_SIZE) 
    return rb_arith_read(rb->arith, address - RB_ARITH_BASE);
  
  if (address >= RB_DISK_BASE && address < RB_DISK_BASE + RB_DISK_SIZE) 
      return rb_disk_read(rb->disk, address - RB_DISK_BASE);

  if (address >= RB_DATETIME_BASE && address < RB_DATETIME_BASE + RB_DATETIME_SIZE) 
      return rb_datetime_read(rb->datetime, address - RB_DATETIME_BASE);
  
  return rb->ram[address];
}

void rb_write(Rainbows* rb, uint32_t address, uint8_t value) {
  if (address >= RB_CONSOLE_BASE && address < RB_CONSOLE_BASE + RB_CONSOLE_SIZE) {
    rb_console_write(rb->console, address - RB_CONSOLE_BASE, value);
    return;
  }

  if (address >= RB_ARITH_BASE && address < RB_ARITH_BASE + RB_ARITH_SIZE) {
    rb_arith_write(rb->arith, address - RB_ARITH_BASE, value);
    return;
  }

  if (address >= RB_DISK_BASE && address < RB_DISK_BASE + RB_DISK_SIZE) {
    rb_disk_write(rb->disk, address - RB_DISK_BASE, value);
    return;
  }

  if (address >= RB_DATETIME_BASE && address < RB_DATETIME_BASE + RB_DATETIME_SIZE) {
    rb_datetime_write(rb->datetime, address - RB_DATETIME_BASE, value);
    return;
  }

  rb->ram[address] = value; 
}
