// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/msg.h"
#include "input/msg_core.h"
#include "../internal.h"
#include "../sdl3_include.h"
#include "basic/assert.h"
#include "basic/utility_defines.h"
#include "context/thread_ctx.h"
#include "basic/profiler.h"
#include <SDL3/SDL_hidapi.h>
#include "basic/safe.h"

func mouse_button mouse_button_from_sdl(Uint8 button) {
  switch (button) {
    case SDL_BUTTON_LEFT:
      return MOUSE_BUTTON_LEFT;
    case SDL_BUTTON_MIDDLE:
      return MOUSE_BUTTON_MIDDLE;
    case SDL_BUTTON_RIGHT:
      return MOUSE_BUTTON_RIGHT;
    case SDL_BUTTON_X1:
      return MOUSE_BUTTON_X1;
    case SDL_BUTTON_X2:
      return MOUSE_BUTTON_X2;
    default:
      return MOUSE_BUTTON_COUNT;
  }
}

func Uint8 mouse_button_to_sdl(mouse_button button) {
  switch (button) {
    case MOUSE_BUTTON_LEFT:
      return SDL_BUTTON_LEFT;
    case MOUSE_BUTTON_MIDDLE:
      return SDL_BUTTON_MIDDLE;
    case MOUSE_BUTTON_RIGHT:
      return SDL_BUTTON_RIGHT;
    case MOUSE_BUTTON_X1:
      return SDL_BUTTON_X1;
    case MOUSE_BUTTON_X2:
      return SDL_BUTTON_X2;
    default:
      return 0;
  }
}

func b32 msg_handle_runtime_internal(msg* src, void* user_data) {
  profile_func_begin;
  (void)user_data;

  if (src->type == MSG_CORE_TYPE_TOUCH_ADDED || src->type == MSG_CORE_TYPE_TOUCH_REMOVED) {
    device dev_id = msg_core_get_touch_device(src)->device;
    devices_update_runtime(dev_id, src->type == MSG_CORE_TYPE_TOUCH_ADDED, (void*)(up)devices_get_instance(dev_id));
  } else if (src->type >= MSG_CORE_TYPE_FINGER_DOWN && src->type <= MSG_CORE_TYPE_FINGER_CANCELED) {
    devices_set_focus(msg_core_get_touch(src)->device);
    devices_update_runtime(msg_core_get_touch(src)->device, 1, (void*)(up)devices_get_instance(msg_core_get_touch(src)->device));
  } else if (src->type >= MSG_CORE_TYPE_AUDIO_DEVICE_ADDED && src->type <= MSG_CORE_TYPE_AUDIO_DEVICE_FORMAT_CHANGED) {
    device dev_id = audio_device_to_device(msg_core_get_audio_device(src)->audio);
    devices_update_runtime(dev_id, src->type != MSG_CORE_TYPE_AUDIO_DEVICE_REMOVED, (void*)(up)devices_get_instance(dev_id));
    devices_set_focus(dev_id);
  } else if (src->type >= MSG_CORE_TYPE_CAMERA_DEVICE_ADDED && src->type <= MSG_CORE_TYPE_CAMERA_DEVICE_DENIED) {
    device dev_id = camera_to_device(msg_core_get_camera_device(src)->camera);
    devices_update_runtime(dev_id, src->type != MSG_CORE_TYPE_CAMERA_DEVICE_REMOVED, (void*)(up)devices_get_instance(dev_id));
    devices_set_focus(dev_id);
  } else if (src->type == MSG_CORE_TYPE_SENSOR_UPDATE) {
    device dev_id = sensor_to_device(msg_core_get_sensor(src)->sensor);
    devices_update_runtime(dev_id, 1, (void*)(up)devices_get_instance(dev_id));
    devices_set_focus(dev_id);
  }
  profile_func_end;
  return true;
}

#define MSG_DEVICE_TRACK_CAP ((sz)128)

typedef struct msg_handler_entry {
  u64 handler_id;
  msg_handler_fn handler_fn;
  void* user_data;
  i32 priority;
  msg_category category;
  u32 type;
} msg_handler_entry;

global_var msg_handler_entry msg_handler_entries[MSG_HANDLER_CAP] = {0};
global_var u32 msg_handler_count = 0;
global_var u64 msg_handler_next_id = 1;
global_var msg_filter_fn msg_filter_current = NULL;
global_var void* msg_filter_user_data = NULL;

func b32 msg_remove_handler(u64 handler_id);
func b32 msg_from_sdl(const SDL_Event* src, msg* out_msg);

func b32 msg_category_is_valid(msg_category category) {
  return category >= MSG_CATEGORY_CORE && category < MSG_CATEGORY_MAX;
}

func b32 msg_filter_accept(const msg* src) {
  profile_func_begin;
  if (msg_filter_current == NULL || src == NULL) {
    profile_func_end;
    return true;
  }
  b32 result = msg_filter_current(src, msg_filter_user_data);
  profile_func_end;
  return result;
}

func b32 msg_device_equal(device lhs, device rhs) {
  return lhs == rhs;
}

func b32 msg_device_list_contains(const device* list, sz count, device src) {
  profile_func_begin;
  if (!list) {
    profile_func_end;
    return false;
  }

  safe_for (sz item_idx = 0; item_idx < count; item_idx += 1) {
    if (msg_device_equal(list[item_idx], src)) {
      profile_func_end;
      return true;
    }
  }

  profile_func_end;
  return false;
}

func sz msg_collect_touch_devices(device* out_ids, sz cap) {
  profile_func_begin;
  int count = 0;
  SDL_TouchID* ids = SDL_GetTouchDevices(&count);
  sz out_count = 0;

  safe_for (int item_idx = 0; ids && item_idx < count && out_count < cap; item_idx += 1) {
    if (ids[item_idx] == 0) {
      continue;
    }

    out_ids[out_count] = devices_make_id(DEVICE_TYPE_TOUCH, (u64)ids[item_idx]);
    out_count += 1;
  }

  if (ids) {
    SDL_free(ids);
  }

  profile_func_end;
  return out_count;
}

func sz msg_collect_tablet_devices(device* out_ids, sz cap) {
  profile_func_begin;
  SDL_hid_device_info* head = SDL_hid_enumerate(0, 0);
  SDL_hid_device_info* entry = head;
  sz out_count = 0;

  safe_while (entry&& out_count < cap) {
    if (entry->usage_page == 0x0D && entry->path != NULL) {
      out_ids[out_count] = devices_make_id(DEVICE_TYPE_TABLET, cstr8_hash64(entry->path));
      out_count += 1;
    }

    entry = entry->next;
  }

  if (head) {
    SDL_hid_free_enumeration(head);
  }

  profile_func_end;
  return out_count;
}

