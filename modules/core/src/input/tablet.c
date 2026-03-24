// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/tablet.h"
#include "basic/assert.h"
#include "../internal.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"
#include <SDL3/SDL_hidapi.h>
#include "basic/safe.h"

typedef struct tablet_state_entry {
  tablet tablet;
  tablet_pen_state pen_state;
} tablet_state_entry;

global_var tablet_state_entry tablet_states[DEVICES_HANDLE_CAP] = {0};

func tablet_state_entry* tablet_find_state(tablet src, b32 create_if_missing) {
  profile_func_begin;
  safe_for (sz item_idx = 0; item_idx < DEVICES_HANDLE_CAP; item_idx += 1) {
    if (tablet_states[item_idx].tablet == src) {
      profile_func_end;
      return &tablet_states[item_idx];
    }
  }

  if (!create_if_missing) {
    profile_func_end;
    return NULL;
  }

  safe_for (sz item_idx = 0; item_idx < DEVICES_HANDLE_CAP; item_idx += 1) {
    if (tablet_states[item_idx].tablet == NULL) {
      tablet_states[item_idx].tablet = src;
      tablet_states[item_idx].pen_state.id = tablet_to_device(src);
      profile_func_end;
      return &tablet_states[item_idx];
    }
  }

  thread_log_error("Tablet state table is full");
  profile_func_end;
  return NULL;
}

func b32 tablet_is_valid(tablet src) {
  return src != NULL && devices_get_type((device)src) == DEVICE_TYPE_TABLET;
}

func tablet device_get_tablet(device src) {
  if (devices_get_type(src) != DEVICE_TYPE_TABLET) {
    invalid_code_path;
    return NULL;
  }

  return (tablet)src;
}

func device tablet_to_device(tablet src) {
  if (!tablet_is_valid(src)) {
    invalid_code_path;
    return NULL;
  }

  return (device)src;
}

func b32 tablet_is_available(void) {
  return tablet_get_total_count() > 0;
}

func sz tablet_get_total_count(void) {
  SDL_hid_device_info* head = SDL_hid_enumerate(0, 0);
  SDL_hid_device_info* entry = head;
  sz total = 0;

  safe_while (entry) {
    if (entry->usage_page == 0x0D) {
      total += 1;
    }
    entry = entry->next;
  }

  if (head) {
    SDL_hid_free_enumeration(head);
  }

  return total;
}

func tablet tablet_get_from_idx(sz idx) {
  profile_func_begin;
  device src = devices_get_device(DEVICE_TYPE_TABLET, idx);
  profile_func_end;
  return device_is_valid(src) ? device_get_tablet(src) : NULL;
}

func tablet tablet_get_primary(void) {
  return tablet_get_from_idx(0);
}

func tablet tablet_get_focused(void) {
  device src = devices_get_focused_device(DEVICE_TYPE_TABLET);
  return device_is_valid(src) ? device_get_tablet(src) : NULL;
}

func b32 tablet_get_last_pen_state(tablet src, tablet_pen_state* out_state) {
  profile_func_begin;
  if (!tablet_is_valid(src) || !out_state) {
    invalid_code_path;
    profile_func_end;
    return false;
  }

  tablet_state_entry* state = tablet_find_state(src, 0);
  if (state == NULL || !state->pen_state.pen_id) {
    profile_func_end;
    return false;
  }

  *out_state = state->pen_state;
  profile_func_end;
  return true;
}

func b32 tablet_read_hid_report(tablet src, void* dst, sz capacity, sz* out_size, i32 timeout_ms) {
  profile_func_begin;
  SDL_hid_device_info* head = SDL_hid_enumerate(0, 0);
  SDL_hid_device_info* entry = head;
  str8_short path_buf = {0};
  SDL_hid_device* hid_device = NULL;
  int read_size = -1;

  if (out_size) {
    *out_size = 0;
  }

  if (!tablet_is_valid(src) || !dst || !capacity) {
    if (head) {
      SDL_hid_free_enumeration(head);
    }
    invalid_code_path;
    profile_func_end;
    return false;
  }

  safe_while (entry) {
    if (entry->usage_page == 0x0D && cstr8_hash64(entry->path) == devices_get_instance(tablet_to_device(src))) {
      sz cpy_idx = 0;

      safe_while (entry->path && entry->path[cpy_idx] && (cpy_idx + 1) < size_of(path_buf)) {
        path_buf[cpy_idx] = entry->path[cpy_idx];
        cpy_idx += 1;
      }

      path_buf[cpy_idx] = '\0';
      break;
    }

    entry = entry->next;
  }

  if (head) {
    SDL_hid_free_enumeration(head);
  }

  if (!path_buf[0]) {
    profile_func_end;
    return false;
  }

  hid_device = SDL_hid_open_path(path_buf);
  if (!hid_device) {
    profile_func_end;
    return false;
  }

  read_size = SDL_hid_read_timeout(hid_device, (unsigned char*)dst, (size_t)capacity, (int)timeout_ms);
  SDL_hid_close(hid_device);

  if (read_size <= 0) {
    profile_func_end;
    return false;
  }

  if (out_size) {
    *out_size = (sz)read_size;
  }

  profile_func_end;
  return true;
}

