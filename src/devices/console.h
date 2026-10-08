#ifndef RAINBOWS_CONSOLE_H
#define RAINBOWS_CONSOLE_H

#include <stdint.h>

#define RB_CONSOLE_BASE (26) 

#define RB_CONSOLE_STDIN (0)
#define RB_CONSOLE_STDOUT (1)
#define RB_CONSOLE_ARGC (2)
#define RB_CONSOLE_ARG_INDEX (3)
#define RB_CONSOLE_ARG_CHAR_I (4) 
#define RB_CONSOLE_ARG_CHAR (5)

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