func void msg_post_touch_device_event(u32 type, device dev_id) {
  profile_func_begin;
  msg device_msg = {0};
  device_msg.type = type;
  msg_core_touch_device_data core_data = {.device = dev_id};
  msg_core_fill_touch_device(&device_msg, &core_data);
  (void)_msg_post(&device_msg, CALLSITE_HERE);
  profile_func_end;
}

func void msg_post_tablet_device_event(u32 type, device dev_id) {
  profile_func_begin;
  msg device_msg = {0};
  device_msg.type = type;
  msg_core_tablet_device_data core_data = {.device = dev_id};
  msg_core_fill_tablet_device(&device_msg, &core_data);
  (void)_msg_post(&device_msg, CALLSITE_HERE);
  profile_func_end;
}

func void msg_refresh_touch_devices(void) {
  profile_func_begin;
  local_persist device previous_ids[MSG_DEVICE_TRACK_CAP] = {0};
  local_persist sz previous_count = 0;
  device current_ids[MSG_DEVICE_TRACK_CAP] = {0};
  sz current_count = msg_collect_touch_devices(current_ids, count_of(current_ids));

  safe_for (sz item_idx = 0; item_idx < current_count; item_idx += 1) {
    if (!msg_device_list_contains(previous_ids, previous_count, current_ids[item_idx])) {
      msg_post_touch_device_event(MSG_CORE_TYPE_TOUCH_ADDED, current_ids[item_idx]);
    }
  }

  safe_for (sz item_idx = 0; item_idx < previous_count; item_idx += 1) {
    if (!msg_device_list_contains(current_ids, current_count, previous_ids[item_idx])) {
      msg_post_touch_device_event(MSG_CORE_TYPE_TOUCH_REMOVED, previous_ids[item_idx]);
    }
  }

  previous_count = current_count;
  safe_for (sz item_idx = 0; item_idx < previous_count; item_idx += 1) {
    previous_ids[item_idx] = current_ids[item_idx];
  }
  profile_func_end;
}

func void msg_refresh_tablet_devices(void) {
  profile_func_begin;
  local_persist device previous_ids[MSG_DEVICE_TRACK_CAP] = {0};
  local_persist sz previous_count = 0;
  device current_ids[MSG_DEVICE_TRACK_CAP] = {0};
  sz current_count = msg_collect_tablet_devices(current_ids, count_of(current_ids));

  safe_for (sz item_idx = 0; item_idx < current_count; item_idx += 1) {
    if (!msg_device_list_contains(previous_ids, previous_count, current_ids[item_idx])) {
      msg_post_tablet_device_event(MSG_CORE_TYPE_TABLET_ADDED, current_ids[item_idx]);
    }
  }

  safe_for (sz item_idx = 0; item_idx < previous_count; item_idx += 1) {
    if (!msg_device_list_contains(current_ids, current_count, previous_ids[item_idx])) {
      msg_post_tablet_device_event(MSG_CORE_TYPE_TABLET_REMOVED, previous_ids[item_idx]);
    }
  }

  previous_count = current_count;
  safe_for (sz item_idx = 0; item_idx < previous_count; item_idx += 1) {
    previous_ids[item_idx] = current_ids[item_idx];
  }
  profile_func_end;
}

func void msg_refresh_synthetic_device_msgs(void) {
  profile_func_begin;
  msg_refresh_touch_devices();
  msg_refresh_tablet_devices();
  profile_func_end;
}

// NOTE:
// SDL uses one process-global event queue for native events. This module only
// uses that queue as an input source; based messages themselves dispatch
// immediately through msg_post on the calling thread.

func b32 msg_handler_should_run_for_msg(const msg_handler_entry* entry, const msg* posted_msg) {
  if (entry == NULL || posted_msg == NULL) {
    return false;
  }

  if (entry->category != MSG_CATEGORY_MAX && entry->category != posted_msg->category) {
    return false;
  }

  if (entry->type != 0 && entry->type != posted_msg->type) {
    return false;
  }

  return true;
}