func b32 tablet_internal_on_msg(msg* src, void* user_data) {
  profile_func_begin;
  (void)user_data;
  if (!src) {
    profile_func_end;
    return true;
  }

  if (src->type == MSG_CORE_TYPE_TABLET_ADDED || src->type == MSG_CORE_TYPE_TABLET_REMOVED) {
    device dev_id = msg_core_get_tablet_device(src)->device;
    b32 connected = src->type == MSG_CORE_TYPE_TABLET_ADDED;
    devices_update_runtime(dev_id, connected, connected ? (void*)(up)devices_get_instance(dev_id) : NULL);
    profile_func_end;
    return true;
  }

  if (src->type == MSG_CORE_TYPE_PEN_PROXIMITY_IN || src->type == MSG_CORE_TYPE_PEN_PROXIMITY_OUT) {
    tablet src_tablet = device_get_tablet(msg_core_get_pen_proximity(src)->device);
    tablet_state_entry* state = tablet_find_state(src_tablet, 1);
    if (state != NULL) {
      state->pen_state.id = tablet_to_device(src_tablet);
      state->pen_state.pen_id = msg_core_get_pen_proximity(src)->pen_id;
      state->pen_state.in_proximity = src->type == MSG_CORE_TYPE_PEN_PROXIMITY_IN;
      state->pen_state.window = msg_core_get_pen_proximity(src)->window;
    }
    devices_set_focus(tablet_to_device(src_tablet));
    devices_update_runtime(tablet_to_device(src_tablet), 1, (void*)(up)devices_get_instance(tablet_to_device(src_tablet)));
  } else if (src->type == MSG_CORE_TYPE_PEN_MOTION) {
    tablet src_tablet = device_get_tablet(msg_core_get_pen_motion(src)->device);
    tablet_state_entry* state = tablet_find_state(src_tablet, 1);
    if (state != NULL) {
      state->pen_state.id = tablet_to_device(src_tablet);
      state->pen_state.pen_id = msg_core_get_pen_motion(src)->pen_id;
      state->pen_state.in_proximity = 1;
      state->pen_state.input_mask = msg_core_get_pen_motion(src)->pen_state;
      state->pen_state.window = msg_core_get_pen_motion(src)->window;
      state->pen_state.x = msg_core_get_pen_motion(src)->x;
      state->pen_state.y = msg_core_get_pen_motion(src)->y;
    }
    devices_set_focus(tablet_to_device(src_tablet));
    devices_update_runtime(tablet_to_device(src_tablet), 1, (void*)(up)devices_get_instance(tablet_to_device(src_tablet)));
  } else if (src->type == MSG_CORE_TYPE_PEN_DOWN || src->type == MSG_CORE_TYPE_PEN_UP) {
    tablet src_tablet = device_get_tablet(msg_core_get_pen_touch(src)->device);
    tablet_state_entry* state = tablet_find_state(src_tablet, 1);
    if (state != NULL) {
      state->pen_state.id = tablet_to_device(src_tablet);
      state->pen_state.pen_id = msg_core_get_pen_touch(src)->pen_id;
      state->pen_state.in_proximity = 1;
      state->pen_state.touching = msg_core_get_pen_touch(src)->down;
      state->pen_state.eraser = msg_core_get_pen_touch(src)->eraser;
      state->pen_state.input_mask = msg_core_get_pen_touch(src)->pen_state;
      state->pen_state.window = msg_core_get_pen_touch(src)->window;
      state->pen_state.x = msg_core_get_pen_touch(src)->x;
      state->pen_state.y = msg_core_get_pen_touch(src)->y;
    }
    devices_set_focus(tablet_to_device(src_tablet));
    devices_update_runtime(tablet_to_device(src_tablet), 1, (void*)(up)devices_get_instance(tablet_to_device(src_tablet)));
  } else if (src->type == MSG_CORE_TYPE_PEN_BUTTON_DOWN || src->type == MSG_CORE_TYPE_PEN_BUTTON_UP) {
    tablet src_tablet = device_get_tablet(msg_core_get_pen_button(src)->device);
    tablet_state_entry* state = tablet_find_state(src_tablet, 1);
    if (state != NULL) {
      state->pen_state.id = tablet_to_device(src_tablet);
      state->pen_state.pen_id = msg_core_get_pen_button(src)->pen_id;
      state->pen_state.in_proximity = 1;
      state->pen_state.input_mask = msg_core_get_pen_button(src)->pen_state;
      state->pen_state.window = msg_core_get_pen_button(src)->window;
      state->pen_state.x = msg_core_get_pen_button(src)->x;
      state->pen_state.y = msg_core_get_pen_button(src)->y;
    }
    devices_set_focus(tablet_to_device(src_tablet));
    devices_update_runtime(tablet_to_device(src_tablet), 1, (void*)(up)devices_get_instance(tablet_to_device(src_tablet)));
  } else if (src->type == MSG_CORE_TYPE_PEN_AXIS) {
    tablet src_tablet = device_get_tablet(msg_core_get_pen_axis(src)->device);
    tablet_state_entry* state = tablet_find_state(src_tablet, 1);
    if (state != NULL) {
      state->pen_state.id = tablet_to_device(src_tablet);
      state->pen_state.pen_id = msg_core_get_pen_axis(src)->pen_id;
      state->pen_state.in_proximity = 1;
      state->pen_state.input_mask = msg_core_get_pen_axis(src)->pen_state;
      state->pen_state.window = msg_core_get_pen_axis(src)->window;
      state->pen_state.x = msg_core_get_pen_axis(src)->x;
      state->pen_state.y = msg_core_get_pen_axis(src)->y;
      if (msg_core_get_pen_axis(src)->axis < TABLET_AXIS_COUNT) {
        state->pen_state.axis_values[msg_core_get_pen_axis(src)->axis] = msg_core_get_pen_axis(src)->value;
      }
    }
    devices_set_focus(tablet_to_device(src_tablet));
    devices_update_runtime(tablet_to_device(src_tablet), 1, (void*)(up)devices_get_instance(tablet_to_device(src_tablet)));
  }
  profile_func_end;
  return true;
}
