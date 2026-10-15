#include "spfloat.h"
#include <math.h>
#include <string.h>

// reads a float and returns its bits as a uint32_t
static uint32_t float2bits(float f) {
  float other_f = f;
  uint32_t out = 0;
  memcpy(&out, &other_f, 4);
  return out;
}

// reads a uint32_t and returns its bits as a float
static float bits2float(uint32_t i) {
  uint32_t other_i = i;
  float out = 0.0f;
  memcpy(&out, &other_i, 4);
  return out;
}

// NOTE: the above two functions assume a float is 4 bytes, rb_float_init
// is designed to fail if this is not the case.

int rb_float_init(RbFloat* f) {
  if (!f) return 0;

  if (sizeof(float) != 4) return 0;

  f->op = 0;
  f->err = 0;
  f->x = bits2float(0);
  f->y = bits2float(0);
  f->result = bits2float(0);

  return 1;
}

uint8_t rb_float_read(RbFloat* f, uint32_t addr) {
  if (addr == RB_FLOAT_OP) return f->op;
  if (addr == RB_FLOAT_ERR) return f->err;

  uint32_t xbits = float2bits(f->x);
  uint32_t ybits = float2bits(f->y);
  uint32_t resultbits = float2bits(f->result);
  
  if (addr == RB_FLOAT_X) return (xbits & 0xff000000) >> 24;
  if (addr == RB_FLOAT_X + 1) return (xbits & 0x00ff0000) >> 16;
  if (addr == RB_FLOAT_X + 2) return (xbits & 0x0000ff00) >> 8;
  if (addr == RB_FLOAT_X + 3) return xbits & 0x000000ff;

  if (addr == RB_FLOAT_Y) return (ybits & 0xff000000) >> 24;
  if (addr == RB_FLOAT_Y + 1) return (ybits & 0x00ff0000) >> 16;
  if (addr == RB_FLOAT_Y + 2) return (ybits & 0x0000ff00) >> 8;
  if (addr == RB_FLOAT_Y + 3) return ybits & 0x000000ff;
  
  if (addr == RB_FLOAT_RESULT) return (resultbits & 0xff000000) >> 24;
  if (addr == RB_FLOAT_RESULT + 1) return (resultbits & 0x00ff0000) >> 16;
  if (addr == RB_FLOAT_RESULT + 2) return (resultbits & 0x0000ff00) >> 8;
  if (addr == RB_FLOAT_RESULT + 3) return resultbits & 0x000000ff;
  
  return 0;
}

void rb_float_write(RbFloat* f, uint32_t addr, uint8_t value) {
  // read-only
  if (addr == RB_FLOAT_RESULT || addr == RB_FLOAT_RESULT + 1 
        || addr == RB_FLOAT_RESULT + 2 || addr == RB_FLOAT_RESULT + 3 
        || addr == RB_FLOAT_ERR) return;

  uint32_t xbits = float2bits(f->x);
  uint32_t ybits = float2bits(f->y);

  if (addr == RB_FLOAT_OP) {
    if (value > RB_FLOAT_OP_FLOAT2INT) return;

    f->op = value;
    switch (f->op) {
      case RB_FLOAT_OP_ADD: // result = x + y
        f->result = (float)f->x + (float)f->y;
        return;
      case RB_FLOAT_OP_SUB: // result = x - y
        f->result = f->x - f->y;
        return;
      case RB_FLOAT_OP_MUL: // result = x * y
        f->result = f->x * f->y;
        return;
      case RB_FLOAT_OP_DIV: // result = x / y
        if (f->y == 0.0f) {
          f->err = 1; // divide by 0 error
        }
        f->result = f->x / f->y;
        return;
      case RB_FLOAT_OP_FLOOR: // result = floor(x)
        f->result = floorf(f->x);
        return;
      case RB_FLOAT_OP_CEIL: // result = ceil(x)
        f->result = ceilf(f->x);
        return;
      case RB_FLOAT_OP_INT2FLOAT: // result = float(x)
        f->result = (float)(xbits);
      case RB_FLOAT_OP_FLOAT2INT: // result = int(x)
        f->result = bits2float((int)(f->x));
      default:
        f->op = 0;
        f->err = 0;
        f->x = bits2float(0);
        f->y = bits2float(0);
        f->result = bits2float(0);
        return;
    }
    
  }
  
  uint32_t value32 = (uint32_t) value;
  if (addr == RB_FLOAT_X) f->x = bits2float((xbits & 0x00ffffff) | (value32 << 24));
  if (addr == RB_FLOAT_X + 1) f->x = bits2float((xbits & 0xff00ffff) | (value32 << 16));
  if (addr == RB_FLOAT_X + 2) f->x = bits2float((xbits & 0x0000ff00) | (value32 << 8));
  if (addr == RB_FLOAT_X + 3) f->x = bits2float((xbits & 0x000000ff) | value32);

  if (addr == RB_FLOAT_Y) f->y = bits2float((ybits & 0x00ffffff) | (value32 << 24));
  if (addr == RB_FLOAT_Y + 1) f->y = bits2float((ybits & 0xff00ffff) | (value32 << 16));
  if (addr == RB_FLOAT_Y + 2) f->y = bits2float((ybits & 0x0000ff00) | (value32 << 8));
  if (addr == RB_FLOAT_Y + 3) f->y = bits2float((ybits & 0x000000ff) | value32);  
}
