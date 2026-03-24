// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/keyboard.h"
#include "basic/assert.h"
#include "../internal.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"
#include "basic/safe.h"

typedef struct keyboard_state_entry {
  keyboard keyboard;
  keymod mods;
  b32 key_state[SDL_SCANCODE_COUNT];
} keyboard_state_entry;

global_var keyboard_state_entry keyboard_states[DEVICES_HANDLE_CAP] = {0};

func keyboard_state_entry* keyboard_find_state(keyboard src, b32 create_if_missing) {
  profile_func_begin;
  safe_for (sz item_idx = 0; item_idx < DEVICES_HANDLE_CAP; item_idx += 1) {
    if (keyboard_states[item_idx].keyboard == src) {
      profile_func_end;
      return &keyboard_states[item_idx];
    }
  }

  if (!create_if_missing) {
    profile_func_end;
    return NULL;
  }

  safe_for (sz item_idx = 0; item_idx < DEVICES_HANDLE_CAP; item_idx += 1) {
    if (keyboard_states[item_idx].keyboard == NULL) {
      keyboard_states[item_idx].keyboard = src;
      profile_func_end;
      return &keyboard_states[item_idx];
    }
  }

  thread_log_error("Keyboard state table is full");
  profile_func_end;
  return NULL;
}

func b32 keyboard_internal_scancode_is_valid(u32 scancode) {
  return scancode < (u32)SDL_SCANCODE_COUNT;
}

func u32 keyboard_internal_scancode_from_vkey(vkey key) {
  profile_func_begin;
  if (!vkey_is_valid(key)) {
    profile_func_end;
    return 0;
  }

  u32 scancode = (u32)key;
  if (!keyboard_internal_scancode_is_valid(scancode)) {
    profile_func_end;
    return 0;
  }

  profile_func_end;
  return scancode;
}

func vkey keyboard_internal_vkey_from_scancode(u32 scancode) {
  profile_func_begin;
  if (!keyboard_internal_scancode_is_valid(scancode)) {
    profile_func_end;
    return VKEY_UNKNOWN;
  }

  vkey key = (vkey)scancode;
  if (!vkey_is_valid(key)) {
    profile_func_end;
    return VKEY_UNKNOWN;
  }

  profile_func_end;
  return key;
}

func b32 keyboard_is_valid(keyboard src) {
  return src != NULL && devices_get_type((device)src) == DEVICE_TYPE_KEYBOARD;
}

func keyboard device_get_keyboard(device src) {
  if (devices_get_type(src) != DEVICE_TYPE_KEYBOARD) {
    invalid_code_path;
    return NULL;
  }

  return (keyboard)src;
}

func device keyboard_to_device(keyboard src) {
  if (!keyboard_is_valid(src)) {
    invalid_code_path;
    return NULL;
  }

  return (device)src;
}

func b32 keyboard_is_available(void) {
  return devices_get_type_count(DEVICE_TYPE_KEYBOARD) > 0;
}

func keyboard keyboard_get_primary(void) {
  device src = devices_get_device(DEVICE_TYPE_KEYBOARD, 0);
  return device_is_valid(src) ? device_get_keyboard(src) : NULL;
}

func keyboard keyboard_get_focused(void) {
  device src = devices_get_focused_device(DEVICE_TYPE_KEYBOARD);
  return device_is_valid(src) ? device_get_keyboard(src) : NULL;
}

func b32 keyboard_is_key_down(keyboard src, vkey key) {
  profile_func_begin;
  if (!keyboard_is_valid(src)) {
    invalid_code_path;
    profile_func_end;
    return false;
  }

  u32 scancode = keyboard_internal_scancode_from_vkey(key);
  keyboard_state_entry* state = keyboard_find_state(src, 0);
  if (state == NULL || !keyboard_internal_scancode_is_valid(scancode)) {
    profile_func_end;
    return false;
  }

  profile_func_end;
  return state->key_state[scancode];
}

func keymod keyboard_get_mods(keyboard src) {
  if (!keyboard_is_valid(src)) {
    invalid_code_path;
    return 0;
  }

  keyboard_state_entry* state = keyboard_find_state(src, 0);
  return state ? state->mods : 0;
}