func void msg_handler_sort_entries(void) {
  profile_func_begin;
  safe_for (u32 outer_idx = 0; outer_idx < msg_handler_count; outer_idx += 1) {
    safe_for (u32 inner_idx = outer_idx + 1; inner_idx < msg_handler_count; inner_idx += 1) {
      msg_handler_entry* lhs = &msg_handler_entries[outer_idx];
      msg_handler_entry* rhs = &msg_handler_entries[inner_idx];
      b32 swap_needed = false;

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
  profile_func_end;
}

func b32 msg_dispatch_handlers(msg* posted_msg) {
  profile_func_begin;
  msg_handler_entry dispatch_entries[MSG_HANDLER_CAP] = {0};
  u32 dispatch_cap = (u32)count_of(dispatch_entries);
  u32 dispatch_count = msg_handler_count;

  if (dispatch_count > dispatch_cap) {
    dispatch_count = dispatch_cap;
  }

  safe_for (u32 idx = 0; idx < dispatch_count; idx += 1) {
    dispatch_entries[idx] = msg_handler_entries[idx];
  }

  safe_for (u32 idx = 0; idx < dispatch_count; idx += 1) {
    msg_handler_entry* entry = &dispatch_entries[idx];

    if (entry->handler_fn == NULL) {
      continue;
    }

    if (!msg_handler_should_run_for_msg(entry, posted_msg)) {
      continue;
    }

    if (!entry->handler_fn(posted_msg, entry->user_data)) {
      profile_func_end;
      return false;
    }
  }

  profile_func_end;
  return true;
}

func b32 msg_dispatch_native_event_with_site(const SDL_Event* native_event, msg* out_msg, callsite site) {
  profile_func_begin;
  msg translated_msg = {0};

  if (native_event == NULL || out_msg == NULL) {
    thread_log_error("Rejected native event dispatch native_event=%p out_msg=%p", (void*)native_event, (void*)out_msg);
    profile_func_end;
    return false;
  }

  if (!msg_from_sdl(native_event, &translated_msg)) {
    thread_log_verbose("Skipped SDL event type=%u because translation failed", (u32)native_event->type);
    profile_func_end;
    return false;
  }

  if (!_msg_post(&translated_msg, site)) {
    thread_log_trace("Dropped translated SDL event type=%u", translated_msg.type);
    profile_func_end;
    return false;
  }

  *out_msg = translated_msg;
  profile_func_end;
  return true;
}

func b32 msg_dispatch_native_event(const SDL_Event* native_event, msg* out_msg) {
  profile_func_begin;
  b32 result = msg_dispatch_native_event_with_site(native_event, out_msg, (callsite) {0});
  profile_func_end;
  return result;
}

func void msg_apply_common(msg* dst, const SDL_Event* src) {
  profile_func_begin;
  dst->type = src->type;
  dst->timestamp = src->common.timestamp;
  dst->post_site = (callsite) {0};
  dst->category = MSG_CATEGORY_CORE;
  profile_func_end;
}

func b32 msg_from_sdl(const SDL_Event* src, msg* out_msg) {
  profile_func_begin;
  if (!src || !out_msg) {
    profile_func_end;
    return false;
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
      profile_func_end;
      return true;

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
      profile_func_end;
      return true;

    case SDL_EVENT_KEYBOARD_ADDED:
    case SDL_EVENT_KEYBOARD_REMOVED:
      msg_core_fill_keyboard_device(
          out_msg,
          &(msg_core_keyboard_device_data) {
              .device = devices_make_id(DEVICE_TYPE_KEYBOARD, (u64)src->kdevice.which),
          });
      profile_func_end;
      return true;

    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
      msg_core_fill_keyboard(
          out_msg,
          &(msg_core_keyboard_data) {
              .window = window_from_native_id((up)src->key.windowID),
              .device = input_native_backend_resolve_keyboard_device(
                  src,
                  devices_make_id(DEVICE_TYPE_KEYBOARD, (u64)src->key.which)),
              .key = keyboard_internal_vkey_from_scancode((u32)src->key.scancode),
              .modifiers = (keymod)src->key.mod,
              .down = src->key.down ? true : false,
              .repeat = src->key.repeat ? true : false,
          });
      profile_func_end;
      return true;

    case SDL_EVENT_TEXT_EDITING:
      msg_core_fill_text_editing(
          out_msg,
          &(msg_core_text_editing_data) {
              .window = window_from_native_id((up)src->edit.windowID),
              .text = src->edit.text,
              .start = (i32)src->edit.start,
              .length = (i32)src->edit.length,
          });
      profile_func_end;
      return true;

    case SDL_EVENT_TEXT_EDITING_CANDIDATES:
      msg_core_fill_text_editing_candidates(
          out_msg,
          &(msg_core_text_editing_candidates_data) {
              .window = window_from_native_id((up)src->edit_candidates.windowID),
              .candidates = (cstr8 const*)src->edit_candidates.candidates,
              .num_candidates = (i32)src->edit_candidates.num_candidates,
              .selected_candidate = (i32)src->edit_candidates.selected_candidate,
              .horizontal = src->edit_candidates.horizontal ? true : false,
          });
      profile_func_end;
      return true;

    case SDL_EVENT_TEXT_INPUT:
      msg_core_fill_text_input(
          out_msg,
          &(msg_core_text_input_data) {
              .window = window_from_native_id((up)src->text.windowID),
              .text = src->text.text,
          });
      profile_func_end;
      return true;

    case SDL_EVENT_MOUSE_ADDED:
    case SDL_EVENT_MOUSE_REMOVED:
      msg_core_fill_mouse_device(
          out_msg,
          &(msg_core_mouse_device_data) {
              .device = devices_make_id(DEVICE_TYPE_MOUSE, (u64)src->mdevice.which),
          });
      profile_func_end;
      return true;

    case SDL_EVENT_MOUSE_MOTION:
      msg_core_fill_mouse_motion(
          out_msg,
          &(msg_core_mouse_motion_data) {
              .window = window_from_native_id((up)src->motion.windowID),
              .device = input_native_backend_resolve_mouse_motion_device(
                  src,
                  devices_make_id(DEVICE_TYPE_MOUSE, (u64)src->motion.which)),
              .button_mask = (u32)src->motion.state,
              .x = src->motion.x,
              .y = src->motion.y,
              .xrel = src->motion.xrel,
              .yrel = src->motion.yrel,
          });
      profile_func_end;
      return true;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
      msg_core_fill_mouse_button(
          out_msg,
          &(msg_core_mouse_button_data) {
              .window = window_from_native_id((up)src->button.windowID),
              .device = input_native_backend_resolve_mouse_button_device(
                  src,
                  devices_make_id(DEVICE_TYPE_MOUSE, (u64)src->button.which)),
              .button = mouse_button_from_sdl((Uint8)src->button.button),
              .down = src->button.down ? true : false,
              .clicks = (u8)src->button.clicks,
              .x = src->button.x,
              .y = src->button.y,
          });
      profile_func_end;
      return true;

    case SDL_EVENT_MOUSE_WHEEL:
      msg_core_fill_mouse_wheel(
          out_msg,
          &(msg_core_mouse_wheel_data) {
              .window = window_from_native_id((up)src->wheel.windowID),
              .device = input_native_backend_resolve_mouse_wheel_device(
                  src,
                  devices_make_id(DEVICE_TYPE_MOUSE, (u64)src->wheel.which)),
              .x = src->wheel.x,
              .y = src->wheel.y,
              .direction = (mouse_wheel_direction)src->wheel.direction,
              .mouse_x = src->wheel.mouse_x,
              .mouse_y = src->wheel.mouse_y,
          });
      profile_func_end;
      return true;

    case SDL_EVENT_JOYSTICK_ADDED:
    case SDL_EVENT_JOYSTICK_REMOVED:
    case SDL_EVENT_JOYSTICK_UPDATE_COMPLETE:
      msg_core_fill_joystick_device(
          out_msg,
          &(msg_core_joystick_device_data) {
              .joystick = joystick_from_native_id((up)src->jdevice.which),
          });
      profile_func_end;
      return true;

    case SDL_EVENT_JOYSTICK_AXIS_MOTION:
      msg_core_fill_joystick_axis(
          out_msg,
          &(msg_core_joystick_axis_data) {
              .joystick = joystick_from_native_id((up)src->jaxis.which),
              .axis = (u8)src->jaxis.axis,
              .value = (i16)src->jaxis.value,
          });
      profile_func_end;
      return true;

    case SDL_EVENT_JOYSTICK_BALL_MOTION:
      msg_core_fill_joystick_ball(
          out_msg,
          &(msg_core_joystick_ball_data) {
              .joystick = joystick_from_native_id((up)src->jball.which),
              .ball = (u8)src->jball.ball,
              .xrel = (i16)src->jball.xrel,
              .yrel = (i16)src->jball.yrel,
          });
      profile_func_end;
      return true;

    case SDL_EVENT_JOYSTICK_HAT_MOTION:
      msg_core_fill_joystick_hat(
          out_msg,
          &(msg_core_joystick_hat_data) {
              .joystick = joystick_from_native_id((up)src->jhat.which),
              .hat = (u8)src->jhat.hat,
              .value = (joystick_hat_state)src->jhat.value,
          });
      profile_func_end;
      return true;

    case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
    case SDL_EVENT_JOYSTICK_BUTTON_UP:
      msg_core_fill_joystick_button(
          out_msg,
          &(msg_core_joystick_button_data) {
              .joystick = joystick_from_native_id((up)src->jbutton.which),
              .button = (u8)src->jbutton.button,
              .down = src->jbutton.down ? true : false,
          });
      profile_func_end;
      return true;

    case SDL_EVENT_JOYSTICK_BATTERY_UPDATED:
      msg_core_fill_joystick_battery(
          out_msg,
          &(msg_core_joystick_battery_data) {
              .joystick = joystick_from_native_id((up)src->jbattery.which),
              .state = joystick_battery_state_from_native((i32)src->jbattery.state),
              .percent = (i32)src->jbattery.percent,
          });
      profile_func_end;
      return true;

    case SDL_EVENT_GAMEPAD_ADDED:
    case SDL_EVENT_GAMEPAD_REMOVED:
    case SDL_EVENT_GAMEPAD_REMAPPED:
    case SDL_EVENT_GAMEPAD_UPDATE_COMPLETE:
    case SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED:
      msg_core_fill_gamepad_device(
          out_msg,
          &(msg_core_gamepad_device_data) {
              .device = devices_make_id(DEVICE_TYPE_GAMEPAD, (u64)src->gdevice.which),
          });
      profile_func_end;
      return true;

    case SDL_EVENT_GAMEPAD_AXIS_MOTION:
      msg_core_fill_gamepad_axis(
          out_msg,
          &(msg_core_gamepad_axis_data) {
              .device = devices_make_id(DEVICE_TYPE_GAMEPAD, (u64)src->gaxis.which),
              .axis = (gamepad_axis)src->gaxis.axis,
              .value = (i16)src->gaxis.value,
          });
      profile_func_end;
      return true;

    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
    case SDL_EVENT_GAMEPAD_BUTTON_UP:
      msg_core_fill_gamepad_button(
          out_msg,
          &(msg_core_gamepad_button_data) {
              .device = devices_make_id(DEVICE_TYPE_GAMEPAD, (u64)src->gbutton.which),
              .button = (gamepad_button)src->gbutton.button,
              .down = src->gbutton.down ? true : false,
          });
      profile_func_end;
      return true;

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
              .device = devices_make_id(DEVICE_TYPE_GAMEPAD, (u64)src->gtouchpad.which),
              .touchpad = (gamepad_touchpad_idx)src->gtouchpad.touchpad,
              .finger = (gamepad_finger_idx)src->gtouchpad.finger,
              .x = src->gtouchpad.x,
              .y = src->gtouchpad.y,
              .pressure = src->gtouchpad.pressure,
          });
      profile_func_end;
      return true;

    case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:
      msg_core_fill_gamepad_sensor(
          out_msg,
          &(msg_core_gamepad_sensor_data) {
              .device = devices_make_id(DEVICE_TYPE_GAMEPAD, (u64)src->gsensor.which),
              .sensor = (gamepad_sensor_kind)src->gsensor.sensor,
              .data = {src->gsensor.data[0], src->gsensor.data[1], src->gsensor.data[2]},
              .sensor_timestamp = (u64)src->gsensor.sensor_timestamp,
      });
      profile_func_end;
      return true;

    case SDL_EVENT_AUDIO_DEVICE_ADDED:
    case SDL_EVENT_AUDIO_DEVICE_REMOVED:
    case SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED:
      msg_core_fill_audio_device(
          out_msg,
          &(msg_core_audio_device_data) {
              .audio = audio_device_from_native_id(
                  (up)src->adevice.which,
                  src->adevice.recording ? AUDIO_DEVICE_TYPE_RECORDING : AUDIO_DEVICE_TYPE_PLAYBACK),
          });
      profile_func_end;
      return true;

    case SDL_EVENT_CAMERA_DEVICE_ADDED:
    case SDL_EVENT_CAMERA_DEVICE_REMOVED:
    case SDL_EVENT_CAMERA_DEVICE_APPROVED:
    case SDL_EVENT_CAMERA_DEVICE_DENIED:
      msg_core_fill_camera_device(
          out_msg,
          &(msg_core_camera_device_data) {
              .camera = camera_from_native_id((up)src->cdevice.which),
          });
      profile_func_end;
      return true;

    case SDL_EVENT_RENDER_TARGETS_RESET:
    case SDL_EVENT_RENDER_DEVICE_RESET:
    case SDL_EVENT_RENDER_DEVICE_LOST:
      msg_core_fill_render(
          out_msg,
          &(msg_core_render_data) {
              .window = window_from_native_id((up)src->render.windowID),
          });
      profile_func_end;
      return true;

    case SDL_EVENT_FINGER_DOWN:
    case SDL_EVENT_FINGER_UP:
    case SDL_EVENT_FINGER_MOTION:
    case SDL_EVENT_FINGER_CANCELED:
      msg_core_fill_touch(
          out_msg,
          &(msg_core_touch_data) {
              .device = input_native_backend_resolve_touch_device(
                  src,
                  devices_make_id(DEVICE_TYPE_TOUCH, (u64)src->tfinger.touchID)),
              .finger_id = (finger_id)src->tfinger.fingerID,
              .x = src->tfinger.x,
              .y = src->tfinger.y,
              .dx = src->tfinger.dx,
              .dy = src->tfinger.dy,
              .pressure = src->tfinger.pressure,
              .window = window_from_native_id((up)src->tfinger.windowID),
          });
      profile_func_end;
      return true;

    case SDL_EVENT_PEN_PROXIMITY_IN:
    case SDL_EVENT_PEN_PROXIMITY_OUT:
      msg_core_fill_pen_proximity(
          out_msg,
          &(msg_core_pen_proximity_data) {
              .window = window_from_native_id((up)src->pproximity.windowID),
              .device = input_native_backend_resolve_pen_device(
                  src,
                  devices_make_id(DEVICE_TYPE_TABLET, (u64)src->pproximity.which)),
              .pen_id = (pen_id)src->pproximity.which,
          });
      profile_func_end;
      return true;

    case SDL_EVENT_PEN_MOTION:
      msg_core_fill_pen_motion(
          out_msg,
          &(msg_core_pen_motion_data) {
              .window = window_from_native_id((up)src->pmotion.windowID),
              .device = input_native_backend_resolve_pen_device(
                  src,
                  devices_make_id(DEVICE_TYPE_TABLET, (u64)src->pmotion.which)),
              .pen_id = (pen_id)src->pmotion.which,
              .pen_state = (tablet_input_flags)src->pmotion.pen_state,
              .x = src->pmotion.x,
              .y = src->pmotion.y,
          });
      profile_func_end;
      return true;

    case SDL_EVENT_PEN_DOWN:
    case SDL_EVENT_PEN_UP:
      msg_core_fill_pen_touch(
          out_msg,
          &(msg_core_pen_touch_data) {
              .window = window_from_native_id((up)src->ptouch.windowID),
              .device = input_native_backend_resolve_pen_device(
                  src,
                  devices_make_id(DEVICE_TYPE_TABLET, (u64)src->ptouch.which)),
              .pen_id = (pen_id)src->ptouch.which,
              .pen_state = (tablet_input_flags)src->ptouch.pen_state,
              .x = src->ptouch.x,
              .y = src->ptouch.y,
              .eraser = src->ptouch.eraser ? true : false,
              .down = src->ptouch.down ? true : false,
          });
      profile_func_end;
      return true;

    case SDL_EVENT_PEN_BUTTON_DOWN:
    case SDL_EVENT_PEN_BUTTON_UP:
      msg_core_fill_pen_button(
          out_msg,
          &(msg_core_pen_button_data) {
              .window = window_from_native_id((up)src->pbutton.windowID),
              .device = input_native_backend_resolve_pen_device(
                  src,
                  devices_make_id(DEVICE_TYPE_TABLET, (u64)src->pbutton.which)),
              .pen_id = (pen_id)src->pbutton.which,
              .pen_state = (tablet_input_flags)src->pbutton.pen_state,
              .x = src->pbutton.x,
              .y = src->pbutton.y,
              .button = (tablet_button)src->pbutton.button,
              .down = src->pbutton.down ? true : false,
          });
      profile_func_end;
      return true;

    case SDL_EVENT_PEN_AXIS:
      msg_core_fill_pen_axis(
          out_msg,
          &(msg_core_pen_axis_data) {
              .window = window_from_native_id((up)src->paxis.windowID),
              .device = input_native_backend_resolve_pen_device(
                  src,
                  devices_make_id(DEVICE_TYPE_TABLET, (u64)src->paxis.which)),
              .pen_id = (pen_id)src->paxis.which,
              .pen_state = (tablet_input_flags)src->paxis.pen_state,
              .x = src->paxis.x,
              .y = src->paxis.y,
              .axis = (tablet_axis)src->paxis.axis,
              .value = src->paxis.value,
          });
      profile_func_end;
      return true;

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
      profile_func_end;
      return true;

    case SDL_EVENT_CLIPBOARD_UPDATE:
      msg_core_fill_clipboard(
          out_msg,
          &(msg_core_clipboard_data) {
              .owner = src->clipboard.owner ? true : false,
              .num_mime_types = (i32)src->clipboard.num_mime_types,
              .mime_types = (cstr8 const*)src->clipboard.mime_types,
          });
      profile_func_end;
      return true;

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
      profile_func_end;
      return true;

    default:
      if (src->type >= SDL_EVENT_USER) {
        profile_func_end;
        return false;
      }
      profile_func_end;
      return true;
  }
}

