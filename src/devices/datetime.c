#include "datetime.h"
#include <time.h>

static void rb_datetime_now(RbDatetime* dt) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    dt->year  = tm->tm_year + 1900;
    dt->month = tm->tm_mon + 1;
    dt->day   = tm->tm_mday;
    dt->hour  = tm->tm_hour;
    dt->min   = tm->tm_min;
    dt->sec   = tm->tm_sec;
    dt->dotw  = tm->tm_wday;
    dt->doty  = tm->tm_yday;
    dt->isdst = tm->tm_isdst;
}

int rb_datetime_init(RbDatetime* dt) {
  if (!dt) return 0;

  rb_datetime_now(dt);
  
  return 1;
}

uint8_t rb_datetime_read(RbDatetime* dt, uint32_t addr) {
  rb_datetime_now(dt);

  if (addr == RB_DATETIME_YEAR) return (dt->year & 0xff00) >> 8;
  if (addr == RB_DATETIME_YEAR + 1) return dt->year & 0x00ff;
  
  if (addr == RB_DATETIME_MONTH) return dt->month;
  if (addr == RB_DATETIME_DAY) return dt->day;
  if (addr == RB_DATETIME_HOUR) return dt->hour;
  if (addr == RB_DATETIME_MIN) return dt->min;
  if (addr == RB_DATETIME_SEC) return dt->sec;
  if (addr == RB_DATETIME_DOTW) return dt->dotw;  
  if (addr == RB_DATETIME_DOTY) return dt->doty;
  if (addr == RB_DATETIME_ISDST) return dt->isdst;

  return 0;
}

void rb_datetime_write(RbDatetime* dt, uint32_t addr, uint8_t value) {
  rb_datetime_now(dt);
  
  return; // all of these are read-only
}
