// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/keyboard.h"
#include "../sdl3_include.h"

func b32 keyboard_is_available(void) {
  return SDL_HasKeyboard() ? 1 : 0;
}

func b32 keyboard_get_primary_device_id(device_id* out_id) {
  return devices_get_device(DEVICE_TYPE_KEYBOARD, 0, out_id);
}

func b32 keyboard_is_key_down(u32 scancode) {
  int key_count = 0;
  const bool* state = SDL_GetKeyboardState(&key_count);

  if (!state || scancode >= (u32)key_count) {
    return 0;
  }

  return state[scancode] ? 1 : 0;
}

func u32 keyboard_get_keycode(u32 scancode, u16 modifiers, b32 key_event) {
  return (u32)SDL_GetKeyFromScancode((SDL_Scancode)scancode, (SDL_Keymod)modifiers, key_event != 0);
}

func const c8* keyboard_get_scancode_name(u32 scancode) {
  return SDL_GetScancodeName((SDL_Scancode)scancode);
}
