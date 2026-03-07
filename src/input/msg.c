// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/assert.h"
#include "basic/utility_defines.h"
#include "context/thread_ctx.h"
#include "basic/profiler.h"
#include <SDL3/SDL_hidapi.h>

func void tablet_internal_on_msg(msg* src);
func void keyboard_internal_on_msg(msg* src);
func void mouse_internal_on_msg(msg* src);
func void gamepads_internal_on_msg(msg* src);
func void bindings_internal_on_msg(const msg* src);

func void msg_notify_internal_listeners(const msg* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  msg* src_mut = (msg*)src;
  keyboard_internal_on_msg(src_mut);
  mouse_internal_on_msg(src_mut);
  gamepads_internal_on_msg(src_mut);
  tablet_internal_on_msg(src_mut);
  bindings_internal_on_msg(src);
  TracyCZoneEnd(__tracy_zone_ctx);
}

const_var i32 MSG_PAYLOAD_CODE = 0x42415345;
const_var sz MSG_DEVICE_TRACK_CAP = 128;
const_var u32 MSG_CATEGORY_USER_TYPE_COUNT = 0x1000u;
#define MSG_HANDLER_CAP 64u

typedef struct msg_handler_entry {
  u64 handler_id;
  msg_handler_fn handler_fn;
  void* user_data;
  i32 priority;
  u32 options;
  u32 type_min;
  u32 type_max;
} msg_handler_entry;

global_var msg_handler_entry msg_handler_entries[MSG_HANDLER_CAP] = {0};
global_var u32 msg_handler_count = 0;
global_var u64 msg_handler_next_id = 1;
global_var msg_filter_fn msg_filter_current = NULL;
global_var void* msg_filter_user_data = NULL;
global_var b32 msg_user_spaces_ready = false;
global_var u32 msg_user_space_bases[MSG_CATEGORY_MAX] = {0};

func b32 msg_remove_handler(u64 handler_id);

func b32 msg_category_is_valid(msg_category category) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return category >= MSG_CATEGORY_CORE && category < MSG_CATEGORY_MAX;
}

func b32 msg_category_needs_user_space(msg_category category) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return category > MSG_CATEGORY_CORE && category < MSG_CATEGORY_MAX;
}

func b32 msg_user_space_ensure(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (msg_user_spaces_ready) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return true;
  }

  for (msg_category category = MSG_CATEGORY_GRAPHICS; category < MSG_CATEGORY_MAX; category += 1) {
    u32 base_type = SDL_RegisterEvents((int)MSG_CATEGORY_USER_TYPE_COUNT);
    if (base_type == (u32)-1) {
      for (sz idx = 0; idx < MSG_CATEGORY_MAX; idx += 1) {
        msg_user_space_bases[idx] = 0;
      }
      msg_user_spaces_ready = false;
      TracyCZoneEnd(__tracy_zone_ctx);
      return false;
    }
    msg_user_space_bases[(sz)category] = base_type;
  }

  msg_user_spaces_ready = true;
  TracyCZoneEnd(__tracy_zone_ctx);
  return true;
}

func b32 msg_user_space_get_sdl_type(msg_category category, u32 local_type, u32* out_sdl_type) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (out_sdl_type != NULL) {
    *out_sdl_type = 0;
  }

  if (!msg_category_needs_user_space(category)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return false;
  }
  if (!msg_user_space_ensure()) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return false;
  }

  u32 base_type = msg_user_space_bases[(sz)category];
  u32 sdl_type = 0;
  if (local_type >= base_type && local_type < (base_type + MSG_CATEGORY_USER_TYPE_COUNT)) {
    sdl_type = local_type;
  } else {
    if (local_type >= MSG_CATEGORY_USER_TYPE_COUNT) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return false;
    }
    sdl_type = base_type + local_type;
  }

  if (out_sdl_type != NULL) {
    *out_sdl_type = sdl_type;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return true;
}

func b32 msg_user_space_decode_sdl_type(u32 sdl_type, msg_category* out_category, u32* out_local_type) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (out_category != NULL) {
    *out_category = MSG_CATEGORY_CORE;
  }
  if (out_local_type != NULL) {
    *out_local_type = sdl_type;
  }

  if (!msg_user_space_ensure()) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return false;
  }

  for (msg_category category = MSG_CATEGORY_GRAPHICS; category < MSG_CATEGORY_MAX; category += 1) {
    u32 base_type = msg_user_space_bases[(sz)category];
    if (sdl_type >= base_type && sdl_type < (base_type + MSG_CATEGORY_USER_TYPE_COUNT)) {
      if (out_category != NULL) {
        *out_category = category;
      }
      if (out_local_type != NULL) {
        *out_local_type = sdl_type - base_type;
      }
      TracyCZoneEnd(__tracy_zone_ctx);
      return true;
    }
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return false;
}

func b32 msg_filter_accept(const msg* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (msg_filter_current == NULL || src == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }
  b32 result = msg_filter_current(src, msg_filter_user_data);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func u64 msg_hash_path(cstr8 src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  u64 hash_value = 1469598103934665603ULL;
  sz idx = 0;

  if (!src) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  while (src[idx]) {
    hash_value ^= (u8)src[idx];
    hash_value *= 1099511628211ULL;
    idx += 1;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return hash_value;
}

func b32 msg_device_id_equal(device_id lhs, device_id rhs) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return lhs.type == rhs.type && lhs.instance == rhs.instance;
}

func b32 msg_device_list_contains(const device_id* list, sz count, device_id src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!list) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  for (sz item_idx = 0; item_idx < count; item_idx += 1) {
    if (msg_device_id_equal(list[item_idx], src)) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;
    }
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
}

func sz msg_collect_touch_devices(device_id* out_ids, sz cap) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  int count = 0;
  SDL_TouchID* ids = SDL_GetTouchDevices(&count);
  sz out_count = 0;

  for (int item_idx = 0; ids && item_idx < count && out_count < cap; item_idx += 1) {
    if (ids[item_idx] == 0) {
      continue;
    }

    out_ids[out_count].type = DEVICE_TYPE_TOUCH;
    out_ids[out_count].instance = (u64)ids[item_idx];
    out_count += 1;
  }

  if (ids) {
    SDL_free(ids);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return out_count;
}

func sz msg_collect_tablet_devices(device_id* out_ids, sz cap) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_hid_device_info* head = SDL_hid_enumerate(0, 0);
  SDL_hid_device_info* entry = head;
  sz out_count = 0;

  while (entry && out_count < cap) {
    if (entry->usage_page == 0x0D && entry->path != NULL) {
      out_ids[out_count].type = DEVICE_TYPE_TABLET;
      out_ids[out_count].instance = msg_hash_path(entry->path);
      out_count += 1;
    }

    entry = entry->next;
  }

  if (head) {
    SDL_hid_free_enumeration(head);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return out_count;
}

