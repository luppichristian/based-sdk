// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/keyboard.h"
#include "basic/assert.h"
#include "../internal.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "interface/window.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

global_var u32 keyboard_repeat_count[SDL_SCANCODE_COUNT] = {0};

func b32 keyboard_scancode_is_valid(keyboard_scancode scancode) {
  return scancode < (u32)SDL_SCANCODE_COUNT;
}

func b32 keyboard_is_available(void) {
  return SDL_HasKeyboard() ? true : false;
}

func device keyboard_get_primary_device(void) {
  return devices_get_device(DEVICE_TYPE_KEYBOARD, 0);
}

func b32 keyboard_is_key_down(keyboard_scancode scancode) {
  profile_func_begin;
  int key_count = 0;
  const bool* state = SDL_GetKeyboardState(&key_count);

  if (!state || scancode >= (u32)key_count) {
    profile_func_end;
    return false;
  }
  assert(scancode < (u32)key_count);

  profile_func_end;
  return state[scancode] ? true : false;
}

func u32 keyboard_get_key_repeat_count(keyboard_scancode scancode) {
  profile_func_begin;
  if (!keyboard_scancode_is_valid(scancode)) {
    profile_func_end;
    return 0;
  }
  assert(keyboard_scancode_is_valid(scancode));

  profile_func_end;
  return keyboard_repeat_count[scancode];
}

func keymod keyboard_get_mods(void) {
  return (keymod)SDL_GetModState();
}

func b32 keyboard_has_mods(keymod required_mods) {
  keymod current_mods = keyboard_get_mods();
  return (current_mods & required_mods) == required_mods;
}

func b32 keyboard_has_mods_exact(keymod required_mods, keymod forbidden_mods) {
  keymod current_mods = keyboard_get_mods();
  return ((current_mods & required_mods) == required_mods) && ((current_mods & forbidden_mods) == 0);
}

func b32 keyboard_is_key_down_mod(keyboard_scancode scancode, keymod required_mods, keymod forbidden_mods) {
  return keyboard_is_key_down(scancode) && keyboard_has_mods_exact(required_mods, forbidden_mods);
}

func keyboard_keycode keyboard_get_keycode(keyboard_scancode scancode, keymod modifiers, b32 key_event) {
  return (keyboard_keycode)SDL_GetKeyFromScancode((SDL_Scancode)scancode, (SDL_Keymod)modifiers, key_event != 0);
}

func cstr8 keyboard_get_scancode_name(keyboard_scancode scancode) {
  return SDL_GetScancodeName((SDL_Scancode)scancode);
}

func b32 keyboard_start_text_input(window opt_window) {
  profile_func_begin;
  SDL_Window* window_ptr = NULL;
  if (window_id_is_valid(opt_window)) {
    window_ptr = SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(opt_window));
  }
  b32 result = SDL_StartTextInput(window_ptr);
  profile_func_end;
  return result;
}

func b32 keyboard_stop_text_input(window opt_window) {
  profile_func_begin;
  SDL_Window* window_ptr = NULL;
  if (window_id_is_valid(opt_window)) {
    window_ptr = SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(opt_window));
  }
  b32 result = SDL_StopTextInput(window_ptr);
  profile_func_end;
  return result;
}

func b32 keyboard_is_text_input_active(window opt_window) {
  profile_func_begin;
  SDL_Window* window_ptr = NULL;
  if (window_id_is_valid(opt_window)) {
    window_ptr = SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(opt_window));
  }
  b32 result = SDL_TextInputActive(window_ptr) ? true : false;
  profile_func_end;
  return result;
}

func b32 keyboard_set_text_input_area(window opt_window, i32 xpos, i32 ypos, i32 width, i32 height) {
  profile_func_begin;
  SDL_Window* window_ptr = NULL;
  if (window_id_is_valid(opt_window)) {
    window_ptr = SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(opt_window));
  }
  SDL_Rect area = {.x = xpos, .y = ypos, .w = width, .h = height};
  b32 result = SDL_SetTextInputArea(window_ptr, &area, 0);
  profile_func_end;
  return result;
}

func void keyboard_internal_on_msg(msg* src) {
  profile_func_begin;
  if (src == NULL || (src->type != MSG_CORE_TYPE_KEY_DOWN && src->type != MSG_CORE_TYPE_KEY_UP)) {
    profile_func_end;
    return;
  }

  if (!keyboard_scancode_is_valid(msg_core_get_keyboard(src)->scancode)) {
    profile_func_end;
    return;
  }

  keyboard_scancode scancode = msg_core_get_keyboard(src)->scancode;

  if (src->type == MSG_CORE_TYPE_KEY_DOWN) {
    if (msg_core_get_keyboard(src)->repeat) {
      keyboard_repeat_count[scancode] += 1;
    } else {
      keyboard_repeat_count[scancode] = 0;
    }
  } else if (src->type == MSG_CORE_TYPE_KEY_UP) {
    keyboard_repeat_count[scancode] = 0;
  }
  profile_func_end;
}
