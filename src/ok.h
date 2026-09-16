#ifndef OK_H
#define OK_H

#include <stdint.h>
#include <stddef.h>

// useful constants
#define OK_WORD_SIZE (3) // word size in bytes
#define OK_MEM_SIZE (1 << (8 * OK_WORD_SIZE)) // default maximum memory
#define OK_STACK_SIZE (768)

typedef enum {
  OK_RUNNING, // currently executing
  OK_HALTED, // halted normally
  OK_PANIC, // halted abnormally
} OkStatus;

typedef struct {
  uint8_t dst[OK_STACK_SIZE]; // circular data stack
  uint8_t rst[OK_STACK_SIZE]; // circular return stack
  uint32_t pc; // program counter
  uint16_t d; // data stack pointer
  uint16_t r; // return stack pointer
  OkStatus status; // current VM status
} OkState;

// memory reading prototypes; these are implemented by the person making the
// VM's emulator
extern uint8_t ok_mem_read(size_t address); // get RAM
extern void ok_mem_write(size_t address, uint8_t val); // set RAM
extern uint8_t ok_fetch(size_t address); // get instruction

// initialize the VM
void ok_init(OkState* s);

// cycle the VM clock
OkStatus ok_tick(OkState* s);

// some helper functions that the user may use for fetching big-endian
// values from byte buffers (RAM or program memory)
uint32_t ok_get_bytes(uint8_t* buffer, size_t index, uint8_t amt);
void ok_set_bytes(uint8_t* buffer, size_t index, uint8_t amt, uint32_t val);

// this helper function opens a file at a path and loads it into a byte buffer
// it loads the bytes from the file into the buffer, with the file's first byte
// at buffer[start]. Returns nonzero upon failure.
int ok_load_file(uint8_t* buffer, size_t start, const char* filepath);
// NOTE: the buffer argument here is expected to be filled with ZEROES, so when
// you create this buffer before passing it to this function, maybe use calloc
// rather than malloc

#ifdef OK_IMPLEMENTATION // VM implementation

#include <stdio.h> // for ok_load_file

// helper functions for reading/writing values in buffers

// get an amt-wide value at index
uint32_t ok_get_bytes(uint8_t* buffer, size_t index, uint8_t amt) {
  uint32_t out = 0;
  for (uint8_t i = 0; i < amt; i++) out = (out << 8) | buffer[index + i];
  return out;
}

// set buffer at index to val, where val is amt bytes wide
void ok_set_bytes(uint8_t* buffer, size_t index, uint8_t amt, uint32_t val) {
  for (int i = amt - 1; i >= 0; i--) {
    buffer[index + i] = (uint8_t) (val & 0xff);
    val >>= 8;
  }
}

// helper function for loading a file at a path
int ok_load_file(uint8_t* buffer, size_t start, const char* filepath) {
  FILE* f = fopen(filepath, "rb");
  if (!f) return 0;

  if (fseek(f, 0, SEEK_END) != 0) {
    fclose(f);
    return 0;
  }
  long filesize = ftell(f);
  if (filesize < 0) {
    fclose(f);
    return 0;
  }
  rewind(f);

  // check bounds, we know the buffer is at most OK_MEM_SIZE bytes
  if ((size_t)filesize + start > OK_MEM_SIZE) {
    fclose(f);
    return 0;
  }

  // read file into buffer at offset "start"
  size_t read = fread(buffer + start, 1, filesize, f);
  fclose(f);

  if (read != (size_t)filesize) return 0;

  return 1;
}

// circular stack functions
static inline void ok_push(uint8_t* s, uint16_t* p, uint8_t n, uint32_t v) {
  for (int i = n - 1; i >= 0; i--) {
    s[*p] = v >> (8 * i);
    if (++*p == OK_STACK_SIZE) *p = 0;
  }
}

static inline uint32_t ok_pop(uint8_t* s, uint16_t* p, uint8_t n) {
  uint32_t out = 0;
  for (int i = 0; i < n; i++) {
    if (!*p) *p = OK_STACK_SIZE;
    --*p;
    out |= (uint32_t)s[*p] << (8 * i);
    s[*p] = 0;
  }
  return out;
}

#define ok_dst_push(s,n,v) ok_push((s)->dst, &(s)->d, n, v)
#define ok_dst_pop(s,n) ok_pop((s)->dst, &(s)->d, n)
#define ok_rst_push(s,n,v) ok_push((s)->rst, &(s)->r, n, v)
#define ok_rst_pop(s,n) ok_pop((s)->rst, &(s)->r, n)

// VM initialization
void ok_init(OkState* s) {
  s->d = 0; s->r = 0; s->pc = 0;
  s->status = OK_RUNNING;
  for (int i = 0; i < OK_STACK_SIZE; i++) {
    s->dst[i] = 0;
    s->rst[i] = 0;
  }
}

