#define OK_IMPLEMENTATION
#include "ok.h"

#include "rainbows.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
  BlockFile* bf;
  char default_path[4096];
  char* bf_path = NULL;
  size_t capacity = 65536;
  int create = 0;

  if (argc < 2) {
    fprintf(stderr, 
    "usage: rainbows program.rom [-d file.blocks | -n file.blocks] [-c capacity]\n");
    return 1;
  }

  for (int i = 2; i < argc; i++) {
    if (!strcmp(argv[i], "-d")) {
      bf_path = argv[++i];
      create = 0;
    } else if (!strcmp(argv[i], "-n")) {
      bf_path = argv[++i];
      create = 1;
    } else if (!strcmp(argv[i], "-c")) {
      capacity = strtoul(argv[++i], NULL, 10);
    } else {
      fprintf(stderr, "unknown option: %s\n", argv[i]);
      return 1;
    }
  }

  if (bf_path == NULL) {
    snprintf(default_path, sizeof(default_path), "%s/.blocks", getenv("$HOME"));
    bf_path = default_path;
    
    if (access(bf_path, F_OK) == 0) {
      bf = blockfile_open(bf_path);
      fprintf(stderr, "warning: %s already exists, ignoring -c\n", bf_path);
    } else {
      bf = blockfile_create(bf_path, capacity);
    }
  } else {
    if (create) {
      bf = blockfile_create(bf_path, capacity);
    } else {
      bf = blockfile_open(bf_path);
    }
  }
  
  if (!bf) {
    printf("Failed to load disk from .blocks file\n");
    return 1;
  }

  uint8_t* ram = calloc(OK_MEM_SIZE, 1);
  uint8_t* program = calloc(OK_MEM_SIZE, 1);
  if (!ram || !program) {
    printf("Failed to init rainbows\n");
    blockfile_close(bf);
    return 1;
  }
  
  if (!ok_load_file(program, 0, argv[1])) {
    printf("Failed to load file %s\n", argv[1]);
    goto had_error;
  }

  // initialize the console
  RbConsole console;
  if (!rb_console_init(&console, argc, argv)) {
    printf("Failed to init console device\n");
    goto had_error;
  }

  // initialize the math device
  RbArith arith;
  if (!rb_arith_init(&arith)) {
    printf("Failed to init arithmetic device\n");
    goto had_error;
  }

  // initialize the disk device
  RbDisk disk;
  if (!rb_disk_init(&disk, bf)) {
    printf("Failed to init disk device\n");
    goto had_error;
  }

  rb_init(&bus, ram, program, &console, &arith, &disk);

  OkState vm;
  ok_init(&vm);
  while (vm.status == OK_RUNNING) ok_tick(&vm);

  free(ram);
  free(program);
  blockfile_close(bf);
  return vm.status != OK_HALTED;

  had_error:
    free(ram);
    free(program);
    blockfile_close(bf);
    return 1;
}
