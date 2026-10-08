#ifndef RAINBOWS_DATETIME_H
#define RAINBOWS_DATETIME_H

#include <stdint.h>

#define RB_DATETIME_BASE (0) 

#define RB_DATETIME_YEAR (1)
#define RB_DATETIME_MONTH (3)
#define RB_DATETIME_DAY (4)
#define RB_DATETIME_HOUR (5)
#define RB_DATETIME_MIN (6)
#define RB_DATETIME_SEC (7)
#define RB_DATETIME_DOTW (8)
#define RB_DATETIME_DOTY (9)
#define RB_DATETIME_ISDST (10)

#define RB_DATETIME_SIZE (11)

// just copying uxn's datetime device
typedef struct {
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
  uint8_t dotw;
  uint8_t doty;
  uint8_t isdst;
} RbDatetime;

int rb_datetime_init(RbDatetime* dt);

uint8_t rb_datetime_read(RbDatetime* dt, uint32_t addr);
void rb_datetime_write(RbDatetime* dt, uint32_t addr, uint8_t value);

#endif // RAINBOWS_CONSOLE_H
