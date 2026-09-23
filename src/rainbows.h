#ifndef RAINBOWS_H
#define RAINBOWS_H

#include <stdint.h>

#include "devices/console.h"

typedef struct {
  uint8_t* ram;
  uint8_t* program;

  RbConsole* console;
} Rainbows;

// ram assumed to point to a buffer of at least OK_MEM_SIZE bytes
int rb_init(Rainbows* rb, uint8_t* ram, uint8_t* program, RbConsole* console);

// addresses are 24-bit ok addresses, represented here with uint32_t
uint8_t rb_read(Rainbows* rb, uint32_t address);
void rb_write(Rainbows* rb, uint32_t address, uint8_t value);

#endif // RAINBOWS_H
