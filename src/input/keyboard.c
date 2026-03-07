// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/keyboard.h"
#include "basic/assert.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"

global_var u32 keyboard_pressed_generation[SDL_SCANCODE_COUNT] = {0};
global_var u32 keyboard_released_generation[SDL_SCANCODE_COUNT] = {0};
global_var u32 keyboard_pressed_seen[INPUT_CAPTURE_MAX_KEYS][SDL_SCANCODE_COUNT] = {0};
global_var u32 keyboard_released_seen[INPUT_CAPTURE_MAX_KEYS][SDL_SCANCODE_COUNT] = {0};
global_var u32 keyboard_pressed_seen_epoch[INPUT_CAPTURE_MAX_KEYS][SDL_SCANCODE_COUNT] = {0};
global_var u32 keyboard_released_seen_epoch[INPUT_CAPTURE_MAX_KEYS][SDL_SCANCODE_COUNT] = {0};
global_var u32 keyboard_repeat_count[SDL_SCANCODE_COUNT] = {0};

func b32 keyboard_scancode_is_valid(u32 scancode) {
  return scancode < (u32)SDL_SCANCODE_COUNT;
}

func u32 keyboard_next_generation(u32 value) {
  u32 result = value + 1;
  if (result == 0) {
    result = 1;
  }
  return result;
}

func b32 keyboard_is_available(void) {
  return SDL_HasKeyboard() ? 1 : 0;
}

func b32 keyboard_get_primary_device_id(device_id* out_id) {
  return devices_get_device(DEVICE_TYPE_KEYBOARD, 0, out_id);
}

func b32 keyboard_is_key_down(input_key key, u32 scancode) {
  (void)key;
  int key_count = 0;
  const bool* state = SDL_GetKeyboardState(&key_count);

  if (!state || scancode >= (u32)key_count) {
    return 0;
  }
  assert(scancode < (u32)key_count);

  return state[scancode] ? 1 : 0;
}

func b32 keyboard_is_key_pressed(input_key key, u32 scancode) {
  if (!keyboard_scancode_is_valid(scancode)) {
    return 0;
  }

  sz slot_idx = input_capture_get_slot(key);
  if (slot_idx >= INPUT_CAPTURE_MAX_KEYS) {
    return 0;
  }

  u32 slot_epoch = input_capture_get_slot_epoch(slot_idx);
  if (keyboard_pressed_seen_epoch[slot_idx][scancode] != slot_epoch) {
    keyboard_pressed_seen_epoch[slot_idx][scancode] = slot_epoch;
    keyboard_pressed_seen[slot_idx][scancode] = keyboard_pressed_generation[scancode];
    return 0;
  }

  u32 generation = keyboard_pressed_generation[scancode];
  if (generation == 0 || keyboard_pressed_seen[slot_idx][scancode] == generation) {
    return 0;
  }

  keyboard_pressed_seen[slot_idx][scancode] = generation;
  return 1;
}

func b32 keyboard_is_key_released(input_key key, u32 scancode) {
  if (!keyboard_scancode_is_valid(scancode)) {
    return 0;
  }

  sz slot_idx = input_capture_get_slot(key);
  if (slot_idx >= INPUT_CAPTURE_MAX_KEYS) {
    return 0;
  }

  u32 slot_epoch = input_capture_get_slot_epoch(slot_idx);
  if (keyboard_released_seen_epoch[slot_idx][scancode] != slot_epoch) {
    keyboard_released_seen_epoch[slot_idx][scancode] = slot_epoch;
    keyboard_released_seen[slot_idx][scancode] = keyboard_released_generation[scancode];
    return 0;
  }

  u32 generation = keyboard_released_generation[scancode];
  if (generation == 0 || keyboard_released_seen[slot_idx][scancode] == generation) {
    return 0;
  }

  keyboard_released_seen[slot_idx][scancode] = generation;
  return 1;
}

func u32 keyboard_get_key_repeat_count(input_key key, u32 scancode) {
  (void)key;
  if (!keyboard_scancode_is_valid(scancode)) {
    return 0;
  }
  assert(keyboard_scancode_is_valid(scancode));

  return keyboard_repeat_count[scancode];
}

func u16 keyboard_get_mods(void) {
  return (u16)SDL_GetModState();
}

func b32 keyboard_has_mods(u16 required_mods) {
  u16 current_mods = keyboard_get_mods();
  return (current_mods & required_mods) == required_mods;
}

func b32 keyboard_has_mods_exact(u16 required_mods, u16 forbidden_mods) {
  u16 current_mods = keyboard_get_mods();
  return ((current_mods & required_mods) == required_mods) && ((current_mods & forbidden_mods) == 0);
}

func b32 keyboard_is_key_down_mod(input_key key, u32 scancode, u16 required_mods, u16 forbidden_mods) {
  return keyboard_is_key_down(key, scancode) && keyboard_has_mods_exact(required_mods, forbidden_mods);
}

func u32 keyboard_get_keycode(u32 scancode, u16 modifiers, b32 key_event) {
  return (u32)SDL_GetKeyFromScancode((SDL_Scancode)scancode, (SDL_Keymod)modifiers, key_event != 0);
}

func cstr8 keyboard_get_scancode_name(u32 scancode) {
  return SDL_GetScancodeName((SDL_Scancode)scancode);
}

func void keyboard_internal_on_msg(msg* src) {
  if (src == NULL || (src->type != MSG_CORE_TYPE_KEY_DOWN && src->type != MSG_CORE_TYPE_KEY_UP)) {
    return;
  }

  if (!keyboard_scancode_is_valid(msg_core_get_keyboard(src)->scancode)) {
    return;
  }

  u32 scancode = msg_core_get_keyboard(src)->scancode;

  if (src->type == MSG_CORE_TYPE_KEY_DOWN) {
    if (!msg_core_get_keyboard(src)->repeat) {
      keyboard_pressed_generation[scancode] = keyboard_next_generation(keyboard_pressed_generation[scancode]);
    }
    if (msg_core_get_keyboard(src)->repeat) {
      keyboard_repeat_count[scancode] += 1;
    } else {
      keyboard_repeat_count[scancode] = 0;
    }
  } else if (src->type == MSG_CORE_TYPE_KEY_UP) {
    keyboard_released_generation[scancode] = keyboard_next_generation(keyboard_released_generation[scancode]);
    keyboard_repeat_count[scancode] = 0;
  }
}