func b32 keyboard_has_mods(keyboard src, keymod required_mods) {
  keymod current_mods = keyboard_get_mods(src);
  return (current_mods & required_mods) == required_mods;
}

func b32 keyboard_has_mods_exact(keyboard src, keymod required_mods, keymod forbidden_mods) {
  keymod current_mods = keyboard_get_mods(src);
  return ((current_mods & required_mods) == required_mods) && ((current_mods & forbidden_mods) == 0);
}

func b32 keyboard_is_key_down_mod(keyboard src, vkey key, keymod required_mods, keymod forbidden_mods) {
  profile_func_begin;
  b32 result = keyboard_is_key_down(src, key) && keyboard_has_mods_exact(src, required_mods, forbidden_mods);
  profile_func_end;
  return result;
}

func i32 keyboard_internal_keycode_from_vkey(vkey key, keymod modifiers, b32 key_event) {
  profile_func_begin;
  u32 scancode = keyboard_internal_scancode_from_vkey(key);
  if (!keyboard_internal_scancode_is_valid(scancode)) {
    profile_func_end;
    return 0;
  }

  i32 result = (i32)SDL_GetKeyFromScancode((SDL_Scancode)scancode, (SDL_Keymod)modifiers, key_event != 0);
  profile_func_end;
  return result;
}

func cstr8 keyboard_get_key_name(keyboard src, vkey key) {
  profile_func_begin;
  if (!keyboard_is_valid(src)) {
    invalid_code_path;
    profile_func_end;
    return "";
  }

  u32 scancode = keyboard_internal_scancode_from_vkey(key);
  if (!keyboard_internal_scancode_is_valid(scancode)) {
    profile_func_end;
    return "";
  }

  cstr8 result = SDL_GetScancodeName((SDL_Scancode)scancode);
  profile_func_end;
  return result;
}

func cstr8 keyboard_get_key_display_name(keyboard src, vkey key, keymod modifiers, b32 key_event) {
  profile_func_begin;
  if (!keyboard_is_valid(src)) {
    invalid_code_path;
    profile_func_end;
    return "";
  }

  i32 keycode = keyboard_internal_keycode_from_vkey(key, modifiers, key_event);
  if (keycode == 0) {
    profile_func_end;
    return "";
  }

  cstr8 result = SDL_GetKeyName((SDL_Keycode)keycode);
  profile_func_end;
  return result;
}

func b32 keyboard_internal_on_msg(msg* src, void* user_data) {
  profile_func_begin;
  (void)user_data;
  if (src == NULL) {
    profile_func_end;
    return true;
  }

  if (src->type == MSG_CORE_TYPE_KEYBOARD_ADDED || src->type == MSG_CORE_TYPE_KEYBOARD_REMOVED) {
    device dev_id = msg_core_get_keyboard_device(src)->device;
    b32 connected = src->type == MSG_CORE_TYPE_KEYBOARD_ADDED;
    devices_update_runtime(dev_id, connected, connected ? (void*)(up)devices_get_instance(dev_id) : NULL);
  }

  if (!(src->type == MSG_CORE_TYPE_KEY_DOWN || src->type == MSG_CORE_TYPE_KEY_UP)) {
    profile_func_end;
    return true;
  }

  keyboard src_keyboard = device_get_keyboard(msg_core_get_keyboard(src)->device);
  keyboard_state_entry* state = keyboard_find_state(src_keyboard, 1);
  u32 scancode = keyboard_internal_scancode_from_vkey(msg_core_get_keyboard(src)->key);
  if (state != NULL) {
    state->mods = msg_core_get_keyboard(src)->modifiers;
    if (keyboard_internal_scancode_is_valid(scancode)) {
      state->key_state[scancode] = msg_core_get_keyboard(src)->down;
    }
  }
  devices_update_runtime(keyboard_to_device(src_keyboard), 1, (void*)(up)devices_get_instance(keyboard_to_device(src_keyboard)));
  devices_set_focus(keyboard_to_device(src_keyboard));
  profile_func_end;
  return true;
}
