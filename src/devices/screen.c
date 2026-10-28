#include "screen.h"
#include <stdlib.h>
#include <string.h>

// called when screen gets set to a size to reallocate the framebuffer
static int screen_alloc(RbScreen* s, uint16_t w, uint16_t h) {
  if (!w || !h || w > RB_SCREEN_MAX_DIM || h > RB_SCREEN_MAX_DIM) return 0;

  uint16_t* p = calloc((size_t)w * h, sizeof(uint16_t));
  if (!p) return 0;
  free(s->fb);

  s->fb = p;
  s->width = w;
  s->height = h;
  s->resized = 1;

  return 1;
}

int rb_screen_init(RbScreen* s) {
  if (!s) return 0;
  memset(s, 0, sizeof(*s));
  s->pending_width = RB_SCREEN_DEFAULT_W;
  s->pending_height = RB_SCREEN_DEFAULT_H;
  return screen_alloc(s, RB_SCREEN_DEFAULT_W, RB_SCREEN_DEFAULT_H);
}

void rb_screen_free(RbScreen* s) {
  free(s->fb);
  s->fb = NULL;
}

static void draw_fill(RbScreen* s) {
  // x and y are treated as signed shorts
  // 32-bit math so x + amt can't overflow
  int32_t x0 = (int16_t)s->x;
  int32_t y0 = (int16_t)s->y;
  int32_t x1 = x0 + s->op_amt_x;
  int32_t y1 = y0 + s->op_amt_y;
  
  // clip the rect to the screen
  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;
  if (x1 > s->width)  x1 = s->width;
  if (y1 > s->height) y1 = s->height;
  
  // nothing left (fully offscreen or empty rect)
  if (x0 >= x1 || y0 >= y1) return;
  
  for (int32_t row = y0; row < y1; row++) {
    uint16_t* line = s->fb + (size_t)row * s->width;
    for (int32_t col = x0; col < x1; col++) line[col] = s->color;
  }
}

static void draw_sprite(RbScreen* s, int transparent) {
  uint32_t w = s->op_amt_x > 16 ? 16 : s->op_amt_x;
  uint32_t h = s->op_amt_y > 16 ? 16 : s->op_amt_y;
  int32_t px0 = (int16_t)s->x;
  int32_t py0 = (int16_t)s->y;
  
  for (uint32_t sy = 0; sy < h; sy++) {
    int32_t dy = py0 + (int32_t)sy;
    if (dy < 0) continue; // skip row when above screen
    if (dy >= s->height) break; // stop when drawing below screen
    
    for (uint32_t sx = 0; sx < w; sx++) {
      int32_t dx = px0 + (int32_t)sx;
      if (dx < 0) continue; // skip at left of screen
      if (dx >= s->width) break; // done when we hit right of screen
      
      // stride = w
      uint16_t px = s->sprite[sy * w + sx];
      if (transparent && px == s->color) continue;
      
      s->fb[(size_t)dy * s->width + dx] = px;
    }
  }
}

static void set_be16(uint16_t* r, uint32_t off, uint8_t v) {
  *r = off == 0 ? (uint16_t)((*r & 0x00ff) | (v << 8))
                : (uint16_t)((*r & 0xff00) | v);
}

static uint8_t get_be16(uint16_t r, uint32_t off) {
  return off == 0 ? r >> 8 : r & 0xff;
}

