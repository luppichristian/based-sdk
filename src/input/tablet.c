// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/tablet.h"
#include "basic/assert.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include <SDL3/SDL_hidapi.h>

local_persist tablet_pen_state tablet_cached_pen_state;

func u64 tablet_hash_path(cstr8 src) {
  u64 hash_value = 1469598103934665603ULL;
  sz idx = 0;

  if (!src) {
    return 0;
  }

  while (src[idx]) {
    hash_value ^= (u8)src[idx];
    hash_value *= 1099511628211ULL;
    idx += 1;
  }

  return hash_value;
}

func b32 tablet_is_available(void) {
  return tablet_get_count() > 0;
}

func sz tablet_get_count(void) {
  SDL_hid_device_info* head = SDL_hid_enumerate(0, 0);
  SDL_hid_device_info* entry = head;
  sz total = 0;

  while (entry) {
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

func b32 tablet_get_device_id(sz idx, device_id* out_id) {
  SDL_hid_device_info* head = SDL_hid_enumerate(0, 0);
  SDL_hid_device_info* entry = head;
  sz current_idx = 0;
  b32 found = 0;

  if (out_id) {
    *out_id = (device_id) {0};
  }

  while (entry) {
    if (entry->usage_page == 0x0D) {
      if (current_idx == idx) {
        if (out_id) {
          out_id->type = DEVICE_TYPE_TABLET;
          out_id->instance = tablet_hash_path(entry->path);
        }
        found = 1;
        break;
      }

      current_idx += 1;
    }

    entry = entry->next;
  }

  if (head) {
    SDL_hid_free_enumeration(head);
  }

  return found;
}

func b32 tablet_get_last_pen_state(tablet_pen_state* out_state) {
  if (!out_state || !tablet_cached_pen_state.pen_id) {
    return 0;
  }
  assert(out_state != NULL);

  *out_state = tablet_cached_pen_state;
  return 1;
}

func b32 tablet_read_hid_report(device_id id, void* dst, sz capacity, sz* out_size, i32 timeout_ms) {
  SDL_hid_device_info* head = SDL_hid_enumerate(0, 0);
  SDL_hid_device_info* entry = head;
  str8_short path_buf = {0};
  SDL_hid_device* device = NULL;
  int read_size = -1;

  if (out_size) {
    *out_size = 0;
  }

  if (!dst || !capacity || id.type != DEVICE_TYPE_TABLET) {
    if (head) {
      SDL_hid_free_enumeration(head);
    }
    return 0;
  }
  assert(dst != NULL);
  assert(capacity > 0);

  while (entry) {
    if (entry->usage_page == 0x0D && tablet_hash_path(entry->path) == id.instance) {
      sz copy_idx = 0;

      while (entry->path && entry->path[copy_idx] && (copy_idx + 1) < size_of(path_buf)) {
        path_buf[copy_idx] = entry->path[copy_idx];
        copy_idx += 1;
      }

      path_buf[copy_idx] = '\0';
      break;
    }

    entry = entry->next;
  }

  if (head) {
    SDL_hid_free_enumeration(head);
  }

  if (!path_buf[0]) {
    return 0;
  }

  device = SDL_hid_open_path(path_buf);
  if (!device) {
    return 0;
  }

  read_size = SDL_hid_read_timeout(device, (unsigned char*)dst, (size_t)capacity, (int)timeout_ms);
  SDL_hid_close(device);

  if (read_size <= 0) {
    return 0;
  }

  if (out_size) {
    *out_size = (sz)read_size;
  }

  return 1;
}

func void tablet_internal_on_msg(msg* src) {
  if (!src) {
    return;
  }

  switch (src->type) {
    case MSG_CORE_TYPE_PEN_PROXIMITY_IN:
      tablet_cached_pen_state.id = msg_core_get_pen_proximity(src)->device;
      tablet_cached_pen_state.pen_id = msg_core_get_pen_proximity(src)->pen_id;
      tablet_cached_pen_state.in_proximity = 1;
      tablet_cached_pen_state.window_id = msg_core_get_pen_proximity(src)->window_id;
      break;

    case MSG_CORE_TYPE_PEN_PROXIMITY_OUT:
      tablet_cached_pen_state.id = msg_core_get_pen_proximity(src)->device;
      tablet_cached_pen_state.pen_id = msg_core_get_pen_proximity(src)->pen_id;
      tablet_cached_pen_state.in_proximity = 0;
      tablet_cached_pen_state.touching = 0;
      tablet_cached_pen_state.window_id = msg_core_get_pen_proximity(src)->window_id;
      break;

    case MSG_CORE_TYPE_PEN_MOTION:
      tablet_cached_pen_state.id = msg_core_get_pen_motion(src)->device;
      tablet_cached_pen_state.pen_id = msg_core_get_pen_motion(src)->pen_id;
      tablet_cached_pen_state.in_proximity = 1;
      tablet_cached_pen_state.input_mask = msg_core_get_pen_motion(src)->pen_state;
      tablet_cached_pen_state.window_id = msg_core_get_pen_motion(src)->window_id;
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
      tablet_cached_pen_state.window_id = msg_core_get_pen_touch(src)->window_id;
      tablet_cached_pen_state.x = msg_core_get_pen_touch(src)->x;
      tablet_cached_pen_state.y = msg_core_get_pen_touch(src)->y;
      break;

    case MSG_CORE_TYPE_PEN_BUTTON_DOWN:
    case MSG_CORE_TYPE_PEN_BUTTON_UP:
      tablet_cached_pen_state.id = msg_core_get_pen_button(src)->device;
      tablet_cached_pen_state.pen_id = msg_core_get_pen_button(src)->pen_id;
      tablet_cached_pen_state.in_proximity = 1;
      tablet_cached_pen_state.input_mask = msg_core_get_pen_button(src)->pen_state;
      tablet_cached_pen_state.window_id = msg_core_get_pen_button(src)->window_id;
      tablet_cached_pen_state.x = msg_core_get_pen_button(src)->x;
      tablet_cached_pen_state.y = msg_core_get_pen_button(src)->y;
      break;

    case MSG_CORE_TYPE_PEN_AXIS:
      tablet_cached_pen_state.id = msg_core_get_pen_axis(src)->device;
      tablet_cached_pen_state.pen_id = msg_core_get_pen_axis(src)->pen_id;
      tablet_cached_pen_state.in_proximity = 1;
      tablet_cached_pen_state.input_mask = msg_core_get_pen_axis(src)->pen_state;
      tablet_cached_pen_state.window_id = msg_core_get_pen_axis(src)->window_id;
      tablet_cached_pen_state.x = msg_core_get_pen_axis(src)->x;
      tablet_cached_pen_state.y = msg_core_get_pen_axis(src)->y;
      if (msg_core_get_pen_axis(src)->axis < TABLET_AXIS_COUNT) {
        tablet_cached_pen_state.axis_values[msg_core_get_pen_axis(src)->axis] = msg_core_get_pen_axis(src)->value;
      }
      break;

    default:
      break;
  }
}
