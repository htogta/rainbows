#ifndef RAINBOWS_SCREEN_H
#define RAINBOWS_SCREEN_H

#include <stdint.h>

// TODO maybe some defines for setting fullscreen/no-resize modes?

#include "disk.h"

#define RB_SCREEN_MAX_DIM (2048) // width cannot be > 2048, same with height
#define RB_SCREEN_DEFAULT_W (640)
#define RB_SCREEN_DEFAULT_H (480)


#define RB_SCREEN_WIDTH (0)
#define RB_SCREEN_HEIGHT (2)
#define RB_SCREEN_DRAW_OP (4)
#define RB_SCREEN_OP_AMT_X (5)
#define RB_SCREEN_OP_AMT_Y (7)
#define RB_SCREEN_X (9)
#define RB_SCREEN_Y (11)
#define RB_SCREEN_COLOR (13)
#define RB_SCREEN_PIXEL (15)
#define RB_SCREEN_FRAME_COUNTER (17)
#define RB_SCREEN_DEPTH (18) // this addr will just return 16 on reading 
#define RB_SCREEN_SPRITE (19) 

#define RB_SCREEN_SIZE (RB_SCREEN_SPRITE + 512) // should be right?
#define RB_SCREEN_BASE (RB_DISK_BASE - RB_SCREEN_SIZE) 

// 640x480 screen, using 16-bit RGB565 for color
// (i.e. framebuffer is 640 * 480 * (16/8) bytes, or 640 * 480 16-bit values)
typedef struct {
  uint16_t width, height;
  uint8_t draw_op;
  uint16_t op_amt_x, op_amt_y;
  uint16_t x, y;
  uint16_t color;
  uint16_t pixel; // returns the color of the pixel at x, y
  // these are read-only
  uint8_t frame_counter;
  uint16_t sprite[256]; // supports blitting a 16x16 sprite at once


  // trying to set width and height sets these instead, real width and height 
  // update on next frame: 
  uint16_t pending_width, pending_height;

  // these are host-side and don't live in VM memory:
  uint16_t* fb;
  int resized;
} RbScreen;

// when draw_op is written to:

// if a 1 is written, we fill a rectangle with width op_amt_x and height 
// op_amt_y starting at coords x and y (and stopping at edges of screen if we go 
// over, rather than wrapping around)

// if a 2 is written, we use the data in "sprite" to draw a sprite to the 
// screen. The width and height of the sprite is determined by op_amt_x and 
// op_amt_y

// if a 3 is written, we draw a sprite like we did with draw_op = 2, only this 
// time the color stored in "color" is treated as transparent.

// if a 4 is written, we resize the screen, width = pending_width and 
// height = pending_height.

// we use 16-bit RGB565 for color.

int rb_screen_init(RbScreen* s);
void rb_screen_free(RbScreen* s);

uint8_t rb_screen_read(RbScreen* s, uint32_t addr);
void rb_screen_write(RbScreen* s, uint32_t addr, uint8_t value);

#endif // RAINBOWS_SCREEN_H
