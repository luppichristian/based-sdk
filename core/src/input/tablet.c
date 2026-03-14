// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/tablet.h"
#include "basic/assert.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"
#include <SDL3/SDL_hidapi.h>
#include "basic/safe.h"

local_persist tablet_pen_state tablet_cached_pen_state;

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

func device tablet_get_device(sz idx) {
  profile_func_begin;
  SDL_hid_device_info* head = SDL_hid_enumerate(0, 0);
  SDL_hid_device_info* entry = head;
  sz current_idx = 0;
  device result = NULL;

  safe_while (entry) {
    if (entry->usage_page == 0x0D) {
      if (current_idx == idx) {
        result = devices_make_id(DEVICE_TYPE_TABLET, cstr8_hash64(entry->path));
        break;
      }

      current_idx += 1;
    }

    entry = entry->next;
  }

  if (head) {
    SDL_hid_free_enumeration(head);
  }

  profile_func_end;
  return result;
}

func b32 tablet_get_last_pen_state(tablet_pen_state* out_state) {
  profile_func_begin;
  if (!out_state || !tablet_cached_pen_state.pen_id) {
    profile_func_end;
    return false;
  }
  assert(out_state != NULL);

  *out_state = tablet_cached_pen_state;
  profile_func_end;
  return true;
}

func b32 tablet_read_hid_report(device dev_id, void* dst, sz capacity, sz* out_size, i32 timeout_ms) {
  profile_func_begin;
  SDL_hid_device_info* head = SDL_hid_enumerate(0, 0);
  SDL_hid_device_info* entry = head;
  str8_short path_buf = {0};
  SDL_hid_device* device = NULL;
  int read_size = -1;

  if (out_size) {
    *out_size = 0;
  }

  if (!dst || !capacity || devices_get_type(dev_id) != DEVICE_TYPE_TABLET) {
    if (head) {
      SDL_hid_free_enumeration(head);
    }
    profile_func_end;
    return false;
  }
  assert(dst != NULL);
  assert(capacity > 0);

  safe_while (entry) {
    if (entry->usage_page == 0x0D && cstr8_hash64(entry->path) == devices_get_instance(dev_id)) {
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

  device = SDL_hid_open_path(path_buf);
  if (!device) {
    profile_func_end;
    return false;
  }

  read_size = SDL_hid_read_timeout(device, (unsigned char*)dst, (size_t)capacity, (int)timeout_ms);
  SDL_hid_close(device);

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

func void tablet_internal_on_msg(msg* src) {
  profile_func_begin;
  if (!src) {
    profile_func_end;
    return;
  }

  switch (src->type) {
    case MSG_CORE_TYPE_PEN_PROXIMITY_IN:
      tablet_cached_pen_state.id = msg_core_get_pen_proximity(src)->device;
      tablet_cached_pen_state.pen_id = msg_core_get_pen_proximity(src)->pen_id;
      tablet_cached_pen_state.in_proximity = 1;
      tablet_cached_pen_state.window = msg_core_get_pen_proximity(src)->window;
      break;

    case MSG_CORE_TYPE_PEN_PROXIMITY_OUT:
      tablet_cached_pen_state.id = msg_core_get_pen_proximity(src)->device;
      tablet_cached_pen_state.pen_id = msg_core_get_pen_proximity(src)->pen_id;
      tablet_cached_pen_state.in_proximity = 0;
      tablet_cached_pen_state.touching = 0;
      tablet_cached_pen_state.window = msg_core_get_pen_proximity(src)->window;
      break;

    case MSG_CORE_TYPE_PEN_MOTION:
      tablet_cached_pen_state.id = msg_core_get_pen_motion(src)->device;
      tablet_cached_pen_state.pen_id = msg_core_get_pen_motion(src)->pen_id;
      tablet_cached_pen_state.in_proximity = 1;
      tablet_cached_pen_state.input_mask = msg_core_get_pen_motion(src)->pen_state;
      tablet_cached_pen_state.window = msg_core_get_pen_motion(src)->window;
      tablet_cached_pen_state.x = msg_core_get_pen_motion(src)->x;
      tablet_cached_pen_state.y = msg_core_get_pen_motion(src)->y;
      break;

    case MSG_CORE_TYPE_PEN_DOWN:
    case MSG_CORE_TYPE_PEN_UP:
      tablet_cached_pen_state.id = msg_core_get_pen_touch(src)->device;
      tablet_cached_pen_state.pen_id = msg_core_get_pen_touch(src)->pen_id;
      tablet_cached_pen_state.in_proximity = 1;
      tablet_cached_pen_state.touching = msg_core_get_pen_touch(src)->down;
      tablet_cached_pen_state.eraser = msg_core_get_pen_touch(src)->eraser;
      tablet_cached_pen_state.input_mask = msg_core_get_pen_touch(src)->pen_state;
      tablet_cached_pen_state.window = msg_core_get_pen_touch(src)->window;
      tablet_cached_pen_state.x = msg_core_get_pen_touch(src)->x;
      tablet_cached_pen_state.y = msg_core_get_pen_touch(src)->y;
      break;

    case MSG_CORE_TYPE_PEN_BUTTON_DOWN:
    case MSG_CORE_TYPE_PEN_BUTTON_UP:
      tablet_cached_pen_state.id = msg_core_get_pen_button(src)->device;
      tablet_cached_pen_state.pen_id = msg_core_get_pen_button(src)->pen_id;
      tablet_cached_pen_state.in_proximity = 1;
      tablet_cached_pen_state.input_mask = msg_core_get_pen_button(src)->pen_state;
      tablet_cached_pen_state.window = msg_core_get_pen_button(src)->window;
      tablet_cached_pen_state.x = msg_core_get_pen_button(src)->x;
      tablet_cached_pen_state.y = msg_core_get_pen_button(src)->y;
      break;

    case MSG_CORE_TYPE_PEN_AXIS:
      tablet_cached_pen_state.id = msg_core_get_pen_axis(src)->device;
      tablet_cached_pen_state.pen_id = msg_core_get_pen_axis(src)->pen_id;
      tablet_cached_pen_state.in_proximity = 1;
      tablet_cached_pen_state.input_mask = msg_core_get_pen_axis(src)->pen_state;
      tablet_cached_pen_state.window = msg_core_get_pen_axis(src)->window;
      tablet_cached_pen_state.x = msg_core_get_pen_axis(src)->x;
      tablet_cached_pen_state.y = msg_core_get_pen_axis(src)->y;
      if (msg_core_get_pen_axis(src)->axis < TABLET_AXIS_COUNT) {
        tablet_cached_pen_state.axis_values[msg_core_get_pen_axis(src)->axis] = msg_core_get_pen_axis(src)->value;
      }
      break;

    default:
      break;
  }
  profile_func_end;
}
