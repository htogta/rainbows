#include "keyboard.h"

#include <string.h>
#include <raylib.h>

// keys that arrive in the CHAR queue as events (with repeat)
// control keys use ascii code, the rest use RB_KEY_ code
// (which can't collide with typed chars)
static const struct { int raylib_key; uint8_t ch; } event_keys[] = {
  { KEY_ENTER, '\n' },
  { KEY_TAB, '\t' },
  { KEY_BACKSPACE, 8 },
  { KEY_DELETE, 127 },
  { KEY_UP, RB_KEY_UP },
  { KEY_DOWN, RB_KEY_DOWN },
  { KEY_LEFT, RB_KEY_LEFT },
  { KEY_RIGHT, RB_KEY_RIGHT },
  { KEY_HOME, RB_KEY_HOME },
  { KEY_END, RB_KEY_END },
  { KEY_INSERT, RB_KEY_INSERT },
  { KEY_PAGE_UP, RB_KEY_PGUP },
  { KEY_PAGE_DOWN, RB_KEY_PGDN },
  { KEY_ESCAPE, 27 }
};

// true if the key code is currently held
static bool rb_key_is_down(int code) {
  if (code >= 'a' && code <= 'z') code = code - 32; // lowercase to uppercase
  if (code >= 32 && code < 127) return IsKeyDown(code);

  switch (code) {
    case RB_KEY_ENTER: return IsKeyDown(KEY_ENTER);
    case RB_KEY_TAB: return IsKeyDown(KEY_TAB);
    case RB_KEY_BACKSPACE: return IsKeyDown(KEY_BACKSPACE);
    case RB_KEY_ESCAPE: return IsKeyDown(KEY_ESCAPE);
    case RB_KEY_UP: return IsKeyDown(KEY_UP);
    case RB_KEY_DOWN: return IsKeyDown(KEY_DOWN);
    case RB_KEY_LEFT: return IsKeyDown(KEY_LEFT);
    case RB_KEY_RIGHT: return IsKeyDown(KEY_RIGHT);
    case RB_KEY_HOME: return IsKeyDown(KEY_HOME);
    case RB_KEY_END: return IsKeyDown(KEY_END);
    case RB_KEY_INSERT: return IsKeyDown(KEY_INSERT);
    case RB_KEY_DELETE: return IsKeyDown(KEY_DELETE);
    case RB_KEY_PGUP: return IsKeyDown(KEY_PAGE_UP);
    case RB_KEY_PGDN: return IsKeyDown(KEY_PAGE_DOWN);
    case RB_KEY_CAPSLOCK: return IsKeyDown(KEY_CAPS_LOCK);
    case RB_KEY_SHIFT: 
      return IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    case RB_KEY_CTRL: 
      return IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
    case RB_KEY_ALT: 
      return IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT);
    default: return false;
  }
}

static void push(RbKeyboard* k, uint8_t ch) {
  uint8_t next = (k->tail + 1) & (RB_KEYBOARD_QUEUE_SIZE - 1);
  if (next == k->head) return; // full, drop the char
  k->queue[k->tail] = ch;
  k->tail = next;
}

int rb_keyboard_init(RbKeyboard* k) {
  if (!k) return 0;
  memset(k, 0, sizeof(*k));
  return 1;
}

void rb_keyboard_update(RbKeyboard* k) {
  // drain typed characters, non-latin-1 is dropped for now
  int cp;
  while ((cp = GetCharPressed()) != 0) if (cp < 256) push(k, (uint8_t) cp); 
  
  // non-character keys delivered as events with key repeat
  for (size_t i = 0; i < sizeof(event_keys) / sizeof(event_keys[0]); i++) {
    int rl = event_keys[i].raylib_key;
    if (IsKeyPressed(rl) || IsKeyPressedRepeat(rl)) push(k, event_keys[i].ch);
  }
 
  // snapshot held-key state, we figure out the edges from the prev frame
  for (int code = 0; code < 256; code++) {
    bool now = rb_key_is_down(code);
    bool was = (k->state[code] & RB_KEY_STATE_DOWN) != 0;
 
    uint8_t s = 0;
    if (now) s |= RB_KEY_STATE_DOWN;
    if (now && !was) s |= RB_KEY_STATE_PRESSED;
    if (!now && was) s |= RB_KEY_STATE_RELEASED;
    k->state[code] = s;
  }
}

uint8_t rb_keyboard_read(RbKeyboard* k, uint32_t addr) {
  if (addr == RB_KEYBOARD_CHAR) {
    if (k->head == k->tail) return 0;
    uint8_t ch = k->queue[k->head];
    k->head = (k->head + 1) & (RB_KEYBOARD_QUEUE_SIZE - 1);
    return ch;
  }
  if (addr == RB_KEYBOARD_KEY_SELECT) return k->key_select;
  if (addr == RB_KEYBOARD_KEY_STATE) return k->state[k->key_select];
 
  return 0;
}

void rb_keyboard_write(RbKeyboard* k, uint32_t addr, uint8_t value) {
  if (addr == RB_KEYBOARD_KEY_SELECT) {
    k->key_select = value;
    return;
  }
  // CHAR and KEY_STATE are readonly
}