func b32 msg_to_sdl_event(msg* src, SDL_Event* out_event) {
  profile_func_begin;
  if (!src || !out_event) {
    profile_func_end;
    return false;
  }
  assert(src != NULL);
  assert(out_event != NULL);

  *out_event = (SDL_Event) {0};
  out_event->type = src->type;

  if (src->category != MSG_CATEGORY_CORE) {
    thread_log_warn("Cannot convert non-core message category=%u type=%u to native SDL event", (u32)src->category, src->type);
    profile_func_end;
    return false;
  }

  if (src->type >= SDL_EVENT_USER) {
    thread_log_warn("Cannot convert non-SDL message type=%u category=%u to native SDL event", src->type, (u32)src->category);
    profile_func_end;
    return false;
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
      profile_func_end;
      return true;

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
      profile_func_end;
      return true;

    case SDL_EVENT_KEYBOARD_ADDED:
    case SDL_EVENT_KEYBOARD_REMOVED:
      out_event->kdevice.type = (SDL_EventType)src->type;
      out_event->kdevice.timestamp = (Uint64)src->timestamp;
      out_event->kdevice.which = (SDL_KeyboardID)devices_get_instance(msg_core_get_keyboard_device(src)->device);
      profile_func_end;
      return true;

    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:   {
      u32 scancode = keyboard_internal_scancode_from_vkey(msg_core_get_keyboard(src)->key);
      out_event->key.type = (SDL_EventType)src->type;
      out_event->key.timestamp = (Uint64)src->timestamp;
      out_event->key.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_keyboard(src)->window);
      out_event->key.which = (SDL_KeyboardID)devices_get_instance(msg_core_get_keyboard(src)->device);
      out_event->key.scancode = (SDL_Scancode)scancode;
      out_event->key.key =
          (SDL_Keycode)keyboard_internal_keycode_from_vkey(msg_core_get_keyboard(src)->key,
                                                           msg_core_get_keyboard(src)->modifiers,
                                                           msg_core_get_keyboard(src)->down);
      out_event->key.mod = (SDL_Keymod)msg_core_get_keyboard(src)->modifiers;
      out_event->key.raw = 0;
      out_event->key.down = msg_core_get_keyboard(src)->down != 0;
      out_event->key.repeat = msg_core_get_keyboard(src)->repeat != 0;
      profile_func_end;
      return true;
    }

    case SDL_EVENT_TEXT_EDITING:
      out_event->edit.type = (SDL_EventType)src->type;
      out_event->edit.timestamp = (Uint64)src->timestamp;
      out_event->edit.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_text_editing(src)->window);
      out_event->edit.text = msg_core_get_text_editing(src)->text;
      out_event->edit.start = (Sint32)msg_core_get_text_editing(src)->start;
      out_event->edit.length = (Sint32)msg_core_get_text_editing(src)->length;
      profile_func_end;
      return true;

    case SDL_EVENT_TEXT_EDITING_CANDIDATES:
      out_event->edit_candidates.type = (SDL_EventType)src->type;
      out_event->edit_candidates.timestamp = (Uint64)src->timestamp;
      out_event->edit_candidates.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_text_editing_candidates(src)->window);
      out_event->edit_candidates.candidates = (const char* const*)msg_core_get_text_editing_candidates(src)->candidates;
      out_event->edit_candidates.num_candidates = (Sint32)msg_core_get_text_editing_candidates(src)->num_candidates;
      out_event->edit_candidates.selected_candidate = (Sint32)msg_core_get_text_editing_candidates(src)->selected_candidate;
      out_event->edit_candidates.horizontal = msg_core_get_text_editing_candidates(src)->horizontal != 0;
      profile_func_end;
      return true;

    case SDL_EVENT_TEXT_INPUT:
      out_event->text.type = (SDL_EventType)src->type;
      out_event->text.timestamp = (Uint64)src->timestamp;
      out_event->text.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_text_input(src)->window);
      out_event->text.text = msg_core_get_text_input(src)->text;
      profile_func_end;
      return true;

    default:
      break;
  }

  switch (src->type) {
    case SDL_EVENT_MOUSE_ADDED:
    case SDL_EVENT_MOUSE_REMOVED:
      out_event->mdevice.type = (SDL_EventType)src->type;
      out_event->mdevice.timestamp = (Uint64)src->timestamp;
      out_event->mdevice.which = (SDL_MouseID)devices_get_instance(msg_core_get_mouse_device(src)->device);
      profile_func_end;
      return true;

    case SDL_EVENT_MOUSE_MOTION:
      out_event->motion.type = (SDL_EventType)src->type;
      out_event->motion.timestamp = (Uint64)src->timestamp;
      out_event->motion.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_mouse_motion(src)->window);
      out_event->motion.which = (SDL_MouseID)devices_get_instance(msg_core_get_mouse_motion(src)->device);
      out_event->motion.state = (SDL_MouseButtonFlags)msg_core_get_mouse_motion(src)->button_mask;
      out_event->motion.x = msg_core_get_mouse_motion(src)->x;
      out_event->motion.y = msg_core_get_mouse_motion(src)->y;
      out_event->motion.xrel = msg_core_get_mouse_motion(src)->xrel;
      out_event->motion.yrel = msg_core_get_mouse_motion(src)->yrel;
      profile_func_end;
      return true;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
      out_event->button.type = (SDL_EventType)src->type;
      out_event->button.timestamp = (Uint64)src->timestamp;
      out_event->button.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_mouse_button(src)->window);
      out_event->button.which = (SDL_MouseID)devices_get_instance(msg_core_get_mouse_button(src)->device);
      out_event->button.button = mouse_button_to_sdl(msg_core_get_mouse_button(src)->button);
      out_event->button.down = msg_core_get_mouse_button(src)->down != 0;
      out_event->button.clicks = (Uint8)msg_core_get_mouse_button(src)->clicks;
      out_event->button.x = msg_core_get_mouse_button(src)->x;
      out_event->button.y = msg_core_get_mouse_button(src)->y;
      profile_func_end;
      return true;

    case SDL_EVENT_MOUSE_WHEEL:
      out_event->wheel.type = (SDL_EventType)src->type;
      out_event->wheel.timestamp = (Uint64)src->timestamp;
      out_event->wheel.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_mouse_wheel(src)->window);
      out_event->wheel.which = (SDL_MouseID)devices_get_instance(msg_core_get_mouse_wheel(src)->device);
      out_event->wheel.x = msg_core_get_mouse_wheel(src)->x;
      out_event->wheel.y = msg_core_get_mouse_wheel(src)->y;
      out_event->wheel.direction = (SDL_MouseWheelDirection)msg_core_get_mouse_wheel(src)->direction;
      out_event->wheel.mouse_x = msg_core_get_mouse_wheel(src)->mouse_x;
      out_event->wheel.mouse_y = msg_core_get_mouse_wheel(src)->mouse_y;
      profile_func_end;
      return true;

    case SDL_EVENT_JOYSTICK_ADDED:
    case SDL_EVENT_JOYSTICK_REMOVED:
    case SDL_EVENT_JOYSTICK_UPDATE_COMPLETE:
      out_event->jdevice.type = (SDL_EventType)src->type;
      out_event->jdevice.timestamp = (Uint64)src->timestamp;
      out_event->jdevice.which = (SDL_JoystickID)joystick_to_native_id(msg_core_get_joystick_device(src)->joystick);
      profile_func_end;
      return true;

    case SDL_EVENT_JOYSTICK_AXIS_MOTION:
      out_event->jaxis.type = (SDL_EventType)src->type;
      out_event->jaxis.timestamp = (Uint64)src->timestamp;
      out_event->jaxis.which = (SDL_JoystickID)joystick_to_native_id(msg_core_get_joystick_axis(src)->joystick);
      out_event->jaxis.axis = (Uint8)msg_core_get_joystick_axis(src)->axis;
      out_event->jaxis.value = (Sint16)msg_core_get_joystick_axis(src)->value;
      profile_func_end;
      return true;

    case SDL_EVENT_JOYSTICK_BALL_MOTION:
      out_event->jball.type = (SDL_EventType)src->type;
      out_event->jball.timestamp = (Uint64)src->timestamp;
      out_event->jball.which = (SDL_JoystickID)joystick_to_native_id(msg_core_get_joystick_ball(src)->joystick);
      out_event->jball.ball = (Uint8)msg_core_get_joystick_ball(src)->ball;
      out_event->jball.xrel = (Sint16)msg_core_get_joystick_ball(src)->xrel;
      out_event->jball.yrel = (Sint16)msg_core_get_joystick_ball(src)->yrel;
      profile_func_end;
      return true;
  }

  switch (src->type) {
    case SDL_EVENT_JOYSTICK_HAT_MOTION:
      out_event->jhat.type = (SDL_EventType)src->type;
      out_event->jhat.timestamp = (Uint64)src->timestamp;
      out_event->jhat.which = (SDL_JoystickID)joystick_to_native_id(msg_core_get_joystick_hat(src)->joystick);
      out_event->jhat.hat = (Uint8)msg_core_get_joystick_hat(src)->hat;
      out_event->jhat.value = (Uint8)msg_core_get_joystick_hat(src)->value;
      profile_func_end;
      return true;

    case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
    case SDL_EVENT_JOYSTICK_BUTTON_UP:
      out_event->jbutton.type = (SDL_EventType)src->type;
      out_event->jbutton.timestamp = (Uint64)src->timestamp;
      out_event->jbutton.which = (SDL_JoystickID)joystick_to_native_id(msg_core_get_joystick_button(src)->joystick);
      out_event->jbutton.button = (Uint8)msg_core_get_joystick_button(src)->button;
      out_event->jbutton.down = msg_core_get_joystick_button(src)->down != 0;
      profile_func_end;
      return true;

    case SDL_EVENT_JOYSTICK_BATTERY_UPDATED:
      out_event->jbattery.type = (SDL_EventType)src->type;
      out_event->jbattery.timestamp = (Uint64)src->timestamp;
      out_event->jbattery.which = (SDL_JoystickID)joystick_to_native_id(msg_core_get_joystick_battery(src)->joystick);
      out_event->jbattery.state = (SDL_PowerState)joystick_battery_state_to_native(msg_core_get_joystick_battery(src)->state);
      out_event->jbattery.percent = (int)msg_core_get_joystick_battery(src)->percent;
      profile_func_end;
      return true;

    case SDL_EVENT_GAMEPAD_ADDED:
    case SDL_EVENT_GAMEPAD_REMOVED:
    case SDL_EVENT_GAMEPAD_REMAPPED:
    case SDL_EVENT_GAMEPAD_UPDATE_COMPLETE:
    case SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED:
      out_event->gdevice.type = (SDL_EventType)src->type;
      out_event->gdevice.timestamp = (Uint64)src->timestamp;
      out_event->gdevice.which = (SDL_JoystickID)devices_get_instance(msg_core_get_gamepad_device(src)->device);
      profile_func_end;
      return true;

    case SDL_EVENT_GAMEPAD_AXIS_MOTION:
      out_event->gaxis.type = (SDL_EventType)src->type;
      out_event->gaxis.timestamp = (Uint64)src->timestamp;
      out_event->gaxis.which = (SDL_JoystickID)devices_get_instance(msg_core_get_gamepad_axis(src)->device);
      out_event->gaxis.axis = (Uint8)msg_core_get_gamepad_axis(src)->axis;
      out_event->gaxis.value = (Sint16)msg_core_get_gamepad_axis(src)->value;
      profile_func_end;
      return true;

    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
    case SDL_EVENT_GAMEPAD_BUTTON_UP:
      out_event->gbutton.type = (SDL_EventType)src->type;
      out_event->gbutton.timestamp = (Uint64)src->timestamp;
      out_event->gbutton.which = (SDL_JoystickID)devices_get_instance(msg_core_get_gamepad_button(src)->device);
      out_event->gbutton.button = (Uint8)msg_core_get_gamepad_button(src)->button;
      out_event->gbutton.down = msg_core_get_gamepad_button(src)->down != 0;
      profile_func_end;
      return true;

    case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
    case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
    case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
      out_event->gtouchpad.type = (SDL_EventType)src->type;
      out_event->gtouchpad.timestamp = (Uint64)src->timestamp;
      out_event->gtouchpad.which = (SDL_JoystickID)devices_get_instance(msg_core_get_gamepad_touchpad(src)->device);
      out_event->gtouchpad.touchpad = (Sint32)msg_core_get_gamepad_touchpad(src)->touchpad;
      out_event->gtouchpad.finger = (Sint32)msg_core_get_gamepad_touchpad(src)->finger;
      out_event->gtouchpad.x = msg_core_get_gamepad_touchpad(src)->x;
      out_event->gtouchpad.y = msg_core_get_gamepad_touchpad(src)->y;
      out_event->gtouchpad.pressure = msg_core_get_gamepad_touchpad(src)->pressure;
      profile_func_end;
      return true;

    case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:
      out_event->gsensor.type = (SDL_EventType)src->type;
      out_event->gsensor.timestamp = (Uint64)src->timestamp;
      out_event->gsensor.which = (SDL_JoystickID)devices_get_instance(msg_core_get_gamepad_sensor(src)->device);
      out_event->gsensor.sensor = (Sint32)msg_core_get_gamepad_sensor(src)->sensor;
      out_event->gsensor.data[0] = msg_core_get_gamepad_sensor(src)->data[0];
      out_event->gsensor.data[1] = msg_core_get_gamepad_sensor(src)->data[1];
      out_event->gsensor.data[2] = msg_core_get_gamepad_sensor(src)->data[2];
      out_event->gsensor.sensor_timestamp = (Uint64)msg_core_get_gamepad_sensor(src)->sensor_timestamp;
      profile_func_end;
      return true;

    default:
      break;
  }

  switch (src->type) {
    case SDL_EVENT_AUDIO_DEVICE_ADDED:
    case SDL_EVENT_AUDIO_DEVICE_REMOVED:
    case SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED:
      out_event->adevice.type = (SDL_EventType)src->type;
      out_event->adevice.timestamp = (Uint64)src->timestamp;
      out_event->adevice.which = (SDL_AudioDeviceID)audio_device_to_native_id(msg_core_get_audio_device(src)->audio);
      out_event->adevice.recording = audio_device_get_type(msg_core_get_audio_device(src)->audio) ==
                                     AUDIO_DEVICE_TYPE_RECORDING;
      profile_func_end;
      return true;

    case SDL_EVENT_CAMERA_DEVICE_ADDED:
    case SDL_EVENT_CAMERA_DEVICE_REMOVED:
    case SDL_EVENT_CAMERA_DEVICE_APPROVED:
    case SDL_EVENT_CAMERA_DEVICE_DENIED:
      out_event->cdevice.type = (SDL_EventType)src->type;
      out_event->cdevice.timestamp = (Uint64)src->timestamp;
      out_event->cdevice.which = (SDL_CameraID)camera_to_native_id(msg_core_get_camera_device(src)->camera);
      profile_func_end;
      return true;

    case SDL_EVENT_RENDER_TARGETS_RESET:
    case SDL_EVENT_RENDER_DEVICE_RESET:
    case SDL_EVENT_RENDER_DEVICE_LOST:
      out_event->render.type = (SDL_EventType)src->type;
      out_event->render.timestamp = (Uint64)src->timestamp;
      out_event->render.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_render(src)->window);
      profile_func_end;
      return true;

    case SDL_EVENT_FINGER_DOWN:
    case SDL_EVENT_FINGER_UP:
    case SDL_EVENT_FINGER_MOTION:
    case SDL_EVENT_FINGER_CANCELED:
      out_event->tfinger.type = (SDL_EventType)src->type;
      out_event->tfinger.timestamp = (Uint64)src->timestamp;
      out_event->tfinger.touchID = (SDL_TouchID)devices_get_instance(msg_core_get_touch(src)->device);
      out_event->tfinger.fingerID = (SDL_FingerID)msg_core_get_touch(src)->finger_id;
      out_event->tfinger.x = msg_core_get_touch(src)->x;
      out_event->tfinger.y = msg_core_get_touch(src)->y;
      out_event->tfinger.dx = msg_core_get_touch(src)->dx;
      out_event->tfinger.dy = msg_core_get_touch(src)->dy;
      out_event->tfinger.pressure = msg_core_get_touch(src)->pressure;
      out_event->tfinger.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_touch(src)->window);
      profile_func_end;
      return true;

    case SDL_EVENT_PEN_PROXIMITY_IN:
    case SDL_EVENT_PEN_PROXIMITY_OUT:
      out_event->pproximity.type = (SDL_EventType)src->type;
      out_event->pproximity.timestamp = (Uint64)src->timestamp;
      out_event->pproximity.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_pen_proximity(src)->window);
      out_event->pproximity.which = (SDL_PenID)msg_core_get_pen_proximity(src)->pen_id;
      profile_func_end;
      return true;

    case SDL_EVENT_PEN_MOTION:
      out_event->pmotion.type = (SDL_EventType)src->type;
      out_event->pmotion.timestamp = (Uint64)src->timestamp;
      out_event->pmotion.windowID = (SDL_WindowID)window_to_native_id(msg_core_get_pen_motion(src)->window);
      out_event->pmotion.which = (SDL_PenID)msg_core_get_pen_motion(src)->pen_id;
      out_event->pmotion.pen_state = (SDL_PenInputFlags)msg_core_get_pen_motion(src)->pen_state;
      out_event->pmotion.x = msg_core_get_pen_motion(src)->x;
      out_event->pmotion.y = msg_core_get_pen_motion(src)->y;
      profile_func_end;
      return true;

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
      profile_func_end;
      return true;

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
      profile_func_end;
      return true;

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
      profile_func_end;
      return true;

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
      profile_func_end;
      return true;

    case SDL_EVENT_CLIPBOARD_UPDATE:
      out_event->clipboard.type = (SDL_EventType)src->type;
      out_event->clipboard.timestamp = (Uint64)src->timestamp;
      out_event->clipboard.owner = msg_core_get_clipboard(src)->owner != 0;
      out_event->clipboard.num_mime_types = (Sint32)msg_core_get_clipboard(src)->num_mime_types;
      out_event->clipboard.mime_types = (const char**)msg_core_get_clipboard(src)->mime_types;
      profile_func_end;
      return true;

    case SDL_EVENT_SENSOR_UPDATE:
      out_event->sensor.type = (SDL_EventType)src->type;
      out_event->sensor.timestamp = (Uint64)src->timestamp;
      out_event->sensor.which = (SDL_SensorID)sensor_to_native_id(msg_core_get_sensor(src)->sensor);
      safe_for (sz idx = 0; idx < count_of(msg_core_get_sensor(src)->data); idx += 1) {
        out_event->sensor.data[idx] = msg_core_get_sensor(src)->data[idx];
      }
      out_event->sensor.sensor_timestamp = (Uint64)msg_core_get_sensor(src)->sensor_timestamp;
      profile_func_end;
      return true;

    default:
      out_event->common.type = (Uint32)src->type;
      out_event->common.timestamp = (Uint64)src->timestamp;
      profile_func_end;
      return true;
  }
}

