#ifndef RAINBOWS_SPFLOAT_H
#define RAINBOWS_SPFLOAT_H

#include <stdint.h>

// a device for working with single-precision (32-bit) floating point numbers

#define RB_FLOAT_BASE (1066) // TODO I'd rather this be earlier but it works for now

#define RB_FLOAT_OP (0)
#define RB_FLOAT_ERR (1)
#define RB_FLOAT_X (2) // 3 4 5
#define RB_FLOAT_Y (6) // 7 8 9
#define RB_FLOAT_RESULT (10) // 11 12 13

#define RB_FLOAT_SIZE (14)

// here's the different operations we support
#define RB_FLOAT_OP_NONE (0) // nop
#define RB_FLOAT_OP_ADD (1)
#define RB_FLOAT_OP_SUB (2)
#define RB_FLOAT_OP_MUL (3) // result = x * y
#define RB_FLOAT_OP_DIV (4) // result = x / y
#define RB_FLOAT_OP_FLOOR (5) // result = floor(x)
#define RB_FLOAT_OP_CEIL (6) // result = ceil(x)
#define RB_FLOAT_OP_INT2FLOAT (7) // result = float(x)
#define RB_FLOAT_OP_FLOAT2INT (8) // result = int(x)

typedef struct {
  uint8_t op;
  uint8_t err;
  float x;
  float y;
  float result;
} RbFloat;

int rb_float_init(RbFloat* f);

uint8_t rb_float_read(RbFloat* f, uint32_t addr);
void rb_float_write(RbFloat* f, uint32_t addr, uint8_t value);

#endif // RAINBOWS_SPFLOAT_H
