#ifndef RAINBOWS_SCREEN_H
#define RAINBOWS_SCREEN_H

#include <stdint.h>

// TODO maybe some defines for setting fullscreen/no-resize modes?

#define RB_SCREEN_BASE () // TODO will set this later

#define RB_SCREEN_FRAME_COUNTER (0)
#define RB_SCREEN_WIDTH (1)
#define RB_SCREEN_HEIGHT (3)
#define RB_SCREEN_PD_WIDTH (5)
#define RB_SCREEN_PD_HEIGHT (7)
#define RB_SCREEN_DEPTH (9) // this addr will just return 16 on reading 
#define RB_SCREEN_DRAW_OP (10)
#define RB_SCREEN_OP_AMT_X (11)
#define RB_SCREEN_OP_AMT_Y (13)
#define RB_SCREEN_X (15)
#define RB_SCREEN_Y (17)
#define RB_SCREEN_COLOR (19)
#define RB_SCREEN_SPRITE (21) 

#define RB_SCREEN_SIZE (276) // should be right?

// 640x480 screen, using 16-bit RGB565 for color
// (i.e. framebuffer is 640 * 480 * (16/8) bytes, or 640 * 480 16-bit values)
typedef struct {
  uint8_t frame_counter; 
  uint16_t width, height;
  uint16_t pending_width, pending_height;
  uint8_t draw_op;
  uint16_t op_amt_x, op_amt_y;
  uint16_t x, y;
  uint16_t color;
  uint16_t sprite[256]; // supports blitting a 16x16 sprite at once

  // these are host-side and don't live in VM memory:
  uint16_t* fb;
  int resized;
} RbScreen;

// when draw_op is written to:
// if a 1 is written, we fill a rectangle with width op_amt_x and height op_amt_y
// starting at coords x and y (and stopping at edges of screen if we go over, rather than wrapping around)
// if a 2 is written, we use the data in "sprite" to draw a sprite to the screen. The width and height of the sprite is determined by op_amt_x and op_amt_y
// if a 3 is written, we draw a sprite like we did with draw_op = 2, only this time the color stored in "color" is treated as transparent.
// if a 4 is written, we resize the screen, width = pending_width and 
// height = pending_height.
// we use 16-bit RGB565 for color.

int rb_screen_init(RbScreen* s);

uint8_t rb_screen_read(RbScreen* s, uint32_t addr);
void rb_screen_write(RbScreen* s, uint32_t addr, uint8_t value);

#endif // RAINBOWS_SCREEN_H
