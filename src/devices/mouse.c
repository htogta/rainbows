#include "mouse.h"

#include <string.h>
#include <raylib.h>

// converts window coords to framebuffer coords (clamped)
static uint16_t window_to_fb(float pos, float window_size, float fb_size) {
  if (window_size <= 0 || fb_size < 1) return 0; // minimized window
  float v = pos * fb_size / window_size;
  if (v < 0) v = 0;
  if (v > fb_size - 1) v = fb_size - 1;
  return (uint16_t) v;
}

// for building the button bitmask
static uint8_t button_bits(bool (*test)(int)) {
  uint8_t bits = 0;
  if (test(MOUSE_BUTTON_LEFT)) bits |= RB_MOUSE_LEFT;
  if (test(MOUSE_BUTTON_RIGHT)) bits |= RB_MOUSE_RIGHT;
  if (test(MOUSE_BUTTON_MIDDLE)) bits |= RB_MOUSE_MIDDLE;
  return bits;
}

// adds a scroll delta to a wrapping counter 
// so fractions get carried to later frames
static void accumulate_scroll(uint8_t* counter, float* rem, float delta) {
  *rem += delta;
  int whole = (int) *rem; // truncates towards 0
  *rem -= (float) whole;
  *counter = (uint8_t) (*counter + whole); // wraps mod 256
}

int rb_mouse_init(RbMouse* m) {
  if (!m) return 0;
  memset(m, 0, sizeof(*m));
  return 1;
}

void rb_mouse_update(RbMouse* m, const RbScreen* s) {
  Vector2 pos = GetMousePosition();
  m->x = window_to_fb(pos.x, (float) GetScreenWidth(),  (float) s->width);
  m->y = window_to_fb(pos.y, (float) GetScreenHeight(), (float) s->height);
  
  m->is_down = button_bits(IsMouseButtonDown);
  m->just_pressed = button_bits(IsMouseButtonPressed);
  m->just_released = button_bits(IsMouseButtonReleased);
  
  // scroll wheel (positive y is up/away from you, positive x is right)
  Vector2 wheel = GetMouseWheelMoveV();
  accumulate_scroll(&m->scrollx, &m->scroll_rem_x, wheel.x);
  accumulate_scroll(&m->scrolly, &m->scroll_rem_y, wheel.y);
}

uint8_t rb_mouse_read(RbMouse* m, uint32_t addr) {
  if (addr == RB_MOUSE_X) return (uint8_t) (m->x >> 8);
  if (addr == RB_MOUSE_X + 1) return (uint8_t) (m->x & 0xff);
  if (addr == RB_MOUSE_Y) return (uint8_t) (m->y >> 8);
  if (addr == RB_MOUSE_Y + 1) return (uint8_t) (m->y & 0xff);
  if (addr == RB_MOUSE_IS_DOWN) return m->is_down;
  if (addr == RB_MOUSE_JUST_PRESSED) return m->just_pressed;
  if (addr == RB_MOUSE_JUST_RELEASED) return m->just_released;
  if (addr == RB_MOUSE_SCROLLX) return m->scrollx;
  if (addr == RB_MOUSE_SCROLLY) return m->scrolly;
 
  return 0;
}

void rb_mouse_write(RbMouse* m, uint32_t addr, uint8_t value) {
  // every register is readonly
  (void) m;
  (void) addr;
  (void) value;
}

