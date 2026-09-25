#include "arith.h"

int rb_arith_init(RbArith* a) {
  if (!a) return 0;

  a->op = 0;
  a->width = 0;
  a->err = 0;
  a->x = 0;
  a->y = 0;
  a->result = 0;

  return 1;
}

uint8_t rb_arith_read(RbArith* a, uint32_t addr) {
  if (addr == RB_ARITH_OP) return a->op;
  
  if (addr == RB_ARITH_WIDTH) return a->width;
  
  if (addr == RB_ARITH_ERR) return a->err;

  // multi-byte values are weird
  if (addr == RB_ARITH_X) return (a->x & 0xff000000) >> 24;
  if (addr == RB_ARITH_X + 1) return (a->x & 0x00ff0000) >> 16;
  if (addr == RB_ARITH_X + 2) return (a->x & 0x0000ff00) >> 8;
  if (addr == RB_ARITH_X + 3) return a->x & 0x000000ff;

  if (addr == RB_ARITH_Y) return (a->y & 0xff000000) >> 24;
  if (addr == RB_ARITH_Y + 1) return (a->y & 0x00ff0000) >> 16;
  if (addr == RB_ARITH_Y + 2) return (a->y & 0x0000ff00) >> 8;
  if (addr == RB_ARITH_Y + 3) return a->y & 0x000000ff;

  if (addr == RB_ARITH_RESULT) return (a->result & 0xff000000) >> 24;
  if (addr == RB_ARITH_RESULT + 1) return (a->result & 0x00ff0000) >> 16;
  if (addr == RB_ARITH_RESULT + 2) return (a->result & 0x0000ff00) >> 8;
  if (addr == RB_ARITH_RESULT + 3) return a->result & 0x000000ff;
  
  return 0;
}

void rb_arith_write(RbArith* a, uint32_t addr, uint8_t value) {
  // read-only
  if (addr == RB_ARITH_RESULT || addr == RB_ARITH_ERR) return;
    
  if (addr == RB_ARITH_WIDTH) {
    if (value > 3) return;
    a->width = value;
  }

  // the actual math ops
  if (addr == RB_ARITH_OP) {
    if (value > RB_ARITH_MOD) return;
    if (a->width > 3) return;

    a->op = value;
    int max_int_size = 1 << ((a->width + 1) * 8);
    switch (a->op) {
      case RB_ARITH_MUL:
        a->result = (a->x * a->y) % max_int_size;
        break;
      case RB_ARITH_DIV:
        if (a->y == 0) {
          a->err = 1; // divide by 0 err
        } else {
          a->result = (a->x / a->y) % max_int_size;
        }
        break;
      case RB_ARITH_MOD:
        if (a->y == 0) {
          a->err = 2; // mod by 0 err
        } else {
          a->result = (a->x % a->y) % max_int_size;
        }
        break;
      default:
        return;
    }
  }
  
  uint32_t value32 = (uint32_t) value;
  if (addr == RB_ARITH_X) a->x = (a->x & 0x00ffffff) | (value32 << 24);
  if (addr == RB_ARITH_X + 1) a->x = (a->x & 0xff00ffff) | (value32 << 16);
  if (addr == RB_ARITH_X + 2) a->x = (a->x & 0x0000ff00) | (value32 << 8);
  if (addr == RB_ARITH_X + 3) a->x = (a->x & 0x000000ff) | value32;

  if (addr == RB_ARITH_Y) a->y = (a->y & 0x00ffffff) | (value32 << 24);
  if (addr == RB_ARITH_Y + 1) a->y = (a->y & 0xff00ffff) | (value32 << 16);
  if (addr == RB_ARITH_Y + 2) a->y = (a->y & 0x0000ff00) | (value32 << 8);
  if (addr == RB_ARITH_Y + 3) a->y = (a->y & 0x000000ff) | value32;  
}