void rb_screen_write(RbScreen* s, uint32_t addr, uint8_t v) {
  if (addr >= RB_SCREEN_SPRITE && addr < RB_SCREEN_SPRITE + 512) {
    uint32_t off = addr - RB_SCREEN_SPRITE;
    set_be16(&s->sprite[off >> 1], off & 1, v);
    return;
  }
  
  switch (addr) {
    case RB_SCREEN_WIDTH:
    case RB_SCREEN_WIDTH + 1:
      set_be16(&s->pending_width, addr - RB_SCREEN_WIDTH, v); 
      break;
    case RB_SCREEN_HEIGHT:
    case RB_SCREEN_HEIGHT + 1:
      set_be16(&s->pending_height, addr - RB_SCREEN_HEIGHT, v);
      break;
    case RB_SCREEN_OP_AMT_X:
    case RB_SCREEN_OP_AMT_X + 1:
      set_be16(&s->op_amt_x, addr - RB_SCREEN_OP_AMT_X, v);
      break;
    case RB_SCREEN_OP_AMT_Y:
    case RB_SCREEN_OP_AMT_Y + 1:
      set_be16(&s->op_amt_y, addr - RB_SCREEN_OP_AMT_Y, v);
      break;
    case RB_SCREEN_X:
    case RB_SCREEN_X + 1:
      set_be16(&s->x, addr - RB_SCREEN_X, v);
      break;
    case RB_SCREEN_Y:
    case RB_SCREEN_Y + 1:
      set_be16(&s->y, addr - RB_SCREEN_Y, v);
      break;
    case RB_SCREEN_COLOR:
    case RB_SCREEN_COLOR + 1:
      set_be16(&s->color, addr - RB_SCREEN_COLOR, v);
      break;
    case RB_SCREEN_DRAW_OP:
      s->draw_op = v;
      switch (v) {
        case 1: 
          draw_fill(s); 
          break;
        case 2: 
          draw_sprite(s, 0); 
          break;
        case 3: 
          draw_sprite(s, 1);
          break;
        case 4:
          screen_alloc(s, s->pending_width, s->pending_height);
          break;
        default:
          break;
      }
      break;
    // DEPTH and FRAME are read-only
  }
}

uint8_t rb_screen_read(RbScreen* s, uint32_t addr) {
  if (addr >= RB_SCREEN_SPRITE && addr < RB_SCREEN_SPRITE + 512) {
    uint32_t off = addr - RB_SCREEN_SPRITE;
    uint16_t w = s->sprite[off >> 1];
    return (off & 1) ? w & 0xff : w >> 8;
  }
  
  switch (addr) {
    case RB_SCREEN_WIDTH: return get_be16(s->width, 0);
    case RB_SCREEN_WIDTH + 1: return get_be16(s->width, 1);
    case RB_SCREEN_HEIGHT: return get_be16(s->height, 0);
    case RB_SCREEN_HEIGHT + 1: return get_be16(s->height, 1);
    case RB_SCREEN_DEPTH: return 16;
    case RB_SCREEN_DRAW_OP: return s->draw_op;
    case RB_SCREEN_FRAME_COUNTER: return s->frame_counter;
    case RB_SCREEN_OP_AMT_X: return get_be16(s->op_amt_x, 0);
    case RB_SCREEN_OP_AMT_X + 1: return get_be16(s->op_amt_x, 1);
    case RB_SCREEN_OP_AMT_Y: return get_be16(s->op_amt_y, 0);
    case RB_SCREEN_OP_AMT_Y + 1: return get_be16(s->op_amt_y, 1);
    case RB_SCREEN_X: return get_be16(s->x, 0);
    case RB_SCREEN_X + 1: return get_be16(s->x, 1);
    case RB_SCREEN_Y: return get_be16(s->y, 0);
    case RB_SCREEN_Y + 1: return get_be16(s->y, 1);
    case RB_SCREEN_COLOR: return get_be16(s->color, 0);
    case RB_SCREEN_COLOR + 1: return get_be16(s->color, 1);
    case RB_SCREEN_PIXEL:
    case RB_SCREEN_PIXEL + 1:
      uint16_t px = (s->x < s->width && s->y < s->height)
                        ? s->fb[(size_t)s->y * s->width + s->x] : 0;
      return (addr == RB_SCREEN_PIXEL) ? (px >> 8) : (px & 0xff);
  }
  
  return 0;
}