static void execute(OkState* vm, uint8_t opcode, uint8_t arg, uint8_t skip) {
  uint32_t a, b, addr, n;
  uint8_t byte;
  uint8_t width = arg + 1;

  switch (opcode) {
    case 0: // add
      b = ok_dst_pop(vm, width);
      a = ok_dst_pop(vm, width);
      if (!skip || ok_dst_pop(vm, 1))
        ok_dst_push(vm, width, a + b);
      else {
        ok_dst_push(vm, width, a);
        ok_dst_push(vm, width, b);
      }
      break;
    case 1: // and
      b = ok_dst_pop(vm, width);
      a = ok_dst_pop(vm, width);
      if (!skip || ok_dst_pop(vm, 1))
        ok_dst_push(vm, width, a & b);
      else {
        ok_dst_push(vm, width, a);
        ok_dst_push(vm, width, b);
      }
      break;
    case 2: // xor
      b = ok_dst_pop(vm, width);
      a = ok_dst_pop(vm, width);
      if (!skip || ok_dst_pop(vm, 1))
        ok_dst_push(vm, width, a ^ b);
      else {
        ok_dst_push(vm, width, a);
        ok_dst_push(vm, width, b);
      }
      break;
    case 3: // shf
      byte = (uint8_t) ok_dst_pop(vm, 1);
      n = ok_dst_pop(vm, width);
      if (!skip || ok_dst_pop(vm, 1)) {
        n = (n >> (byte & 15)) << (byte >> 4);
        ok_dst_push(vm, width, n);
      } else {
        ok_dst_push(vm, width, n);
        ok_dst_push(vm, 1, byte);
      }
      break;
    case 4: // swp
      b = ok_dst_pop(vm, width);
      a = ok_dst_pop(vm, width);
      if (!skip || ok_dst_pop(vm, 1)) {
        ok_dst_push(vm, width, b);
        ok_dst_push(vm, width, a);
      } else {
        ok_dst_push(vm, width, a);
        ok_dst_push(vm, width, b);
      }
      break;
    case 5: // cmp
      b = ok_dst_pop(vm, width);
      a = ok_dst_pop(vm, width);
      if (!skip || ok_dst_pop(vm, 1)) {
        ok_dst_push(vm, 1, (a > b) - (a < b));
      } else {
        ok_dst_push(vm, width, a);
        ok_dst_push(vm, width, b);
      }
      break;
    case 6: // str
      addr = ok_dst_pop(vm, OK_WORD_SIZE);
      if (!skip || ok_dst_pop(vm, 1)) {
        for (int i = width; i--;) ok_mem_write(addr + i, ok_dst_pop(vm, 1));
      } else {
        ok_dst_push(vm, OK_WORD_SIZE, addr);
      }
      break;
    case 7: // lod
      addr = ok_dst_pop(vm, OK_WORD_SIZE);
      if (!skip || ok_dst_pop(vm, 1)) {
        for (int i = 0; i < width; i++) ok_dst_push(vm, 1, ok_mem_read(addr + i));
      } else {
        ok_dst_push(vm, OK_WORD_SIZE, addr);
      }
      break;
    case 8: // dup
      n = ok_dst_pop(vm, width);
      if (!skip || ok_dst_pop(vm, 1)) {
        ok_dst_push(vm, width, n);
        ok_dst_push(vm, width, n);
      } else {
        ok_dst_push(vm, width, n);
      }
      break;
    case 9: // drp (pop from stack)
      n = ok_dst_pop(vm, width);
      if (skip && !ok_dst_pop(vm, 1)) ok_dst_push(vm, width, n);
      break;
    case 10: // psh (push onto return stack)
      n = ok_dst_pop(vm, width);
      if (!skip || ok_dst_pop(vm, 1)) {
        ok_rst_push(vm, width, n);
      } else {
        ok_dst_push(vm, width, n);
      }
      break;
    case 11: // pop (off of return stack)
      n = ok_rst_pop(vm, width);
      if (!skip || ok_dst_pop(vm, 1)) {
        ok_dst_push(vm, width, n);
      } else {
        ok_rst_push(vm, width, n);
      }
      break;
    case 12: // jmp
      addr = ok_dst_pop(vm, width);
      if (!skip || ok_dst_pop(vm, 1)) {
        vm->pc = addr % OK_MEM_SIZE;
      } else {
        ok_dst_push(vm, width, addr);
      }
      break;
    case 13: // lit
      if (!skip || ok_dst_pop(vm, 1)) {
        for (int i = width; i--;) ok_dst_push(vm, 1, ok_fetch(vm->pc++));
      } else {
        for (int i = width; i--;) vm->pc++;
      }
      break;
    case 14: // fet
      addr = ok_dst_pop(vm, OK_WORD_SIZE);
      if (!skip || ok_dst_pop(vm, 1)) {
        for (int i = 0; i < width; i++) ok_dst_push(vm, 1, ok_fetch(addr + i));
      } else {
        ok_dst_push(vm, OK_WORD_SIZE, addr);
      }
      break;
    case 15: // nop
      // even though it's a no-op, skip flag still means pop
      if (skip) ok_dst_pop(vm, 1);
      break;
  }
}

// cycle the VM clock
OkStatus ok_tick(OkState* s) {
  uint8_t i = ok_fetch(s->pc++);
  if (!(i & 0x80)) return s->status = OK_HALTED;
  execute(s, i & 15, (i >> 4) & 3, i & 0x40);
  return s->status;
}

#endif // OK_IMPLEMENTATION

#endif // OK_H