func b32 msg_poll(msg* out_msg) {
  profile_func_begin;
  SDL_Event native_event;

  msg_refresh_synthetic_device_msgs();
  if (!out_msg) {
    thread_log_error("Rejected message poll because output buffer is NULL");
    profile_func_end;
    return false;
  }
  assert(out_msg != NULL);

  safe_while (SDL_PollEvent(&native_event)) {
    if (msg_dispatch_native_event(&native_event, out_msg)) {
      profile_func_end;
      return true;
    }
  }

  profile_func_end;
  return false;
}

func b32 _msg_post_native(const void* native_event, msg* out_msg, callsite site) {
  profile_func_begin;
  if (native_event == NULL || out_msg == NULL) {
    thread_log_error("Rejected native message post native_event=%p out_msg=%p", (void*)native_event, (void*)out_msg);
    profile_func_end;
    return false;
  }

  b32 result = msg_dispatch_native_event_with_site((const SDL_Event*)native_event, out_msg, site);
  profile_func_end;
  return result;
}

func b32 _msg_post(const msg* src, callsite site) {
  profile_func_begin;
  msg posted_msg;

  if (!src) {
    thread_log_error("Rejected message post because source is NULL");
    profile_func_end;
    return false;
  }
  assert(src != NULL);

  posted_msg = *src;
  posted_msg.post_site = site;
  if (!msg_category_is_valid(posted_msg.category)) {
    thread_log_error("Rejected message post because category is invalid category=%u type=%u",
                     (u32)posted_msg.category,
                     posted_msg.type);
    profile_func_end;
    return false;
  }

  if (!msg_filter_accept(&posted_msg)) {
    profile_func_end;
    return false;
  }

  if (!msg_dispatch_handlers(&posted_msg)) {
    profile_func_end;
    return false;
  }

  profile_func_end;
  return true;
}

