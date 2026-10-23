#ifndef RAINBOWS_KEYBOARD_H
#define RAINBOWS_KEYBOARD_H

#include <stdint.h>

#include "screen.h"

#define RB_KEYBOARD_CHAR (0) // pops next typed char from queue
#define RB_KEYBOARD_KEY_SELECT (1) // selects a key code to query
#define RB_KEYBOARD_KEY_STATE (2) // see bitmasks

#define RB_KEYBOARD_SIZE (3)

#define RB_KEYBOARD_BASE (RB_SCREEN_BASE - RB_KEYBOARD_SIZE)

// keyboard state bitmasks:
#define RB_KEY_STATE_DOWN (1 << 0) // currently held (also set on press frame)
#define RB_KEY_STATE_PRESSED (1 << 1) // just went down this frame
#define RB_KEY_STATE_RELEASED (1 << 2) // just went up this frame

// keyboard keycodes
#define RB_KEY_ENTER (128)
#define RB_KEY_TAB (129)
#define RB_KEY_BACKSPACE (130)
#define RB_KEY_ESCAPE (131)
#define RB_KEY_UP (132)
#define RB_KEY_DOWN (133)
#define RB_KEY_LEFT (134)
#define RB_KEY_RIGHT (135)
#define RB_KEY_SHIFT (136) // left or right, don't care
#define RB_KEY_CTRL (137) // left or right, don't care
#define RB_KEY_ALT (138) // left or right, don't care
#define RB_KEY_HOME (139)
#define RB_KEY_END (140)
#define RB_KEY_INSERT (141)
#define RB_KEY_DELETE (142)
#define RB_KEY_PGUP (143)
#define RB_KEY_PGDN (144)
#define RB_KEY_CAPSLOCK (145) // why not?

// should be plenty big for the queue
#define RB_KEYBOARD_QUEUE_SIZE (64)

typedef struct {
  uint8_t queue[RB_KEYBOARD_QUEUE_SIZE]; 
  uint8_t head; // next slot to read
  uint8_t tail; // next slot to write
  uint8_t key_select;
  uint8_t state[256]; // bitmask of RB_KEY_STATE_*
} RbKeyboard;

int rb_keyboard_init(RbKeyboard* k);

// called once per frame
void rb_keyboard_update(RbKeyboard* k);

uint8_t rb_keyboard_read(RbKeyboard* k, uint32_t addr);
void rb_keyboard_write(RbKeyboard* k, uint32_t addr, uint8_t value);

#endif // RAINBOWS_KEYBOARD_H