func void msg_post_touch_device_event(u32 type, device_id device) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  msg device_msg = {0};
  device_msg.type = type;
  msg_core_touch_device_data core_data = {.device = device};
  msg_core_fill_touch_device(&device_msg, &core_data);
  (void)_msg_post(&device_msg, CALLSITE_HERE);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void msg_post_tablet_device_event(u32 type, device_id device) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  msg device_msg = {0};
  device_msg.type = type;
  msg_core_tablet_device_data core_data = {.device = device};
  msg_core_fill_tablet_device(&device_msg, &core_data);
  (void)_msg_post(&device_msg, CALLSITE_HERE);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void msg_refresh_touch_devices(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  local_persist device_id previous_ids[MSG_DEVICE_TRACK_CAP] = {0};
  local_persist sz previous_count = 0;
  device_id current_ids[MSG_DEVICE_TRACK_CAP] = {0};
  sz current_count = msg_collect_touch_devices(current_ids, count_of(current_ids));

  for (sz item_idx = 0; item_idx < current_count; item_idx += 1) {
    if (!msg_device_list_contains(previous_ids, previous_count, current_ids[item_idx])) {
      msg_post_touch_device_event(MSG_CORE_TYPE_TOUCH_ADDED, current_ids[item_idx]);
    }
  }

  for (sz item_idx = 0; item_idx < previous_count; item_idx += 1) {
    if (!msg_device_list_contains(current_ids, current_count, previous_ids[item_idx])) {
      msg_post_touch_device_event(MSG_CORE_TYPE_TOUCH_REMOVED, previous_ids[item_idx]);
    }
  }

  previous_count = current_count;
  for (sz item_idx = 0; item_idx < previous_count; item_idx += 1) {
    previous_ids[item_idx] = current_ids[item_idx];
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void msg_refresh_tablet_devices(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  local_persist device_id previous_ids[MSG_DEVICE_TRACK_CAP] = {0};
  local_persist sz previous_count = 0;
  device_id current_ids[MSG_DEVICE_TRACK_CAP] = {0};
  sz current_count = msg_collect_tablet_devices(current_ids, count_of(current_ids));

  for (sz item_idx = 0; item_idx < current_count; item_idx += 1) {
    if (!msg_device_list_contains(previous_ids, previous_count, current_ids[item_idx])) {
      msg_post_tablet_device_event(MSG_CORE_TYPE_TABLET_ADDED, current_ids[item_idx]);
    }
  }

  for (sz item_idx = 0; item_idx < previous_count; item_idx += 1) {
    if (!msg_device_list_contains(current_ids, current_count, previous_ids[item_idx])) {
      msg_post_tablet_device_event(MSG_CORE_TYPE_TABLET_REMOVED, previous_ids[item_idx]);
    }
  }

  previous_count = current_count;
  for (sz item_idx = 0; item_idx < previous_count; item_idx += 1) {
    previous_ids[item_idx] = current_ids[item_idx];
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void msg_refresh_synthetic_device_msgs(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  msg_refresh_touch_devices();
  msg_refresh_tablet_devices();
  TracyCZoneEnd(__tracy_zone_ctx);
}

// NOTE:
// SDL uses one process-global event queue. This module intentionally mirrors
// that model: any thread may post, while one designated thread should pump/poll/wait.

func b32 msg_handler_should_run_for_stage(u32 options, msg_handler_stage stage) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  u32 stage_options = options &
                      (MSG_HANDLER_OPTION_STAGE_BEFORE_POST | MSG_HANDLER_OPTION_STAGE_AFTER_POST |
                       MSG_HANDLER_OPTION_STAGE_POST_FAILED);

  if (stage_options == 0) {
    stage_options = MSG_HANDLER_OPTION_STAGE_BEFORE_POST;
  }

  switch (stage) {
    case MSG_HANDLER_STAGE_BEFORE_POST:
      TracyCZoneEnd(__tracy_zone_ctx);
      return (stage_options & MSG_HANDLER_OPTION_STAGE_BEFORE_POST) != 0;
    case MSG_HANDLER_STAGE_AFTER_POST:
      TracyCZoneEnd(__tracy_zone_ctx);
      return (stage_options & MSG_HANDLER_OPTION_STAGE_AFTER_POST) != 0;
    case MSG_HANDLER_STAGE_POST_FAILED:
      TracyCZoneEnd(__tracy_zone_ctx);
      return (stage_options & MSG_HANDLER_OPTION_STAGE_POST_FAILED) != 0;
    default:
      TracyCZoneEnd(__tracy_zone_ctx);
      return 0;
  }
}

func b32 msg_handler_should_run_for_type(const msg_handler_entry* entry, u32 type) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (entry->type_min == 0 && entry->type_max == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  if (entry->type_min <= entry->type_max) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return in_range(type, entry->type_min, entry->type_max);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return in_range(type, entry->type_max, entry->type_min);
}

func void msg_handler_sort_entries(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  for (u32 outer_idx = 0; outer_idx < msg_handler_count; outer_idx += 1) {
    for (u32 inner_idx = outer_idx + 1; inner_idx < msg_handler_count; inner_idx += 1) {
      msg_handler_entry* lhs = &msg_handler_entries[outer_idx];
      msg_handler_entry* rhs = &msg_handler_entries[inner_idx];
      b32 swap_needed = 0;

      if (lhs->priority < rhs->priority) {
        swap_needed = 1;
      } else if (lhs->priority == rhs->priority && lhs->handler_id > rhs->handler_id) {
        swap_needed = 1;
      }

      if (swap_needed) {
        msg_handler_entry temp = *lhs;
        *lhs = *rhs;
        *rhs = temp;
      }
    }
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}

func b32 msg_dispatch_handlers(msg* posted_msg, msg_handler_stage stage) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  msg_handler_entry dispatch_entries[MSG_HANDLER_CAP] = {0};
  u64 once_handler_ids[MSG_HANDLER_CAP] = {0};
  u32 dispatch_cap = (u32)count_of(dispatch_entries);
  u32 once_handler_cap = (u32)count_of(once_handler_ids);
  u32 dispatch_count = msg_handler_count;
  u32 once_handler_count = 0;

  if (dispatch_count > dispatch_cap) {
    dispatch_count = dispatch_cap;
  }

  for (u32 idx = 0; idx < dispatch_count; idx += 1) {
    dispatch_entries[idx] = msg_handler_entries[idx];
  }

  for (u32 idx = 0; idx < dispatch_count; idx += 1) {
    msg_handler_entry* entry = &dispatch_entries[idx];

    if (entry->handler_fn == NULL) {
      continue;
    }

    if ((entry->options & MSG_HANDLER_OPTION_DISABLED) != 0) {
      continue;
    }

    if (!msg_handler_should_run_for_stage(entry->options, stage)) {
      continue;
    }

    if (!msg_handler_should_run_for_type(entry, posted_msg->type)) {
      continue;
    }

    msg_handler_result result = entry->handler_fn(posted_msg, stage, entry->user_data);

    if ((entry->options & MSG_HANDLER_OPTION_ONCE) != 0 && once_handler_count < once_handler_cap) {
      once_handler_ids[once_handler_count] = entry->handler_id;
      once_handler_count += 1;
    }

    if (result == MSG_HANDLER_RESULT_STOP_DISPATCH) {
      break;
    }

    if (stage == MSG_HANDLER_STAGE_BEFORE_POST && result == MSG_HANDLER_RESULT_CANCEL_POST) {
      for (u32 once_idx = 0; once_idx < once_handler_count; once_idx += 1) {
        (void)msg_remove_handler(once_handler_ids[once_idx]);
      }
      TracyCZoneEnd(__tracy_zone_ctx);
      return 0;
    }
  }

  for (u32 once_idx = 0; once_idx < once_handler_count; once_idx += 1) {
    (void)msg_remove_handler(once_handler_ids[once_idx]);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func void msg_apply_common(msg* dst, const SDL_Event* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  dst->type = src->type;
  dst->timestamp = src->common.timestamp;
  dst->post_site = (callsite) {0};
  dst->category = MSG_CATEGORY_CORE;
  TracyCZoneEnd(__tracy_zone_ctx);
}

func b32 msg_from_sdl(const SDL_Event* src, msg* out_msg) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!src || !out_msg) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(src != NULL);
  assert(out_msg != NULL);

  *out_msg = (msg) {0};
  msg_apply_common(out_msg, src);

  switch (src->type) {
    case SDL_EVENT_DISPLAY_ORIENTATION:
    case SDL_EVENT_DISPLAY_ADDED:
    case SDL_EVENT_DISPLAY_REMOVED:
    case SDL_EVENT_DISPLAY_MOVED:
    case SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED:
    case SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED:
    case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED:
      msg_core_fill_monitor(
          out_msg,
          &(msg_core_monitor_data) {
              .monitor = monitor_from_native_id((up)src->display.displayID),
              .data1 = (i32)src->display.data1,
              .data2 = (i32)src->display.data2,
          });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_WINDOW_SHOWN:
    case SDL_EVENT_WINDOW_HIDDEN:
    case SDL_EVENT_WINDOW_EXPOSED:
    case SDL_EVENT_WINDOW_MOVED:
    case SDL_EVENT_WINDOW_RESIZED:
    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
    case SDL_EVENT_WINDOW_METAL_VIEW_RESIZED:
    case SDL_EVENT_WINDOW_MINIMIZED:
    case SDL_EVENT_WINDOW_MAXIMIZED:
    case SDL_EVENT_WINDOW_RESTORED:
    case SDL_EVENT_WINDOW_MOUSE_ENTER:
    case SDL_EVENT_WINDOW_MOUSE_LEAVE:
    case SDL_EVENT_WINDOW_FOCUS_GAINED:
    case SDL_EVENT_WINDOW_FOCUS_LOST:
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
    case SDL_EVENT_WINDOW_HIT_TEST:
    case SDL_EVENT_WINDOW_ICCPROF_CHANGED:
    case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
    case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
    case SDL_EVENT_WINDOW_SAFE_AREA_CHANGED:
    case SDL_EVENT_WINDOW_OCCLUDED:
    case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
    case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
    case SDL_EVENT_WINDOW_DESTROYED:
    case SDL_EVENT_WINDOW_HDR_STATE_CHANGED:
      msg_core_fill_window(
          out_msg,
          &(msg_core_window_data) {
              .window = window_from_native_id((up)src->window.windowID),
              .data1 = (i32)src->window.data1,
              .data2 = (i32)src->window.data2,
          });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_KEYBOARD_ADDED:
    case SDL_EVENT_KEYBOARD_REMOVED:
      msg_core_fill_keyboard_device(
          out_msg,
          &(msg_core_keyboard_device_data) {
              .device = {.type = DEVICE_TYPE_KEYBOARD, .instance = (u64)src->kdevice.which},
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
      msg_core_fill_keyboard(
          out_msg,
          &(msg_core_keyboard_data) {
              .window = window_from_native_id((up)src->key.windowID),
              .device = {.type = DEVICE_TYPE_KEYBOARD, .instance = (u64)src->key.which},
              .scancode = (keyboard_scancode)src->key.scancode,
              .keycode = (keyboard_keycode)src->key.key,
              .modifiers = (keymod)src->key.mod,
              .raw = (keyboard_raw_key)src->key.raw,
              .down = src->key.down ? 1 : 0,
              .repeat = src->key.repeat ? 1 : 0,
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_TEXT_EDITING:
      msg_core_fill_text_editing(
          out_msg,
          &(msg_core_text_editing_data) {
              .window = window_from_native_id((up)src->edit.windowID),
              .text = src->edit.text,
              .start = (i32)src->edit.start,
              .length = (i32)src->edit.length,
          });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_TEXT_EDITING_CANDIDATES:
      msg_core_fill_text_editing_candidates(
          out_msg,
          &(msg_core_text_editing_candidates_data) {
              .window = window_from_native_id((up)src->edit_candidates.windowID),
              .candidates = (cstr8 const*)src->edit_candidates.candidates,
              .num_candidates = (i32)src->edit_candidates.num_candidates,
              .selected_candidate = (i32)src->edit_candidates.selected_candidate,
              .horizontal = src->edit_candidates.horizontal ? 1 : 0,
          });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_TEXT_INPUT:
      msg_core_fill_text_input(
          out_msg,
          &(msg_core_text_input_data) {
              .window = window_from_native_id((up)src->text.windowID),
              .text = src->text.text,
          });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_MOUSE_ADDED:
    case SDL_EVENT_MOUSE_REMOVED:
      msg_core_fill_mouse_device(
          out_msg,
          &(msg_core_mouse_device_data) {
              .device = {.type = DEVICE_TYPE_MOUSE, .instance = (u64)src->mdevice.which},
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_MOUSE_MOTION:
      msg_core_fill_mouse_motion(
          out_msg,
          &(msg_core_mouse_motion_data) {
              .window = window_from_native_id((up)src->motion.windowID),
              .device = {.type = DEVICE_TYPE_MOUSE, .instance = (u64)src->motion.which},
              .button_mask = (u32)src->motion.state,
              .x = src->motion.x,
              .y = src->motion.y,
              .xrel = src->motion.xrel,
              .yrel = src->motion.yrel,
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
      msg_core_fill_mouse_button(
          out_msg,
          &(msg_core_mouse_button_data) {
              .window = window_from_native_id((up)src->button.windowID),
              .device = {.type = DEVICE_TYPE_MOUSE, .instance = (u64)src->button.which},
              .button = (mouse_button)src->button.button,
              .down = src->button.down ? 1 : 0,
              .clicks = (u8)src->button.clicks,
              .x = src->button.x,
              .y = src->button.y,
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_MOUSE_WHEEL:
      msg_core_fill_mouse_wheel(
          out_msg,
          &(msg_core_mouse_wheel_data) {
              .window = window_from_native_id((up)src->wheel.windowID),
              .device = {.type = DEVICE_TYPE_MOUSE, .instance = (u64)src->wheel.which},
              .x = src->wheel.x,
              .y = src->wheel.y,
              .direction = (mouse_wheel_direction)src->wheel.direction,
              .mouse_x = src->wheel.mouse_x,
              .mouse_y = src->wheel.mouse_y,
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_JOYSTICK_ADDED:
    case SDL_EVENT_JOYSTICK_REMOVED:
    case SDL_EVENT_JOYSTICK_UPDATE_COMPLETE:
      msg_core_fill_joystick_device(
          out_msg,
          &(msg_core_joystick_device_data) {
              .device = {.type = DEVICE_TYPE_JOYSTICK, .instance = (u64)src->jdevice.which},
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_JOYSTICK_AXIS_MOTION:
      msg_core_fill_joystick_axis(
          out_msg,
          &(msg_core_joystick_axis_data) {
              .device = {.type = DEVICE_TYPE_JOYSTICK, .instance = (u64)src->jaxis.which},
              .axis = (u8)src->jaxis.axis,
              .value = (i16)src->jaxis.value,
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_JOYSTICK_BALL_MOTION:
      msg_core_fill_joystick_ball(
          out_msg,
          &(msg_core_joystick_ball_data) {
              .device = {.type = DEVICE_TYPE_JOYSTICK, .instance = (u64)src->jball.which},
              .ball = (u8)src->jball.ball,
              .xrel = (i16)src->jball.xrel,
              .yrel = (i16)src->jball.yrel,
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_JOYSTICK_HAT_MOTION:
      msg_core_fill_joystick_hat(
          out_msg,
          &(msg_core_joystick_hat_data) {
              .device = {.type = DEVICE_TYPE_JOYSTICK, .instance = (u64)src->jhat.which},
              .hat = (u8)src->jhat.hat,
              .value = (joystick_hat_state)src->jhat.value,
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
    case SDL_EVENT_JOYSTICK_BUTTON_UP:
      msg_core_fill_joystick_button(
          out_msg,
          &(msg_core_joystick_button_data) {
              .device = {.type = DEVICE_TYPE_JOYSTICK, .instance = (u64)src->jbutton.which},
              .button = (u8)src->jbutton.button,
              .down = src->jbutton.down ? 1 : 0,
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_JOYSTICK_BATTERY_UPDATED:
      msg_core_fill_joystick_battery(
          out_msg,
          &(msg_core_joystick_battery_data) {
              .device = {.type = DEVICE_TYPE_JOYSTICK, .instance = (u64)src->jbattery.which},
              .state = (battery_state)src->jbattery.state,
              .percent = (i32)src->jbattery.percent,
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_GAMEPAD_ADDED:
    case SDL_EVENT_GAMEPAD_REMOVED:
    case SDL_EVENT_GAMEPAD_REMAPPED:
    case SDL_EVENT_GAMEPAD_UPDATE_COMPLETE:
    case SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED:
      msg_core_fill_gamepad_device(
          out_msg,
          &(msg_core_gamepad_device_data) {
              .device = {.type = DEVICE_TYPE_GAMEPAD, .instance = (u64)src->gdevice.which},
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_GAMEPAD_AXIS_MOTION:
      msg_core_fill_gamepad_axis(
          out_msg,
          &(msg_core_gamepad_axis_data) {
              .device = {.type = DEVICE_TYPE_GAMEPAD, .instance = (u64)src->gaxis.which},
              .axis = (gamepad_axis)src->gaxis.axis,
              .value = (i16)src->gaxis.value,
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
    case SDL_EVENT_GAMEPAD_BUTTON_UP:
      msg_core_fill_gamepad_button(
          out_msg,
          &(msg_core_gamepad_button_data) {
              .device = {.type = DEVICE_TYPE_GAMEPAD, .instance = (u64)src->gbutton.which},
              .button = (gamepad_button)src->gbutton.button,
              .down = src->gbutton.down ? 1 : 0,
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    default:
      break;
  }

  switch (src->type) {
    case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
    case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
    case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
      msg_core_fill_gamepad_touchpad(
          out_msg,
          &(msg_core_gamepad_touchpad_data) {
              .device = {.type = DEVICE_TYPE_GAMEPAD, .instance = (u64)src->gtouchpad.which},
              .touchpad = (gamepad_touchpad_idx)src->gtouchpad.touchpad,
              .finger = (gamepad_finger_idx)src->gtouchpad.finger,
              .x = src->gtouchpad.x,
              .y = src->gtouchpad.y,
              .pressure = src->gtouchpad.pressure,
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:
      msg_core_fill_gamepad_sensor(
          out_msg,
          &(msg_core_gamepad_sensor_data) {
              .device = {.type = DEVICE_TYPE_GAMEPAD, .instance = (u64)src->gsensor.which},
              .sensor = (gamepad_sensor_kind)src->gsensor.sensor,
              .data = {src->gsensor.data[0], src->gsensor.data[1], src->gsensor.data[2]},
              .sensor_timestamp = (u64)src->gsensor.sensor_timestamp,
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_AUDIO_DEVICE_ADDED:
    case SDL_EVENT_AUDIO_DEVICE_REMOVED:
    case SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED:
      msg_core_fill_audio_device(
          out_msg,
          &(msg_core_audio_device_data) {
              .device = devices_make_audio_device_id(
                  (u64)src->adevice.which,
                  src->adevice.recording ? AUDIO_DEVICE_TYPE_RECORDING : AUDIO_DEVICE_TYPE_PLAYBACK),
          });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_CAMERA_DEVICE_ADDED:
    case SDL_EVENT_CAMERA_DEVICE_REMOVED:
    case SDL_EVENT_CAMERA_DEVICE_APPROVED:
    case SDL_EVENT_CAMERA_DEVICE_DENIED:
      msg_core_fill_camera_device(
          out_msg,
          &(msg_core_camera_device_data) {
              .camera = camera_from_native_id((up)src->cdevice.which),
          });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_RENDER_TARGETS_RESET:
    case SDL_EVENT_RENDER_DEVICE_RESET:
    case SDL_EVENT_RENDER_DEVICE_LOST:
      msg_core_fill_render(
          out_msg,
          &(msg_core_render_data) {
              .window = window_from_native_id((up)src->render.windowID),
          });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_FINGER_DOWN:
    case SDL_EVENT_FINGER_UP:
    case SDL_EVENT_FINGER_MOTION:
    case SDL_EVENT_FINGER_CANCELED:
      msg_core_fill_touch(
          out_msg,
          &(msg_core_touch_data) {
              .device = {.type = DEVICE_TYPE_TOUCH, .instance = (u64)src->tfinger.touchID},
              .finger_id = (finger_id)src->tfinger.fingerID,
              .x = src->tfinger.x,
              .y = src->tfinger.y,
              .dx = src->tfinger.dx,
              .dy = src->tfinger.dy,
              .pressure = src->tfinger.pressure,
              .window = window_from_native_id((up)src->tfinger.windowID),
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_PEN_PROXIMITY_IN:
    case SDL_EVENT_PEN_PROXIMITY_OUT:
      msg_core_fill_pen_proximity(
          out_msg,
          &(msg_core_pen_proximity_data) {
              .window = window_from_native_id((up)src->pproximity.windowID),
              .device = {.type = DEVICE_TYPE_TABLET, .instance = (u64)src->pproximity.which},
              .pen_id = (pen_id)src->pproximity.which,
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_PEN_MOTION:
      msg_core_fill_pen_motion(
          out_msg,
          &(msg_core_pen_motion_data) {
              .window = window_from_native_id((up)src->pmotion.windowID),
              .device = {.type = DEVICE_TYPE_TABLET, .instance = (u64)src->pmotion.which},
              .pen_id = (pen_id)src->pmotion.which,
              .pen_state = (tablet_input_flags)src->pmotion.pen_state,
              .x = src->pmotion.x,
              .y = src->pmotion.y,
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_PEN_DOWN:
    case SDL_EVENT_PEN_UP:
      msg_core_fill_pen_touch(
          out_msg,
          &(msg_core_pen_touch_data) {
              .window = window_from_native_id((up)src->ptouch.windowID),
              .device = {.type = DEVICE_TYPE_TABLET, .instance = (u64)src->ptouch.which},
              .pen_id = (pen_id)src->ptouch.which,
              .pen_state = (tablet_input_flags)src->ptouch.pen_state,
              .x = src->ptouch.x,
              .y = src->ptouch.y,
              .eraser = src->ptouch.eraser ? 1 : 0,
              .down = src->ptouch.down ? 1 : 0,
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_PEN_BUTTON_DOWN:
    case SDL_EVENT_PEN_BUTTON_UP:
      msg_core_fill_pen_button(
          out_msg,
          &(msg_core_pen_button_data) {
              .window = window_from_native_id((up)src->pbutton.windowID),
              .device = {.type = DEVICE_TYPE_TABLET, .instance = (u64)src->pbutton.which},
              .pen_id = (pen_id)src->pbutton.which,
              .pen_state = (tablet_input_flags)src->pbutton.pen_state,
              .x = src->pbutton.x,
              .y = src->pbutton.y,
              .button = (tablet_button)src->pbutton.button,
              .down = src->pbutton.down ? 1 : 0,
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_PEN_AXIS:
      msg_core_fill_pen_axis(
          out_msg,
          &(msg_core_pen_axis_data) {
              .window = window_from_native_id((up)src->paxis.windowID),
              .device = {.type = DEVICE_TYPE_TABLET, .instance = (u64)src->paxis.which},
              .pen_id = (pen_id)src->paxis.which,
              .pen_state = (tablet_input_flags)src->paxis.pen_state,
              .x = src->paxis.x,
              .y = src->paxis.y,
              .axis = (tablet_axis)src->paxis.axis,
              .value = src->paxis.value,
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_DROP_FILE:
    case SDL_EVENT_DROP_TEXT:
    case SDL_EVENT_DROP_BEGIN:
    case SDL_EVENT_DROP_COMPLETE:
    case SDL_EVENT_DROP_POSITION:
      msg_core_fill_drop(
          out_msg,
          &(msg_core_drop_data) {
              .window = window_from_native_id((up)src->drop.windowID),
              .x = src->drop.x,
              .y = src->drop.y,
              .source = src->drop.source,
              .data = src->drop.data,
          });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_CLIPBOARD_UPDATE:
      msg_core_fill_clipboard(
          out_msg,
          &(msg_core_clipboard_data) {
              .owner = src->clipboard.owner ? 1 : 0,
              .num_mime_types = (i32)src->clipboard.num_mime_types,
              .mime_types = (cstr8 const*)src->clipboard.mime_types,
          });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_SENSOR_UPDATE:
      msg_core_fill_sensor(
          out_msg,
          &(msg_core_sensor_data) {
              .sensor = sensor_from_native_id((up)src->sensor.which),
              .data = {
                       src->sensor.data[0],
                       src->sensor.data[1],
                       src->sensor.data[2],
                       src->sensor.data[3],
                       src->sensor.data[4],
                       src->sensor.data[5],
                       },
              .sensor_timestamp = (u64)src->sensor.sensor_timestamp,
      });
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    default:
      if (src->type >= SDL_EVENT_USER && src->user.code == (Sint32)MSG_PAYLOAD_CODE && src->user.data1 != NULL) {
        msg* payload = (msg*)src->user.data1;
        *out_msg = *payload;
        SDL_free(payload);
        TracyCZoneEnd(__tracy_zone_ctx);
        return 1;
      }

      if (src->type >= SDL_EVENT_USER) {
        msg_category category = MSG_CATEGORY_CORE;
        u32 local_type = src->type;
        if (msg_user_space_decode_sdl_type(src->type, &category, &local_type)) {
          out_msg->category = category;
          out_msg->type = local_type;
          TracyCZoneEnd(__tracy_zone_ctx);
          return 1;
        }
        TracyCZoneEnd(__tracy_zone_ctx);
        return 0;
      }
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;
  }
}

func b32 msg_to_sdl_event(msg* src, SDL_Event* out_event) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!src || !out_event) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(src != NULL);
  assert(out_event != NULL);

  *out_event = (SDL_Event) {0};
  out_event->type = src->type;

  if (msg_category_needs_user_space(src->category)) {
    u32 sdl_type = 0;
    if (!msg_user_space_get_sdl_type(src->category, src->type, &sdl_type)) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return 0;
    }
    out_event->user.type = sdl_type;
    out_event->user.timestamp = (Uint64)src->timestamp;
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  if (src->type >= SDL_EVENT_USER) {
    out_event->user.type = (Uint32)src->type;
    out_event->user.timestamp = (Uint64)src->timestamp;
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  switch (src->type) {
    case SDL_EVENT_DISPLAY_ORIENTATION:
    case SDL_EVENT_DISPLAY_ADDED:
    case SDL_EVENT_DISPLAY_REMOVED:
    case SDL_EVENT_DISPLAY_MOVED:
    case SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED:
    case SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED:
    case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED:
      out_event->display.type = (SDL_EventType)src->type;
      out_event->display.timestamp = (Uint64)src->timestamp;
      out_event->display.displayID = (SDL_DisplayID)monitor_to_native_id(msg_core_get_monitor(src)->monitor);
      out_event->display.data1 = (Sint32)msg_core_get_monitor(src)->data1;
      out_event->display.data2 = (Sint32)msg_core_get_monitor(src)->data2;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_WINDOW_SHOWN:
    case SDL_EVENT_WINDOW_HIDDEN:
    case SDL_EVENT_WINDOW_EXPOSED:
    case SDL_EVENT_WINDOW_MOVED:
    case SDL_EVENT_WINDOW_RESIZED:
    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
    case SDL_EVENT_WINDOW_METAL_VIEW_RESIZED:
    case SDL_EVENT_WINDOW_MINIMIZED:
    case SDL_EVENT_WINDOW_MAXIMIZED:
    case SDL_EVENT_WINDOW_RESTORED:
    case SDL_EVENT_WINDOW_MOUSE_ENTER:
    case SDL_EVENT_WINDOW_MOUSE_LEAVE:
    case SDL_EVENT_WINDOW_FOCUS_GAINED:
    case SDL_EVENT_WINDOW_FOCUS_LOST:
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
    case SDL_EVENT_WINDOW_HIT_TEST:
    case SDL_EVENT_WINDOW_ICCPROF_CHANGED:
    case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
    case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
    case SDL_EVENT_WINDOW_SAFE_AREA_CHANGED:
    case SDL_EVENT_WINDOW_OCCLUDED:
    case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
    case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
    case SDL_EVENT_WINDOW_DESTROYED:
    case SDL_EVENT_WINDOW_HDR_STATE_CHANGED:
      out_event->window.type = (SDL_EventType)src->type;
      out_event->window.timestamp = (Uint64)src->timestamp;
      out_event->window.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_window(src)->window);
      out_event->window.data1 = (Sint32)msg_core_get_window(src)->data1;
      out_event->window.data2 = (Sint32)msg_core_get_window(src)->data2;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_KEYBOARD_ADDED:
    case SDL_EVENT_KEYBOARD_REMOVED:
      out_event->kdevice.type = (SDL_EventType)src->type;
      out_event->kdevice.timestamp = (Uint64)src->timestamp;
      out_event->kdevice.which = (SDL_KeyboardID)msg_core_get_keyboard_device(src)->device.instance;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
      out_event->key.type = (SDL_EventType)src->type;
      out_event->key.timestamp = (Uint64)src->timestamp;
      out_event->key.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_keyboard(src)->window);
      out_event->key.which = (SDL_KeyboardID)msg_core_get_keyboard(src)->device.instance;
      out_event->key.scancode = (SDL_Scancode)msg_core_get_keyboard(src)->scancode;
      out_event->key.key = (SDL_Keycode)msg_core_get_keyboard(src)->keycode;
      out_event->key.mod = (SDL_Keymod)msg_core_get_keyboard(src)->modifiers;
      out_event->key.raw = (Uint16)msg_core_get_keyboard(src)->raw;
      out_event->key.down = msg_core_get_keyboard(src)->down != 0;
      out_event->key.repeat = msg_core_get_keyboard(src)->repeat != 0;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_TEXT_EDITING:
      out_event->edit.type = (SDL_EventType)src->type;
      out_event->edit.timestamp = (Uint64)src->timestamp;
      out_event->edit.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_text_editing(src)->window);
      out_event->edit.text = msg_core_get_text_editing(src)->text;
      out_event->edit.start = (Sint32)msg_core_get_text_editing(src)->start;
      out_event->edit.length = (Sint32)msg_core_get_text_editing(src)->length;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_TEXT_EDITING_CANDIDATES:
      out_event->edit_candidates.type = (SDL_EventType)src->type;
      out_event->edit_candidates.timestamp = (Uint64)src->timestamp;
      out_event->edit_candidates.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_text_editing_candidates(src)->window);
      out_event->edit_candidates.candidates = (const char* const*)msg_core_get_text_editing_candidates(src)->candidates;
      out_event->edit_candidates.num_candidates = (Sint32)msg_core_get_text_editing_candidates(src)->num_candidates;
      out_event->edit_candidates.selected_candidate = (Sint32)msg_core_get_text_editing_candidates(src)->selected_candidate;
      out_event->edit_candidates.horizontal = msg_core_get_text_editing_candidates(src)->horizontal != 0;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_TEXT_INPUT:
      out_event->text.type = (SDL_EventType)src->type;
      out_event->text.timestamp = (Uint64)src->timestamp;
      out_event->text.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_text_input(src)->window);
      out_event->text.text = msg_core_get_text_input(src)->text;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    default:
      break;
  }

  switch (src->type) {
    case SDL_EVENT_MOUSE_ADDED:
    case SDL_EVENT_MOUSE_REMOVED:
      out_event->mdevice.type = (SDL_EventType)src->type;
      out_event->mdevice.timestamp = (Uint64)src->timestamp;
      out_event->mdevice.which = (SDL_MouseID)msg_core_get_mouse_device(src)->device.instance;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_MOUSE_MOTION:
      out_event->motion.type = (SDL_EventType)src->type;
      out_event->motion.timestamp = (Uint64)src->timestamp;
      out_event->motion.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_mouse_motion(src)->window);
      out_event->motion.which = (SDL_MouseID)msg_core_get_mouse_motion(src)->device.instance;
      out_event->motion.state = (SDL_MouseButtonFlags)msg_core_get_mouse_motion(src)->button_mask;
      out_event->motion.x = msg_core_get_mouse_motion(src)->x;
      out_event->motion.y = msg_core_get_mouse_motion(src)->y;
      out_event->motion.xrel = msg_core_get_mouse_motion(src)->xrel;
      out_event->motion.yrel = msg_core_get_mouse_motion(src)->yrel;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
      out_event->button.type = (SDL_EventType)src->type;
      out_event->button.timestamp = (Uint64)src->timestamp;
      out_event->button.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_mouse_button(src)->window);
      out_event->button.which = (SDL_MouseID)msg_core_get_mouse_button(src)->device.instance;
      out_event->button.button = (Uint8)msg_core_get_mouse_button(src)->button;
      out_event->button.down = msg_core_get_mouse_button(src)->down != 0;
      out_event->button.clicks = (Uint8)msg_core_get_mouse_button(src)->clicks;
      out_event->button.x = msg_core_get_mouse_button(src)->x;
      out_event->button.y = msg_core_get_mouse_button(src)->y;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_MOUSE_WHEEL:
      out_event->wheel.type = (SDL_EventType)src->type;
      out_event->wheel.timestamp = (Uint64)src->timestamp;
      out_event->wheel.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_mouse_wheel(src)->window);
      out_event->wheel.which = (SDL_MouseID)msg_core_get_mouse_wheel(src)->device.instance;
      out_event->wheel.x = msg_core_get_mouse_wheel(src)->x;
      out_event->wheel.y = msg_core_get_mouse_wheel(src)->y;
      out_event->wheel.direction = (SDL_MouseWheelDirection)msg_core_get_mouse_wheel(src)->direction;
      out_event->wheel.mouse_x = msg_core_get_mouse_wheel(src)->mouse_x;
      out_event->wheel.mouse_y = msg_core_get_mouse_wheel(src)->mouse_y;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_JOYSTICK_ADDED:
    case SDL_EVENT_JOYSTICK_REMOVED:
    case SDL_EVENT_JOYSTICK_UPDATE_COMPLETE:
      out_event->jdevice.type = (SDL_EventType)src->type;
      out_event->jdevice.timestamp = (Uint64)src->timestamp;
      out_event->jdevice.which = (SDL_JoystickID)msg_core_get_joystick_device(src)->device.instance;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_JOYSTICK_AXIS_MOTION:
      out_event->jaxis.type = (SDL_EventType)src->type;
      out_event->jaxis.timestamp = (Uint64)src->timestamp;
      out_event->jaxis.which = (SDL_JoystickID)msg_core_get_joystick_axis(src)->device.instance;
      out_event->jaxis.axis = (Uint8)msg_core_get_joystick_axis(src)->axis;
      out_event->jaxis.value = (Sint16)msg_core_get_joystick_axis(src)->value;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_JOYSTICK_BALL_MOTION:
      out_event->jball.type = (SDL_EventType)src->type;
      out_event->jball.timestamp = (Uint64)src->timestamp;
      out_event->jball.which = (SDL_JoystickID)msg_core_get_joystick_ball(src)->device.instance;
      out_event->jball.ball = (Uint8)msg_core_get_joystick_ball(src)->ball;
      out_event->jball.xrel = (Sint16)msg_core_get_joystick_ball(src)->xrel;
      out_event->jball.yrel = (Sint16)msg_core_get_joystick_ball(src)->yrel;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;
  }

  switch (src->type) {
    case SDL_EVENT_JOYSTICK_HAT_MOTION:
      out_event->jhat.type = (SDL_EventType)src->type;
      out_event->jhat.timestamp = (Uint64)src->timestamp;
      out_event->jhat.which = (SDL_JoystickID)msg_core_get_joystick_hat(src)->device.instance;
      out_event->jhat.hat = (Uint8)msg_core_get_joystick_hat(src)->hat;
      out_event->jhat.value = (Uint8)msg_core_get_joystick_hat(src)->value;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
    case SDL_EVENT_JOYSTICK_BUTTON_UP:
      out_event->jbutton.type = (SDL_EventType)src->type;
      out_event->jbutton.timestamp = (Uint64)src->timestamp;
      out_event->jbutton.which = (SDL_JoystickID)msg_core_get_joystick_button(src)->device.instance;
      out_event->jbutton.button = (Uint8)msg_core_get_joystick_button(src)->button;
      out_event->jbutton.down = msg_core_get_joystick_button(src)->down != 0;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_JOYSTICK_BATTERY_UPDATED:
      out_event->jbattery.type = (SDL_EventType)src->type;
      out_event->jbattery.timestamp = (Uint64)src->timestamp;
      out_event->jbattery.which = (SDL_JoystickID)msg_core_get_joystick_battery(src)->device.instance;
      out_event->jbattery.state = (SDL_PowerState)msg_core_get_joystick_battery(src)->state;
      out_event->jbattery.percent = (int)msg_core_get_joystick_battery(src)->percent;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_GAMEPAD_ADDED:
    case SDL_EVENT_GAMEPAD_REMOVED:
    case SDL_EVENT_GAMEPAD_REMAPPED:
    case SDL_EVENT_GAMEPAD_UPDATE_COMPLETE:
    case SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED:
      out_event->gdevice.type = (SDL_EventType)src->type;
      out_event->gdevice.timestamp = (Uint64)src->timestamp;
      out_event->gdevice.which = (SDL_JoystickID)msg_core_get_gamepad_device(src)->device.instance;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_GAMEPAD_AXIS_MOTION:
      out_event->gaxis.type = (SDL_EventType)src->type;
      out_event->gaxis.timestamp = (Uint64)src->timestamp;
      out_event->gaxis.which = (SDL_JoystickID)msg_core_get_gamepad_axis(src)->device.instance;
      out_event->gaxis.axis = (Uint8)msg_core_get_gamepad_axis(src)->axis;
      out_event->gaxis.value = (Sint16)msg_core_get_gamepad_axis(src)->value;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
    case SDL_EVENT_GAMEPAD_BUTTON_UP:
      out_event->gbutton.type = (SDL_EventType)src->type;
      out_event->gbutton.timestamp = (Uint64)src->timestamp;
      out_event->gbutton.which = (SDL_JoystickID)msg_core_get_gamepad_button(src)->device.instance;
      out_event->gbutton.button = (Uint8)msg_core_get_gamepad_button(src)->button;
      out_event->gbutton.down = msg_core_get_gamepad_button(src)->down != 0;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
    case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
    case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
      out_event->gtouchpad.type = (SDL_EventType)src->type;
      out_event->gtouchpad.timestamp = (Uint64)src->timestamp;
      out_event->gtouchpad.which = (SDL_JoystickID)msg_core_get_gamepad_touchpad(src)->device.instance;
      out_event->gtouchpad.touchpad = (Sint32)msg_core_get_gamepad_touchpad(src)->touchpad;
      out_event->gtouchpad.finger = (Sint32)msg_core_get_gamepad_touchpad(src)->finger;
      out_event->gtouchpad.x = msg_core_get_gamepad_touchpad(src)->x;
      out_event->gtouchpad.y = msg_core_get_gamepad_touchpad(src)->y;
      out_event->gtouchpad.pressure = msg_core_get_gamepad_touchpad(src)->pressure;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:
      out_event->gsensor.type = (SDL_EventType)src->type;
      out_event->gsensor.timestamp = (Uint64)src->timestamp;
      out_event->gsensor.which = (SDL_JoystickID)msg_core_get_gamepad_sensor(src)->device.instance;
      out_event->gsensor.sensor = (Sint32)msg_core_get_gamepad_sensor(src)->sensor;
      out_event->gsensor.data[0] = msg_core_get_gamepad_sensor(src)->data[0];
      out_event->gsensor.data[1] = msg_core_get_gamepad_sensor(src)->data[1];
      out_event->gsensor.data[2] = msg_core_get_gamepad_sensor(src)->data[2];
      out_event->gsensor.sensor_timestamp = (Uint64)msg_core_get_gamepad_sensor(src)->sensor_timestamp;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    default:
      break;
  }

  switch (src->type) {
    case SDL_EVENT_AUDIO_DEVICE_ADDED:
    case SDL_EVENT_AUDIO_DEVICE_REMOVED:
    case SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED:
      out_event->adevice.type = (SDL_EventType)src->type;
      out_event->adevice.timestamp = (Uint64)src->timestamp;
      out_event->adevice.which = (SDL_AudioDeviceID)devices_get_audio_native_id(msg_core_get_audio_device(src)->device);
      out_event->adevice.recording =
          devices_get_audio_device_type(msg_core_get_audio_device(src)->device) == AUDIO_DEVICE_TYPE_RECORDING;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_CAMERA_DEVICE_ADDED:
    case SDL_EVENT_CAMERA_DEVICE_REMOVED:
    case SDL_EVENT_CAMERA_DEVICE_APPROVED:
    case SDL_EVENT_CAMERA_DEVICE_DENIED:
      out_event->cdevice.type = (SDL_EventType)src->type;
      out_event->cdevice.timestamp = (Uint64)src->timestamp;
      out_event->cdevice.which = (SDL_CameraID)camera_to_native_id(msg_core_get_camera_device(src)->camera);
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_RENDER_TARGETS_RESET:
    case SDL_EVENT_RENDER_DEVICE_RESET:
    case SDL_EVENT_RENDER_DEVICE_LOST:
      out_event->render.type = (SDL_EventType)src->type;
      out_event->render.timestamp = (Uint64)src->timestamp;
      out_event->render.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_render(src)->window);
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_FINGER_DOWN:
    case SDL_EVENT_FINGER_UP:
    case SDL_EVENT_FINGER_MOTION:
    case SDL_EVENT_FINGER_CANCELED:
      out_event->tfinger.type = (SDL_EventType)src->type;
      out_event->tfinger.timestamp = (Uint64)src->timestamp;
      out_event->tfinger.touchID = (SDL_TouchID)msg_core_get_touch(src)->device.instance;
      out_event->tfinger.fingerID = (SDL_FingerID)msg_core_get_touch(src)->finger_id;
      out_event->tfinger.x = msg_core_get_touch(src)->x;
      out_event->tfinger.y = msg_core_get_touch(src)->y;
      out_event->tfinger.dx = msg_core_get_touch(src)->dx;
      out_event->tfinger.dy = msg_core_get_touch(src)->dy;
      out_event->tfinger.pressure = msg_core_get_touch(src)->pressure;
      out_event->tfinger.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_touch(src)->window);
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_PEN_PROXIMITY_IN:
    case SDL_EVENT_PEN_PROXIMITY_OUT:
      out_event->pproximity.type = (SDL_EventType)src->type;
      out_event->pproximity.timestamp = (Uint64)src->timestamp;
      out_event->pproximity.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_pen_proximity(src)->window);
      out_event->pproximity.which = (SDL_PenID)msg_core_get_pen_proximity(src)->pen_id;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_PEN_MOTION:
      out_event->pmotion.type = (SDL_EventType)src->type;
      out_event->pmotion.timestamp = (Uint64)src->timestamp;
      out_event->pmotion.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_pen_motion(src)->window);
      out_event->pmotion.which = (SDL_PenID)msg_core_get_pen_motion(src)->pen_id;
      out_event->pmotion.pen_state = (SDL_PenInputFlags)msg_core_get_pen_motion(src)->pen_state;
      out_event->pmotion.x = msg_core_get_pen_motion(src)->x;
      out_event->pmotion.y = msg_core_get_pen_motion(src)->y;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_PEN_DOWN:
    case SDL_EVENT_PEN_UP:
      out_event->ptouch.type = (SDL_EventType)src->type;
      out_event->ptouch.timestamp = (Uint64)src->timestamp;
      out_event->ptouch.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_pen_touch(src)->window);
      out_event->ptouch.which = (SDL_PenID)msg_core_get_pen_touch(src)->pen_id;
      out_event->ptouch.pen_state = (SDL_PenInputFlags)msg_core_get_pen_touch(src)->pen_state;
      out_event->ptouch.x = msg_core_get_pen_touch(src)->x;
      out_event->ptouch.y = msg_core_get_pen_touch(src)->y;
      out_event->ptouch.eraser = msg_core_get_pen_touch(src)->eraser != 0;
      out_event->ptouch.down = msg_core_get_pen_touch(src)->down != 0;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_PEN_BUTTON_DOWN:
    case SDL_EVENT_PEN_BUTTON_UP:
      out_event->pbutton.type = (SDL_EventType)src->type;
      out_event->pbutton.timestamp = (Uint64)src->timestamp;
      out_event->pbutton.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_pen_button(src)->window);
      out_event->pbutton.which = (SDL_PenID)msg_core_get_pen_button(src)->pen_id;
      out_event->pbutton.pen_state = (SDL_PenInputFlags)msg_core_get_pen_button(src)->pen_state;
      out_event->pbutton.x = msg_core_get_pen_button(src)->x;
      out_event->pbutton.y = msg_core_get_pen_button(src)->y;
      out_event->pbutton.button = (Uint8)msg_core_get_pen_button(src)->button;
      out_event->pbutton.down = msg_core_get_pen_button(src)->down != 0;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_PEN_AXIS:
      out_event->paxis.type = (SDL_EventType)src->type;
      out_event->paxis.timestamp = (Uint64)src->timestamp;
      out_event->paxis.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_pen_axis(src)->window);
      out_event->paxis.which = (SDL_PenID)msg_core_get_pen_axis(src)->pen_id;
      out_event->paxis.pen_state = (SDL_PenInputFlags)msg_core_get_pen_axis(src)->pen_state;
      out_event->paxis.x = msg_core_get_pen_axis(src)->x;
      out_event->paxis.y = msg_core_get_pen_axis(src)->y;
      out_event->paxis.axis = (SDL_PenAxis)msg_core_get_pen_axis(src)->axis;
      out_event->paxis.value = msg_core_get_pen_axis(src)->value;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    default:
      break;
  }

  switch (src->type) {
    case SDL_EVENT_DROP_FILE:
    case SDL_EVENT_DROP_TEXT:
    case SDL_EVENT_DROP_BEGIN:
    case SDL_EVENT_DROP_COMPLETE:
    case SDL_EVENT_DROP_POSITION:
      out_event->drop.type = (SDL_EventType)src->type;
      out_event->drop.timestamp = (Uint64)src->timestamp;
      out_event->drop.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_drop(src)->window);
      out_event->drop.x = msg_core_get_drop(src)->x;
      out_event->drop.y = msg_core_get_drop(src)->y;
      out_event->drop.source = msg_core_get_drop(src)->source;
      out_event->drop.data = msg_core_get_drop(src)->data;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_CLIPBOARD_UPDATE:
      out_event->clipboard.type = (SDL_EventType)src->type;
      out_event->clipboard.timestamp = (Uint64)src->timestamp;
      out_event->clipboard.owner = msg_core_get_clipboard(src)->owner != 0;
      out_event->clipboard.num_mime_types = (Sint32)msg_core_get_clipboard(src)->num_mime_types;
      out_event->clipboard.mime_types = (const char**)msg_core_get_clipboard(src)->mime_types;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    case SDL_EVENT_SENSOR_UPDATE:
      out_event->sensor.type = (SDL_EventType)src->type;
      out_event->sensor.timestamp = (Uint64)src->timestamp;
      out_event->sensor.which = (SDL_SensorID)sensor_to_native_id(msg_core_get_sensor(src)->sensor);
      for (sz idx = 0; idx < count_of(msg_core_get_sensor(src)->data); idx += 1) {
        out_event->sensor.data[idx] = msg_core_get_sensor(src)->data[idx];
      }
      out_event->sensor.sensor_timestamp = (Uint64)msg_core_get_sensor(src)->sensor_timestamp;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;

    default:
      out_event->common.type = (Uint32)src->type;
      out_event->common.timestamp = (Uint64)src->timestamp;
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;
  }
}

func void msg_pump(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  // Pumps the process-global queue.
  SDL_PumpEvents();
  msg_refresh_synthetic_device_msgs();
  thread_log_trace("msg_pump");
  TracyCZoneEnd(__tracy_zone_ctx);
}

func b32 msg_poll(msg* out_msg) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Event native_event;

  msg_refresh_synthetic_device_msgs();
  if (!out_msg) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(out_msg != NULL);

  while (SDL_PollEvent(&native_event)) {
    if (!msg_from_sdl(&native_event, out_msg)) {
      continue;
    }
    if (!msg_filter_accept(out_msg)) {
      continue;
    }
    msg_notify_internal_listeners(out_msg);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
}

func b32 msg_wait(msg* out_msg) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Event native_event;

  if (!out_msg) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(out_msg != NULL);

  msg_refresh_synthetic_device_msgs();
  while (SDL_PollEvent(&native_event)) {
    if (!msg_from_sdl(&native_event, out_msg)) {
      continue;
    }
    if (!msg_filter_accept(out_msg)) {
      continue;
    }
    msg_notify_internal_listeners(out_msg);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  for (;;) {
    if (!SDL_WaitEvent(&native_event)) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return 0;
    }
    if (!msg_from_sdl(&native_event, out_msg)) {
      continue;
    }
    if (!msg_filter_accept(out_msg)) {
      continue;
    }
    msg_notify_internal_listeners(out_msg);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }
}

func b32 msg_wait_timeout(msg* out_msg, i32 timeout_ms) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Event native_event;

  if (!out_msg) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  if (timeout_ms < 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(out_msg != NULL);

  msg_refresh_synthetic_device_msgs();
  while (SDL_PollEvent(&native_event)) {
    if (!msg_from_sdl(&native_event, out_msg)) {
      continue;
    }
    if (!msg_filter_accept(out_msg)) {
      continue;
    }
    msg_notify_internal_listeners(out_msg);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  if (!SDL_WaitEventTimeout(&native_event, (Sint32)timeout_ms) || !msg_from_sdl(&native_event, out_msg) ||
      !msg_filter_accept(out_msg)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  msg_notify_internal_listeners(out_msg);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 msg_peek(msg* out_msg) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Event native_event = {0};
  if (out_msg == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (SDL_PeepEvents(&native_event, 1, SDL_PEEKEVENT, SDL_EVENT_FIRST, SDL_EVENT_LAST) <= 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (!msg_from_sdl(&native_event, out_msg) || !msg_filter_accept(out_msg)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func i32 msg_count(u32 type_min, u32 type_max) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (type_min > type_max) {
    u32 temp = type_min;
    type_min = type_max;
    type_max = temp;
  }

  i32 count = SDL_PeepEvents(NULL, 0, SDL_PEEKEVENT, type_min, type_max);
  TracyCZoneEnd(__tracy_zone_ctx);
  return count;
}

func void msg_flush(u32 type_min, u32 type_max) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (type_min > type_max) {
    u32 temp = type_min;
    type_min = type_max;
    type_max = temp;
  }
  SDL_FlushEvents(type_min, type_max);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func b32 _msg_post(const msg* src, callsite site) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Event native_event;
  msg posted_msg;
  msg* payload = NULL;

  if (!src) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(src != NULL);

  posted_msg = *src;
  posted_msg.post_site = site;
  if (!msg_category_is_valid(posted_msg.category)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (!msg_filter_accept(&posted_msg)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (!msg_dispatch_handlers(&posted_msg, MSG_HANDLER_STAGE_BEFORE_POST)) {
    thread_log_trace("msg_post: cancelled type=%u", posted_msg.type);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (msg_category_needs_user_space(posted_msg.category) ||
      (posted_msg.category == MSG_CATEGORY_CORE && posted_msg.type >= MSG_CORE_TYPE_USER)) {
    u32 native_event_type = posted_msg.type;
    if (msg_category_needs_user_space(posted_msg.category)) {
      if (!msg_user_space_get_sdl_type(posted_msg.category, posted_msg.type, &native_event_type)) {
        TracyCZoneEnd(__tracy_zone_ctx);
        return 0;
      }
    }

    payload = (msg*)SDL_malloc(size_of(*payload));
    if (!payload) {
      thread_log_error("msg_post: payload alloc failed type=%u", posted_msg.type);
      TracyCZoneEnd(__tracy_zone_ctx);
      return 0;
    }
    *payload = posted_msg;

    native_event = (SDL_Event) {0};
    native_event.user.type = native_event_type;
    native_event.user.timestamp = (Uint64)posted_msg.timestamp;
    native_event.user.code = (Sint32)MSG_PAYLOAD_CODE;
    native_event.user.data1 = payload;
    native_event.user.data2 = NULL;
  } else if (!msg_to_sdl_event(&posted_msg, &native_event)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  // Push into SDL's process-global queue.
  if (!SDL_PushEvent(&native_event)) {
    if (payload != NULL) {
      SDL_free(payload);
    }
    (void)msg_dispatch_handlers(&posted_msg, MSG_HANDLER_STAGE_POST_FAILED);
    thread_log_warn("msg_post: SDL_PushEvent failed type=%u", posted_msg.type);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  (void)msg_dispatch_handlers(&posted_msg, MSG_HANDLER_STAGE_AFTER_POST);
  msg_notify_internal_listeners(&posted_msg);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func u64 msg_add_handler(const msg_handler_desc* desc) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (desc == NULL || desc->handler_fn == NULL || msg_handler_count >= MSG_HANDLER_CAP) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(desc != NULL);
  assert(desc->handler_fn != NULL);

  u64 handler_id = msg_handler_next_id;
  msg_handler_next_id += 1;
  if (msg_handler_next_id == 0) {
    msg_handler_next_id = 1;
  }

  msg_handler_entry* entry = &msg_handler_entries[msg_handler_count];
  *entry = (msg_handler_entry) {
      .handler_id = handler_id,
      .handler_fn = desc->handler_fn,
      .user_data = desc->user_data,
      .priority = desc->priority,
      .options = desc->options,
      .type_min = desc->type_min,
      .type_max = desc->type_max,
  };
  msg_handler_count += 1;
  msg_handler_sort_entries();
  thread_log_trace("msg_add_handler: id=%llu count=%u", (unsigned long long)handler_id, msg_handler_count);
  TracyCZoneEnd(__tracy_zone_ctx);
  return handler_id;
}

func b32 msg_remove_handler(u64 handler_id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (handler_id == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(handler_id != 0);

  for (u32 idx = 0; idx < msg_handler_count; idx += 1) {
    if (msg_handler_entries[idx].handler_id != handler_id) {
      continue;
    }

    for (u32 move_idx = idx + 1; move_idx < msg_handler_count; move_idx += 1) {
      msg_handler_entries[move_idx - 1] = msg_handler_entries[move_idx];
    }

    msg_handler_count -= 1;
    msg_handler_entries[msg_handler_count] = (msg_handler_entry) {0};
    thread_log_trace("msg_remove_handler: id=%llu count=%u", (unsigned long long)handler_id, msg_handler_count);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
}

func void msg_clear_handlers(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  for (u32 idx = 0; idx < msg_handler_count; idx += 1) {
    msg_handler_entries[idx] = (msg_handler_entry) {0};
  }

  msg_handler_count = 0;
  msg_handler_next_id = 1;
  thread_log_trace("msg_clear_handlers");
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void msg_set_filter(msg_filter_fn filter_fn, void* user_data) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  msg_filter_current = filter_fn;
  msg_filter_user_data = user_data;
  TracyCZoneEnd(__tracy_zone_ctx);
}

func b32 msg_from_native(const void* native_event, msg* out_msg) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (native_event == NULL || out_msg == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return msg_from_sdl((const SDL_Event*)native_event, out_msg);
}

func b32 msg_to_native(const msg* src, void* native_event) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (src == NULL || native_event == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return msg_to_sdl_event((msg*)src, (SDL_Event*)native_event);
}




