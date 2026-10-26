#ifndef RAINBOWS_MOUSE_H
#define RAINBOWS_MOUSE_H

#include <stdint.h>

#include "keyboard.h" // also pulls in screen.h

#define RB_MOUSE_X (0)
#define RB_MOUSE_Y (2)
#define RB_MOUSE_IS_DOWN (4)
#define RB_MOUSE_JUST_PRESSED (5)
#define RB_MOUSE_JUST_RELEASED (6)
#define RB_MOUSE_SCROLLX (7)
#define RB_MOUSE_SCROLLY (8)

#define RB_MOUSE_SIZE (9)

#define RB_MOUSE_BASE (RB_KEYBOARD_BASE - RB_MOUSE_SIZE)

// shared between is_down, just_pressed, and just_released
#define RB_MOUSE_LEFT (1 << 0)
#define RB_MOUSE_RIGHT (1 << 1)
#define RB_MOUSE_MIDDLE (1 << 2)

typedef struct {
  uint16_t x;
  uint16_t y;
  uint8_t is_down;
  uint8_t just_pressed;
  uint8_t just_released;
  uint8_t scrollx;
  uint8_t scrolly;

  float scroll_rem_x; // fractional scroll left over (useful for trackpads)
  float scroll_rem_y;
} RbMouse;

int rb_mouse_init(RbMouse* m);

// call once per frame, needs screen to map window coords to framebuffer coords
void rb_mouse_update(RbMouse* m, const RbScreen* s);

uint8_t rb_mouse_read(RbMouse* m, uint32_t addr);
void rb_mouse_write(RbMouse* m, uint32_t addr, uint8_t value);

#endif // RAINBOWS_MOUSE_H