func u64 msg_add_handler(const msg_handler_desc* desc) {
  profile_func_begin;
  if (desc == NULL || desc->handler_fn == NULL || msg_handler_count >= MSG_HANDLER_CAP) {
    thread_log_error("Rejected message handler add desc=%p has_fn=%u count=%u cap=%u",
                     (void*)desc,
                     (u32)(desc != NULL && desc->handler_fn != NULL),
                     msg_handler_count,
                     MSG_HANDLER_CAP);
    profile_func_end;
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
      .category = desc->category,
      .type = desc->type,
  };
  msg_handler_count += 1;
  msg_handler_sort_entries();
  thread_log_trace("msg_add_handler: id=%llu count=%u", (unsigned long long)handler_id, msg_handler_count);
  profile_func_end;
  return handler_id;
}

func b32 msg_remove_handler(u64 handler_id) {
  profile_func_begin;
  if (handler_id == 0) {
    profile_func_end;
    return false;
  }
  assert(handler_id != 0);

  safe_for (u32 idx = 0; idx < msg_handler_count; idx += 1) {
    if (msg_handler_entries[idx].handler_id != handler_id) {
      continue;
    }

    safe_for (u32 move_idx = idx + 1; move_idx < msg_handler_count; move_idx += 1) {
      msg_handler_entries[move_idx - 1] = msg_handler_entries[move_idx];
    }

    msg_handler_count -= 1;
    msg_handler_entries[msg_handler_count] = (msg_handler_entry) {0};
    thread_log_trace("msg_remove_handler: id=%llu count=%u", (unsigned long long)handler_id, msg_handler_count);
    profile_func_end;
    return true;
  }

  thread_log_warn("Message handler removal missed id=%llu", (unsigned long long)handler_id);
  profile_func_end;
  return false;
}

func void msg_set_filter(msg_filter_fn filter_fn, void* user_data) {
  profile_func_begin;
  msg_filter_current = filter_fn;
  msg_filter_user_data = user_data;
  thread_log_trace("Updated message filter enabled=%u user_data=%p", (u32)(filter_fn != NULL), user_data);
  profile_func_end;
}

func b32 msg_from_native(const void* native_event, msg* out_msg) {
  profile_func_begin;
  if (native_event == NULL || out_msg == NULL) {
    profile_func_end;
    return false;
  }
  profile_func_end;
  return msg_from_sdl((const SDL_Event*)native_event, out_msg);
}

func b32 msg_to_native(const msg* src, void* native_event) {
  profile_func_begin;
  if (src == NULL || native_event == NULL) {
    profile_func_end;
    return false;
  }
  profile_func_end;
  return msg_to_sdl_event((msg*)src, (SDL_Event*)native_event);
}
