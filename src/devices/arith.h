#ifndef RAINBOWS_ARITH_H
#define RAINBOWS_ARITH_H

#include <stdint.h>

#define RB_ARITH_BASE (0) // TODO change

#define RB_ARITH_OP (0)
#define RB_ARITH_WIDTH (1)
#define RB_ARITH_ERR (2)
#define RB_ARITH_X (3)
#define RB_ARITH_Y (7)
#define RB_ARITH_RESULT (11)

#define RB_ARITH_SIZE (15)

// here's our different operations we support
#define RB_ARITH_NONE (0) // nop
#define RB_ARITH_MUL (1) // result = x * y
#define RB_ARITH_DIV (2) // result = x / y
#define RB_ARITH_MOD (3) // result = x % y

typedef struct {
  uint8_t op;
  uint8_t width;
  uint8_t err;
  uint32_t x;
  uint32_t y;
  uint32_t result;
} RbArith;

int rb_arith_init(RbArith* a);

uint8_t rb_arith_read(RbArith* a, uint32_t addr);
void rb_arith_write(RbArith* a, uint32_t addr, uint8_t value);

#endif // RAINBOWS_ARITH_H
