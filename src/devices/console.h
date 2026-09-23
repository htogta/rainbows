#ifndef RAINBOWS_CONSOLE_H
#define RAINBOWS_CONSOLE_H

#include <stdint.h>

#define RB_CONSOLE_BASE (0) // TODO we sure we want this to be 0?

#define RB_CONSOLE_STDIN (RB_CONSOLE_BASE)
#define RB_CONSOLE_STDOUT (RB_CONSOLE_BASE + 1)
#define RB_CONSOLE_ARGC (RB_CONSOLE_BASE + 2)
#define RB_CONSOLE_ARG_INDEX (RB_CONSOLE_BASE + 3)
#define RB_CONSOLE_ARG_CHAR_I (RB_CONSOLE_BASE + 4) 
#define RB_CONSOLE_ARG_CHAR (RB_CONSOLE_BASE + 5)

#define RB_CONSOLE_SIZE (6)

typedef struct {
  uint8_t argc;
  char** argv;
  uint8_t arg_index; // determines which argument in argv we're looking at
  uint8_t arg_char_i; // determines which character in argv[arg_index] we're looking at
  uint8_t arg_char; // the character at (argv[arg_index])[arg_char_i]
} RbConsole;

int rb_console_init(RbConsole* c, int argc, char** argv);

uint8_t rb_console_read(RbConsole* c, uint32_t addr);
void rb_console_write(RbConsole* c, uint32_t addr, uint8_t value);

#endif // RAINBOWS_CONSOLE_H
