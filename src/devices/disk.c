#include "disk.h"

int rb_disk_init(RbDisk* d, BlockFile* bf) {
  if (!d || !bf) return 0;

  if (bf->count <= 0) return 0;

  d->bf = bf;
  d->capacity = bf->count;
  d->update = 0;
  d->error = 0;
  d->current = 0;

  for (int i = 0; i < 1024; i++) d->data[i] = 0;

  return 1;
}

uint8_t rb_disk_read(RbDisk* d, uint32_t addr) {
  if (addr == RB_DISK_CAPACITY) return (d->capacity & 0xff000000) >> 24;
  if (addr == RB_DISK_CAPACITY + 1) return (d->capacity & 0x00ff0000) >> 16;
  if (addr == RB_DISK_CAPACITY + 2) return (d->capacity & 0x0000ff00) >> 8;
  if (addr == RB_DISK_CAPACITY + 3) return (d->capacity & 0x000000ff);

  if (addr == RB_DISK_ERROR) return d->error;

  if (addr >= RB_DISK_DATA && addr < RB_DISK_DATA + 1024) {
    return d->data[addr - RB_DISK_DATA];
  }

  if (addr == RB_DISK_CURRENT) return (d->current & 0xff000000) >> 24;
  if (addr == RB_DISK_CURRENT + 1) return (d->current & 0x00ff0000) >> 16;
  if (addr == RB_DISK_CURRENT + 2) return (d->current & 0x0000ff00) >> 8;
  if (addr == RB_DISK_CURRENT + 3) return (d->current & 0x000000ff);

  // this is where it gets interesting
  if (addr == RB_DISK_UPDATE) {
    Block* bl = block_read(d->bf, d->current);

    if (bl) {
      for (int i = 0; i < 1024; i++) d->data[i] = bl->data[i];
      d->error = 0;
    } else {
      d->error = 1;
    }

    block_free(bl);
    return d->update;
  }

  return 0;
}

void rb_disk_write(RbDisk* d, uint32_t addr, uint8_t value) {
  // read-only:
  if (addr >= RB_DISK_CAPACITY && addr < RB_DISK_CAPACITY + 4) {
    return; // read-only
  }
  if (addr == RB_DISK_ERROR) return;

  if (addr >= RB_DISK_DATA && addr < RB_DISK_DATA + 1024) {
    d->data[addr - RB_DISK_DATA] = value;
  }

  uint32_t value32 = (uint32_t) value;
  if (addr == RB_DISK_CURRENT) {
    d->current = (d->current & 0x00ffffff) | (value32 << 24);
  } 
  if (addr == RB_DISK_CURRENT + 1) {
    d->current = (d->current & 0xff00ffff) | (value32 << 16);
  }
  if (addr == RB_DISK_CURRENT + 2) {
    d->current = (d->current & 0xffff00ff) | (value32 << 8);
  }
  if (addr == RB_DISK_CURRENT + 3) {
    d->current = (d->current & 0xffffff00) | value32;
  }
  
  // this is where it gets interesting
  if (addr == RB_DISK_UPDATE) {
    Block* new_block = block_new(d->data, 1024);
    if (new_block) {
      new_block->number = d->current;
      int ok = block_update(d->bf, new_block);
      if (!ok) d->error = 2; // failed to write
      block_free(new_block);
    } else {
      d->error = 3; // failed to allocate?
    }
    
    d->update = value;
  }
}
