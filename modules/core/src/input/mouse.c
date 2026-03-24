// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/mouse.h"
#include "basic/assert.h"
#include "../internal.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

typedef struct mouse_state_entry {
  mouse mouse;
  mouse_state buttons;
} mouse_state_entry;

global_var mouse_state_entry mouse_states[DEVICES_HANDLE_CAP] = {0};

func mouse_state_entry* mouse_find_state(mouse src, b32 create_if_missing) {
  profile_func_begin;
  for (sz item_idx = 0; item_idx < DEVICES_HANDLE_CAP; item_idx += 1) {
    if (mouse_states[item_idx].mouse == src) {
      profile_func_end;
      return &mouse_states[item_idx];
    }
  }

  if (!create_if_missing) {
    profile_func_end;
    return NULL;
  }

  for (sz item_idx = 0; item_idx < DEVICES_HANDLE_CAP; item_idx += 1) {
    if (mouse_states[item_idx].mouse == NULL) {
      mouse_states[item_idx].mouse = src;
      profile_func_end;
      return &mouse_states[item_idx];
    }
  }

  thread_log_error("Mouse state table is full");
  profile_func_end;
  return NULL;
}

func b32 mouse_button_is_valid(mouse_button button) {
  return button >= 0 && button < MOUSE_BUTTON_COUNT;
}

func mouse_state mouse_make_state(SDL_MouseButtonFlags buttons) {
  profile_func_begin;
  mouse_state result = 0;
  if ((buttons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) != 0) {
    result |= bit(MOUSE_BUTTON_LEFT);
  }
  if ((buttons & SDL_BUTTON_MASK(SDL_BUTTON_MIDDLE)) != 0) {
    result |= bit(MOUSE_BUTTON_MIDDLE);
  }
  if ((buttons & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT)) != 0) {
    result |= bit(MOUSE_BUTTON_RIGHT);
  }
  if ((buttons & SDL_BUTTON_MASK(SDL_BUTTON_X1)) != 0) {
    result |= bit(MOUSE_BUTTON_X1);
  }
  if ((buttons & SDL_BUTTON_MASK(SDL_BUTTON_X2)) != 0) {
    result |= bit(MOUSE_BUTTON_X2);
  }
  profile_func_end;
  return result;
}

func b32 mouse_is_valid(mouse src) {
  return src != NULL && devices_get_type((device)src) == DEVICE_TYPE_MOUSE;
}

func mouse device_get_mouse(device src) {
  if (devices_get_type(src) != DEVICE_TYPE_MOUSE) {
    invalid_code_path;
    return NULL;
  }

  return (mouse)src;
}

func device mouse_to_device(mouse src) {
  if (!mouse_is_valid(src)) {
    invalid_code_path;
    return NULL;
  }

  return (device)src;
}

func b32 mouse_is_available(void) {
  return devices_get_type_count(DEVICE_TYPE_MOUSE) > 0;
}

func mouse mouse_get_primary(void) {
  device src = devices_get_device(DEVICE_TYPE_MOUSE, 0);
  return device_is_valid(src) ? device_get_mouse(src) : NULL;
}

func mouse mouse_get_focused(void) {
  device src = devices_get_focused_device(DEVICE_TYPE_MOUSE);
  return device_is_valid(src) ? device_get_mouse(src) : NULL;
}

func mouse_state mouse_get_state(mouse src) {
  if (!mouse_is_valid(src)) {
    invalid_code_path;
    return 0;
  }

  mouse_state_entry* state = mouse_find_state(src, 0);
  return state ? state->buttons : 0;
}

func b32 mouse_is_button_down(mouse src, mouse_button button) {
  profile_func_begin;
  mouse_state state = mouse_get_state(src);
  if (!mouse_button_is_valid(button)) {
    profile_func_end;
    return false;
  }

  profile_func_end;
  return (state & bit(button)) != 0 ? true : false;
}

func b32 mouse_internal_on_msg(msg* src, void* user_data) {
  profile_func_begin;
  (void)user_data;
  if (src == NULL) {
    profile_func_end;
    return true;
  }

  if (src->type == MSG_CORE_TYPE_MOUSE_ADDED || src->type == MSG_CORE_TYPE_MOUSE_REMOVED) {
    device dev_id = msg_core_get_mouse_device(src)->device;
    b32 connected = src->type == MSG_CORE_TYPE_MOUSE_ADDED;
    devices_update_runtime(dev_id, connected, connected ? (void*)(up)devices_get_instance(dev_id) : NULL);
    profile_func_end;
    return true;
  }

  if (src->type == MSG_CORE_TYPE_MOUSE_MOTION) {
    mouse src_mouse = device_get_mouse(msg_core_get_mouse_motion(src)->device);
    mouse_state_entry* state = mouse_find_state(src_mouse, 1);
    if (state != NULL) {
      state->buttons = mouse_make_state((SDL_MouseButtonFlags)msg_core_get_mouse_motion(src)->button_mask);
    }
    devices_update_runtime(mouse_to_device(src_mouse), 1, (void*)(up)devices_get_instance(mouse_to_device(src_mouse)));
    devices_set_focus(mouse_to_device(src_mouse));
    profile_func_end;
    return true;
  }

  if (src->type == MSG_CORE_TYPE_MOUSE_BUTTON_DOWN || src->type == MSG_CORE_TYPE_MOUSE_BUTTON_UP) {
    mouse src_mouse = device_get_mouse(msg_core_get_mouse_button(src)->device);
    mouse_state_entry* state = mouse_find_state(src_mouse, 1);
    if (state != NULL && msg_core_get_mouse_button(src)->button < MOUSE_BUTTON_COUNT) {
      if (msg_core_get_mouse_button(src)->down) {
        state->buttons |= bit(msg_core_get_mouse_button(src)->button);
      } else {
        state->buttons &= ~bit(msg_core_get_mouse_button(src)->button);
      }
    }
    devices_update_runtime(mouse_to_device(src_mouse), 1, (void*)(up)devices_get_instance(mouse_to_device(src_mouse)));
    devices_set_focus(mouse_to_device(src_mouse));
    profile_func_end;
    return true;
  }

  if (src->type == MSG_CORE_TYPE_MOUSE_WHEEL) {
    mouse src_mouse = device_get_mouse(msg_core_get_mouse_wheel(src)->device);
    devices_update_runtime(mouse_to_device(src_mouse), 1, (void*)(up)devices_get_instance(mouse_to_device(src_mouse)));
    devices_set_focus(mouse_to_device(src_mouse));
  }
  profile_func_end;
  return true;
}
