// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/tablet.h"
#include "input/msg.h"
#include "../sdl3_include.h"
#include <SDL3/SDL_hidapi.h>

local_persist tablet_pen_state tablet_cached_pen_state;

func u64 tablet_hash_path(const c8* src) {
  u64 hash_value = 1469598103934665603ULL;
  sz index = 0;

  if (!src) {
    return 0;
  }

  while (src[index]) {
    hash_value ^= (u8)src[index];
    hash_value *= 1099511628211ULL;
    index += 1;
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

func b32 tablet_get_device_id(sz index, input_device_id* out_id) {
  SDL_hid_device_info* head = SDL_hid_enumerate(0, 0);
  SDL_hid_device_info* entry = head;
  sz current_index = 0;
  b32 found = 0;

  if (out_id) {
    *out_id = (input_device_id){0};
  }

  while (entry) {
    if (entry->usage_page == 0x0D) {
      if (current_index == index) {
        if (out_id) {
          out_id->type = INPUT_DEVICE_TYPE_TABLET;
          out_id->instance = tablet_hash_path(entry->path);
        }
        found = 1;
        break;
      }

      current_index += 1;
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

  *out_state = tablet_cached_pen_state;
  return 1;
}

func b32 tablet_read_hid_report(input_device_id id, void* dst, sz capacity, sz* out_size, i32 timeout_ms) {
  SDL_hid_device_info* head = SDL_hid_enumerate(0, 0);
  SDL_hid_device_info* entry = head;
  c8 path_buf[INPUT_DEVICE_NAME_CAPACITY] = {0};
  SDL_hid_device* device = NULL;
  int read_size = -1;

  if (out_size) {
    *out_size = 0;
  }

  if (!dst || !capacity || id.type != INPUT_DEVICE_TYPE_TABLET) {
    if (head) {
      SDL_hid_free_enumeration(head);
    }
    return 0;
  }

  while (entry) {
    if (entry->usage_page == 0x0D && tablet_hash_path(entry->path) == id.instance) {
      sz copy_index = 0;

      while (entry->path && entry->path[copy_index] && (copy_index + 1) < size_of(path_buf)) {
        path_buf[copy_index] = entry->path[copy_index];
        copy_index += 1;
      }

      path_buf[copy_index] = '\0';
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

func void tablet_internal_on_msg(const msg* src) {
  if (!src) {
    return;
  }

  switch (src->type) {
    case MSG_TYPE_PEN_PROXIMITY_IN:
      tablet_cached_pen_state.pen_id = src->pen_proximity.pen_id;
      tablet_cached_pen_state.in_proximity = 1;
      tablet_cached_pen_state.window_id = src->pen_proximity.window_id;
      break;

    case MSG_TYPE_PEN_PROXIMITY_OUT:
      tablet_cached_pen_state.pen_id = src->pen_proximity.pen_id;
      tablet_cached_pen_state.in_proximity = 0;
      tablet_cached_pen_state.touching = 0;
      tablet_cached_pen_state.window_id = src->pen_proximity.window_id;
      break;

    case MSG_TYPE_PEN_MOTION:
      tablet_cached_pen_state.pen_id = src->pen_motion.pen_id;
      tablet_cached_pen_state.in_proximity = 1;
      tablet_cached_pen_state.input_mask = src->pen_motion.pen_state;
      tablet_cached_pen_state.window_id = src->pen_motion.window_id;
      tablet_cached_pen_state.x = src->pen_motion.x;
      tablet_cached_pen_state.y = src->pen_motion.y;
      break;

    case MSG_TYPE_PEN_DOWN:
    case MSG_TYPE_PEN_UP:
      tablet_cached_pen_state.pen_id = src->pen_touch.pen_id;
      tablet_cached_pen_state.in_proximity = 1;
      tablet_cached_pen_state.touching = src->pen_touch.down;
      tablet_cached_pen_state.eraser = src->pen_touch.eraser;
      tablet_cached_pen_state.input_mask = src->pen_touch.pen_state;
      tablet_cached_pen_state.window_id = src->pen_touch.window_id;
      tablet_cached_pen_state.x = src->pen_touch.x;
      tablet_cached_pen_state.y = src->pen_touch.y;
      break;

    case MSG_TYPE_PEN_BUTTON_DOWN:
    case MSG_TYPE_PEN_BUTTON_UP:
      tablet_cached_pen_state.pen_id = src->pen_button.pen_id;
      tablet_cached_pen_state.in_proximity = 1;
      tablet_cached_pen_state.input_mask = src->pen_button.pen_state;
      tablet_cached_pen_state.window_id = src->pen_button.window_id;
      tablet_cached_pen_state.x = src->pen_button.x;
      tablet_cached_pen_state.y = src->pen_button.y;
      break;

    case MSG_TYPE_PEN_AXIS:
      tablet_cached_pen_state.pen_id = src->pen_axis.pen_id;
      tablet_cached_pen_state.in_proximity = 1;
      tablet_cached_pen_state.input_mask = src->pen_axis.pen_state;
      tablet_cached_pen_state.window_id = src->pen_axis.window_id;
      tablet_cached_pen_state.x = src->pen_axis.x;
      tablet_cached_pen_state.y = src->pen_axis.y;
      if (src->pen_axis.axis < TABLET_AXIS_COUNT) {
        tablet_cached_pen_state.axis_values[src->pen_axis.axis] = src->pen_axis.value;
      }
      break;

    default:
      break;
  }
}
