#include "console.h"

#include <stdio.h>
#include <string.h>

int rb_console_init(RbConsole* c, int argc, char** argv) {
  if (!c) return 0;
  if (argc > 255 || argc < 0) return 0;
  if (!argv) return 0;

  c->argc = (uint8_t) (argc & 0xff);
  c->argv = argv;
  c->arg_index = 0;
  c->arg_char_i = 0;

  if (c->argc > 0) { // TODO is this always true?
    char* argument = c->argv[c->arg_index];
    c->arg_char = (uint8_t) argument[c->arg_char_i];
  } else {
    c->arg_char = 0;
  }

  return 1;
}

uint8_t rb_console_read(RbConsole* c, uint32_t addr) {
  if (addr == RB_CONSOLE_STDIN) return (uint8_t) getchar();
  if (addr == RB_CONSOLE_ARGC) return c->argc;
  if (addr == RB_CONSOLE_ARG_INDEX) return c->arg_index;
  if (addr == RB_CONSOLE_ARG_CHAR_I) return c->arg_char_i;
  if (addr == RB_CONSOLE_ARG_CHAR) return c->arg_char;
  
  return 0;
}

void rb_console_write(RbConsole* c, uint32_t addr, uint8_t value) {
  // these are read-only
  if (addr == RB_CONSOLE_STDIN) return;
  if (addr == RB_CONSOLE_ARGC) return;
  if (addr == RB_CONSOLE_ARG_CHAR) return;

  if (addr == RB_CONSOLE_STDOUT) {
    putchar((char) value);
    return;
  }

  if (addr == RB_CONSOLE_ARG_INDEX) {
    if (value > c->argc) return;
    
    c->arg_index = value;
    char* argument = c->argv[c->arg_index];

    if (c->arg_char_i > strlen(argument) + 1) { // out of bounds
      c->arg_char = 0;
    } else {
      c->arg_char = argument[c->arg_char_i];
    }
    return;
  }

  if (addr == RB_CONSOLE_ARG_CHAR_I) {
    char* argument = c->argv[c->arg_index];
    if (c->arg_char_i > strlen(argument) + 1) return;

    c->arg_char_i = value;
    c->arg_char = argument[value];
    return;
  }
}
