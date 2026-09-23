#define OK_IMPLEMENTATION
#include "ok.h"

#include "rainbows.h"

#include <stdio.h>
#include <stdlib.h>

static Rainbows bus;

uint8_t ok_mem_read(size_t address) {
  return rb_read(&bus, address);
}

void ok_mem_write(size_t address, uint8_t val) {
  return rb_write(&bus, address, val);
}

uint8_t ok_fetch(size_t address) {
  return bus.program[address];
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("usage: okmin file.rom\n");
    return 1;
  }

  uint8_t* ram = calloc(OK_MEM_SIZE, 1);
  uint8_t* program = calloc(OK_MEM_SIZE, 1);
  if (!ram || !program) {
    printf("Failed to init rainbows\n");
    free(ram);
    free(program);
    return 1;
  }
  
  if (!ok_load_file(program, 0, argv[1])) {
    printf("Failed to load file %s\n", argv[1]);
    free(ram);
    free(program);
    return 1; 
  }

  // initialize the console
  RbConsole console;
  if (!rb_console_init(&console, argc, argv)) {
    printf("Failed to init console device\n");
    free(ram);
    free(program);
    return 1;
  }

  rb_init(&bus, ram, program, &console);

  OkState vm;
  ok_init(&vm);
  while (vm.status == OK_RUNNING) ok_tick(&vm);

  free(ram);
  free(program);
  return vm.status != OK_HALTED;
}
