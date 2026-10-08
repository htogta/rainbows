#ifndef RAINBOWS_DISK_H
#define RAINBOWS_DISK_H

#include <stdint.h>

#define RB_DISK_BASE (0) // TODO

#define RB_DISK_CAPACITY (RB_DISK_BASE)
#define RB_DISK_UPDATE (RB_DISK_BASE + 4)
#define RB_DISK_ERROR (RB_DISK_BASE + 5)
#define RB_DISK_CURRENT (RB_DISK_BASE + 6)
#define RB_DISK_DATA (RB_DISK_UPDATE + 10)

#define RB_DISK_SIZE ((RB_DISK_DATA - RB_DISK_BASE) + 1024)

#include "blocks.h"

// capacity is read-only, it's the amt of blocks you can use;
// read from update to load block[current] into data, write to update to store data to block[current]
// current points to the currently held block, writing to it loads that block's 
// data into data, setting error if something goes wrong (like if it's out of 
// bounds);

// TODO a lot of these fields are probably redundant 
// coz they're stored in BlockFile
typedef struct {
  BlockFile* bf;
  uint32_t capacity; // total # of blocks 
  uint8_t update;
  uint8_t error;
  uint32_t current; // currently indexed block
  uint8_t data[1024];
} RbDisk;

int rb_disk_init(RbDisk* d, BlockFile* bf);

uint8_t rb_disk_read(RbDisk* d, uint32_t addr);
void rb_disk_write(RbDisk* d, uint32_t addr, uint8_t value);

#endif // RAINBOWS_DISK_H
