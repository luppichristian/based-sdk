// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/msg.h"
#include "../sdl3_include.h"
#include "basic/utility_defines.h"
#include <SDL3/SDL_hidapi.h>

func void tablet_internal_on_msg(const msg* src);
func void keyboard_internal_on_msg(const msg* src);
func void mouse_internal_on_msg(const msg* src);
func void gamepads_internal_on_msg(const msg* src);
func void bindings_internal_on_msg(const msg* src);

func void msg_notify_internal_listeners(const msg* src) {
  keyboard_internal_on_msg(src);
  mouse_internal_on_msg(src);
  gamepads_internal_on_msg(src);
  tablet_internal_on_msg(src);
  bindings_internal_on_msg(src);
}

const_var i32 MSG_PAYLOAD_CODE = 0x42415345;
const_var sz MSG_DEVICE_TRACK_CAP = 128;
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

func b32 msg_remove_handler(u64 handler_id);

func u64 msg_hash_path(cstr8 src) {
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

func b32 msg_device_id_equal(device_id lhs, device_id rhs) {
  return lhs.type == rhs.type && lhs.instance == rhs.instance;
}

func b32 msg_device_list_contains(const device_id* list, sz count, device_id src) {
  if (!list) {
    return 0;
  }

  for (sz item_idx = 0; item_idx < count; item_idx += 1) {
    if (msg_device_id_equal(list[item_idx], src)) {
      return 1;
    }
  }

  return 0;
}

func sz msg_collect_touch_devices(device_id* out_ids, sz cap) {
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

  return out_count;
}

func sz msg_collect_tablet_devices(device_id* out_ids, sz cap) {
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

  return out_count;
}

func void msg_post_touch_device_event(u32 type, device_id device) {
  msg device_msg = {0};
  device_msg.type = type;
  device_msg.touch_device.device = device;
  (void)_msg_post(&device_msg, CALLSITE_HERE);
}

func void msg_post_tablet_device_event(u32 type, device_id device) {
  msg device_msg = {0};
  device_msg.type = type;
  device_msg.tablet_device.device = device;
  (void)_msg_post(&device_msg, CALLSITE_HERE);
}

func void msg_refresh_touch_devices(void) {
  local_persist device_id previous_ids[MSG_DEVICE_TRACK_CAP] = {0};
  local_persist sz previous_count = 0;
  device_id current_ids[MSG_DEVICE_TRACK_CAP] = {0};
  sz current_count = msg_collect_touch_devices(current_ids, count_of(current_ids));

  for (sz item_idx = 0; item_idx < current_count; item_idx += 1) {
    if (!msg_device_list_contains(previous_ids, previous_count, current_ids[item_idx])) {
      msg_post_touch_device_event(MSG_TYPE_TOUCH_ADDED, current_ids[item_idx]);
    }
  }

  for (sz item_idx = 0; item_idx < previous_count; item_idx += 1) {
    if (!msg_device_list_contains(current_ids, current_count, previous_ids[item_idx])) {
      msg_post_touch_device_event(MSG_TYPE_TOUCH_REMOVED, previous_ids[item_idx]);
    }
  }

  previous_count = current_count;
  for (sz item_idx = 0; item_idx < previous_count; item_idx += 1) {
    previous_ids[item_idx] = current_ids[item_idx];
  }
}

func void msg_refresh_tablet_devices(void) {
  local_persist device_id previous_ids[MSG_DEVICE_TRACK_CAP] = {0};
  local_persist sz previous_count = 0;
  device_id current_ids[MSG_DEVICE_TRACK_CAP] = {0};
  sz current_count = msg_collect_tablet_devices(current_ids, count_of(current_ids));

  for (sz item_idx = 0; item_idx < current_count; item_idx += 1) {
    if (!msg_device_list_contains(previous_ids, previous_count, current_ids[item_idx])) {
      msg_post_tablet_device_event(MSG_TYPE_TABLET_ADDED, current_ids[item_idx]);
    }
  }

  for (sz item_idx = 0; item_idx < previous_count; item_idx += 1) {
    if (!msg_device_list_contains(current_ids, current_count, previous_ids[item_idx])) {
      msg_post_tablet_device_event(MSG_TYPE_TABLET_REMOVED, previous_ids[item_idx]);
    }
  }

  previous_count = current_count;
  for (sz item_idx = 0; item_idx < previous_count; item_idx += 1) {
    previous_ids[item_idx] = current_ids[item_idx];
  }
}

func void msg_refresh_synthetic_device_messages(void) {
  msg_refresh_touch_devices();
  msg_refresh_tablet_devices();
}

// NOTE:
// SDL uses one process-global event queue. This module intentionally mirrors
// that model: any thread may post, while one designated thread should pump/poll/wait.

func b32 msg_handler_should_run_for_stage(u32 options, msg_handler_stage stage) {
  u32 stage_options = options &
                      (MSG_HANDLER_OPTION_STAGE_BEFORE_POST | MSG_HANDLER_OPTION_STAGE_AFTER_POST |
                       MSG_HANDLER_OPTION_STAGE_POST_FAILED);

  if (stage_options == 0) {
    stage_options = MSG_HANDLER_OPTION_STAGE_BEFORE_POST;
  }

  switch (stage) {
    case MSG_HANDLER_STAGE_BEFORE_POST:
      return (stage_options & MSG_HANDLER_OPTION_STAGE_BEFORE_POST) != 0;
    case MSG_HANDLER_STAGE_AFTER_POST:
      return (stage_options & MSG_HANDLER_OPTION_STAGE_AFTER_POST) != 0;
    case MSG_HANDLER_STAGE_POST_FAILED:
      return (stage_options & MSG_HANDLER_OPTION_STAGE_POST_FAILED) != 0;
    default:
      return 0;
  }
}

func b32 msg_handler_should_run_for_type(const msg_handler_entry* entry, u32 type) {
  if (entry->type_min == 0 && entry->type_max == 0) {
    return 1;
  }

  if (entry->type_min <= entry->type_max) {
    return in_range(type, entry->type_min, entry->type_max);
  }

  return in_range(type, entry->type_max, entry->type_min);
}

func void msg_handler_sort_entries(void) {
  for (u32 outer_index = 0; outer_index < msg_handler_count; outer_index += 1) {
    for (u32 inner_index = outer_index + 1; inner_index < msg_handler_count; inner_index += 1) {
      msg_handler_entry* lhs = &msg_handler_entries[outer_index];
      msg_handler_entry* rhs = &msg_handler_entries[inner_index];
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
}

func b32 msg_dispatch_handlers(msg* posted_msg, msg_handler_stage stage) {
  msg_handler_entry dispatch_entries[MSG_HANDLER_CAP] = {0};
  u64 once_handler_ids[MSG_HANDLER_CAP] = {0};
  u32 dispatch_cap = (u32)count_of(dispatch_entries);
  u32 once_handler_cap = (u32)count_of(once_handler_ids);
  u32 dispatch_count = msg_handler_count;
  u32 once_handler_count = 0;

  if (dispatch_count > dispatch_cap) {
    dispatch_count = dispatch_cap;
  }

  for (u32 index = 0; index < dispatch_count; index += 1) {
    dispatch_entries[index] = msg_handler_entries[index];
  }

  for (u32 index = 0; index < dispatch_count; index += 1) {
    msg_handler_entry* entry = &dispatch_entries[index];

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
      for (u32 once_index = 0; once_index < once_handler_count; once_index += 1) {
        (void)msg_remove_handler(once_handler_ids[once_index]);
      }
      return 0;
    }
  }

  for (u32 once_index = 0; once_index < once_handler_count; once_index += 1) {
    (void)msg_remove_handler(once_handler_ids[once_index]);
  }

  return 1;
}

func void msg_apply_common(msg* dst, const SDL_Event* src) {
  dst->type = src->type;
  dst->timestamp = src->common.timestamp;
  dst->post_site = (callsite) {0};
}

func b32 msg_from_sdl(const SDL_Event* src, msg* out_msg) {
  if (!src || !out_msg) {
    return 0;
  }

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
      out_msg->display.display_id = (u32)src->display.displayID;
      out_msg->display.data1 = (i32)src->display.data1;
      out_msg->display.data2 = (i32)src->display.data2;
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
      out_msg->window.window_id = (u32)src->window.windowID;
      out_msg->window.data1 = (i32)src->window.data1;
      out_msg->window.data2 = (i32)src->window.data2;
      return 1;

    case SDL_EVENT_KEYBOARD_ADDED:
    case SDL_EVENT_KEYBOARD_REMOVED:
      out_msg->keyboard_device.device.type = DEVICE_TYPE_KEYBOARD;
      out_msg->keyboard_device.device.instance = (u64)src->kdevice.which;
      return 1;

    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
      out_msg->keyboard.window_id = (u32)src->key.windowID;
      out_msg->keyboard.device.type = DEVICE_TYPE_KEYBOARD;
      out_msg->keyboard.device.instance = (u64)src->key.which;
      out_msg->keyboard.scancode = (u32)src->key.scancode;
      out_msg->keyboard.keycode = (i32)src->key.key;
      out_msg->keyboard.modifiers = (u16)src->key.mod;
      out_msg->keyboard.raw = (u16)src->key.raw;
      out_msg->keyboard.down = src->key.down ? 1 : 0;
      out_msg->keyboard.repeat = src->key.repeat ? 1 : 0;
      return 1;

    case SDL_EVENT_TEXT_EDITING:
      out_msg->text_editing.window_id = (u32)src->edit.windowID;
      out_msg->text_editing.text = src->edit.text;
      out_msg->text_editing.start = (i32)src->edit.start;
      out_msg->text_editing.length = (i32)src->edit.length;
      return 1;

    case SDL_EVENT_TEXT_EDITING_CANDIDATES:
      out_msg->text_editing_candidates.window_id = (u32)src->edit_candidates.windowID;
      out_msg->text_editing_candidates.candidates = (cstr8 const*)src->edit_candidates.candidates;
      out_msg->text_editing_candidates.num_candidates = (i32)src->edit_candidates.num_candidates;
      out_msg->text_editing_candidates.selected_candidate = (i32)src->edit_candidates.selected_candidate;
      out_msg->text_editing_candidates.horizontal = src->edit_candidates.horizontal ? 1 : 0;
      return 1;

    case SDL_EVENT_TEXT_INPUT:
      out_msg->text_input.window_id = (u32)src->text.windowID;
      out_msg->text_input.text = src->text.text;
      return 1;

    case SDL_EVENT_MOUSE_ADDED:
    case SDL_EVENT_MOUSE_REMOVED:
      out_msg->mouse_device.device.type = DEVICE_TYPE_MOUSE;
      out_msg->mouse_device.device.instance = (u64)src->mdevice.which;
      return 1;

    case SDL_EVENT_MOUSE_MOTION:
      out_msg->mouse_motion.window_id = (u32)src->motion.windowID;
      out_msg->mouse_motion.device.type = DEVICE_TYPE_MOUSE;
      out_msg->mouse_motion.device.instance = (u64)src->motion.which;
      out_msg->mouse_motion.button_mask = (u32)src->motion.state;
      out_msg->mouse_motion.x = src->motion.x;
      out_msg->mouse_motion.y = src->motion.y;
      out_msg->mouse_motion.xrel = src->motion.xrel;
      out_msg->mouse_motion.yrel = src->motion.yrel;
      return 1;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
      out_msg->mouse_button.window_id = (u32)src->button.windowID;
      out_msg->mouse_button.device.type = DEVICE_TYPE_MOUSE;
      out_msg->mouse_button.device.instance = (u64)src->button.which;
      out_msg->mouse_button.button = (mouse_button)src->button.button;
      out_msg->mouse_button.down = src->button.down ? 1 : 0;
      out_msg->mouse_button.clicks = (u8)src->button.clicks;
      out_msg->mouse_button.x = src->button.x;
      out_msg->mouse_button.y = src->button.y;
      return 1;

    case SDL_EVENT_MOUSE_WHEEL:
      out_msg->mouse_wheel.window_id = (u32)src->wheel.windowID;
      out_msg->mouse_wheel.device.type = DEVICE_TYPE_MOUSE;
      out_msg->mouse_wheel.device.instance = (u64)src->wheel.which;
      out_msg->mouse_wheel.x = src->wheel.x;
      out_msg->mouse_wheel.y = src->wheel.y;
      out_msg->mouse_wheel.direction = (u32)src->wheel.direction;
      out_msg->mouse_wheel.mouse_x = src->wheel.mouse_x;
      out_msg->mouse_wheel.mouse_y = src->wheel.mouse_y;
      return 1;

    case SDL_EVENT_JOYSTICK_ADDED:
    case SDL_EVENT_JOYSTICK_REMOVED:
    case SDL_EVENT_JOYSTICK_UPDATE_COMPLETE:
      out_msg->joystick_device.device.type = DEVICE_TYPE_JOYSTICK;
      out_msg->joystick_device.device.instance = (u64)src->jdevice.which;
      return 1;

    case SDL_EVENT_JOYSTICK_AXIS_MOTION:
      out_msg->joystick_axis.device.type = DEVICE_TYPE_JOYSTICK;
      out_msg->joystick_axis.device.instance = (u64)src->jaxis.which;
      out_msg->joystick_axis.axis = (u8)src->jaxis.axis;
      out_msg->joystick_axis.value = (i16)src->jaxis.value;
      return 1;

    case SDL_EVENT_JOYSTICK_BALL_MOTION:
      out_msg->joystick_ball.device.type = DEVICE_TYPE_JOYSTICK;
      out_msg->joystick_ball.device.instance = (u64)src->jball.which;
      out_msg->joystick_ball.ball = (u8)src->jball.ball;
      out_msg->joystick_ball.xrel = (i16)src->jball.xrel;
      out_msg->joystick_ball.yrel = (i16)src->jball.yrel;
      return 1;

    case SDL_EVENT_JOYSTICK_HAT_MOTION:
      out_msg->joystick_hat.device.type = DEVICE_TYPE_JOYSTICK;
      out_msg->joystick_hat.device.instance = (u64)src->jhat.which;
      out_msg->joystick_hat.hat = (u8)src->jhat.hat;
      out_msg->joystick_hat.value = (u8)src->jhat.value;
      return 1;

    case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
    case SDL_EVENT_JOYSTICK_BUTTON_UP:
      out_msg->joystick_button.device.type = DEVICE_TYPE_JOYSTICK;
      out_msg->joystick_button.device.instance = (u64)src->jbutton.which;
      out_msg->joystick_button.button = (u8)src->jbutton.button;
      out_msg->joystick_button.down = src->jbutton.down ? 1 : 0;
      return 1;

    case SDL_EVENT_JOYSTICK_BATTERY_UPDATED:
      out_msg->joystick_battery.device.type = DEVICE_TYPE_JOYSTICK;
      out_msg->joystick_battery.device.instance = (u64)src->jbattery.which;
      out_msg->joystick_battery.state = (i32)src->jbattery.state;
      out_msg->joystick_battery.percent = (i32)src->jbattery.percent;
      return 1;

    case SDL_EVENT_GAMEPAD_ADDED:
    case SDL_EVENT_GAMEPAD_REMOVED:
    case SDL_EVENT_GAMEPAD_REMAPPED:
    case SDL_EVENT_GAMEPAD_UPDATE_COMPLETE:
    case SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED:
      out_msg->gamepad_device.device.type = DEVICE_TYPE_GAMEPAD;
      out_msg->gamepad_device.device.instance = (u64)src->gdevice.which;
      return 1;

    case SDL_EVENT_GAMEPAD_AXIS_MOTION:
      out_msg->gamepad_axis.device.type = DEVICE_TYPE_GAMEPAD;
      out_msg->gamepad_axis.device.instance = (u64)src->gaxis.which;
      out_msg->gamepad_axis.axis = (gamepad_axis)src->gaxis.axis;
      out_msg->gamepad_axis.value = (i16)src->gaxis.value;
      return 1;

    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
    case SDL_EVENT_GAMEPAD_BUTTON_UP:
      out_msg->gamepad_button.device.type = DEVICE_TYPE_GAMEPAD;
      out_msg->gamepad_button.device.instance = (u64)src->gbutton.which;
      out_msg->gamepad_button.button = (gamepad_button)src->gbutton.button;
      out_msg->gamepad_button.down = src->gbutton.down ? 1 : 0;
      return 1;

    default:
      break;
  }

  switch (src->type) {
    case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
    case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
    case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
      out_msg->gamepad_touchpad.device.type = DEVICE_TYPE_GAMEPAD;
      out_msg->gamepad_touchpad.device.instance = (u64)src->gtouchpad.which;
      out_msg->gamepad_touchpad.touchpad = (i32)src->gtouchpad.touchpad;
      out_msg->gamepad_touchpad.finger = (i32)src->gtouchpad.finger;
      out_msg->gamepad_touchpad.x = src->gtouchpad.x;
      out_msg->gamepad_touchpad.y = src->gtouchpad.y;
      out_msg->gamepad_touchpad.pressure = src->gtouchpad.pressure;
      return 1;

    case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:
      out_msg->gamepad_sensor.device.type = DEVICE_TYPE_GAMEPAD;
      out_msg->gamepad_sensor.device.instance = (u64)src->gsensor.which;
      out_msg->gamepad_sensor.sensor = (i32)src->gsensor.sensor;
      out_msg->gamepad_sensor.data[0] = src->gsensor.data[0];
      out_msg->gamepad_sensor.data[1] = src->gsensor.data[1];
      out_msg->gamepad_sensor.data[2] = src->gsensor.data[2];
      out_msg->gamepad_sensor.sensor_timestamp = (u64)src->gsensor.sensor_timestamp;
      return 1;

    case SDL_EVENT_AUDIO_DEVICE_ADDED:
    case SDL_EVENT_AUDIO_DEVICE_REMOVED:
    case SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED:
      out_msg->audio_device.audio_device_id = (u32)src->adevice.which;
      out_msg->audio_device.recording = src->adevice.recording ? 1 : 0;
      return 1;

    case SDL_EVENT_CAMERA_DEVICE_ADDED:
    case SDL_EVENT_CAMERA_DEVICE_REMOVED:
    case SDL_EVENT_CAMERA_DEVICE_APPROVED:
    case SDL_EVENT_CAMERA_DEVICE_DENIED:
      out_msg->camera_device.camera_id = (u32)src->cdevice.which;
      return 1;

    case SDL_EVENT_RENDER_TARGETS_RESET:
    case SDL_EVENT_RENDER_DEVICE_RESET:
    case SDL_EVENT_RENDER_DEVICE_LOST:
      out_msg->render.window_id = (u32)src->render.windowID;
      return 1;

    case SDL_EVENT_FINGER_DOWN:
    case SDL_EVENT_FINGER_UP:
    case SDL_EVENT_FINGER_MOTION:
    case SDL_EVENT_FINGER_CANCELED:
      out_msg->touch.device.type = DEVICE_TYPE_TOUCH;
      out_msg->touch.device.instance = (u64)src->tfinger.touchID;
      out_msg->touch.finger_id = (u64)src->tfinger.fingerID;
      out_msg->touch.x = src->tfinger.x;
      out_msg->touch.y = src->tfinger.y;
      out_msg->touch.dx = src->tfinger.dx;
      out_msg->touch.dy = src->tfinger.dy;
      out_msg->touch.pressure = src->tfinger.pressure;
      out_msg->touch.window_id = (u32)src->tfinger.windowID;
      return 1;

    case SDL_EVENT_PEN_PROXIMITY_IN:
    case SDL_EVENT_PEN_PROXIMITY_OUT:
      out_msg->pen_proximity.window_id = (u32)src->pproximity.windowID;
      out_msg->pen_proximity.device.type = DEVICE_TYPE_TABLET;
      out_msg->pen_proximity.device.instance = (u64)src->pproximity.which;
      out_msg->pen_proximity.pen_id = (u32)src->pproximity.which;
      return 1;

    case SDL_EVENT_PEN_MOTION:
      out_msg->pen_motion.window_id = (u32)src->pmotion.windowID;
      out_msg->pen_motion.device.type = DEVICE_TYPE_TABLET;
      out_msg->pen_motion.device.instance = (u64)src->pmotion.which;
      out_msg->pen_motion.pen_id = (u32)src->pmotion.which;
      out_msg->pen_motion.pen_state = (u32)src->pmotion.pen_state;
      out_msg->pen_motion.x = src->pmotion.x;
      out_msg->pen_motion.y = src->pmotion.y;
      return 1;

    case SDL_EVENT_PEN_DOWN:
    case SDL_EVENT_PEN_UP:
      out_msg->pen_touch.window_id = (u32)src->ptouch.windowID;
      out_msg->pen_touch.device.type = DEVICE_TYPE_TABLET;
      out_msg->pen_touch.device.instance = (u64)src->ptouch.which;
      out_msg->pen_touch.pen_id = (u32)src->ptouch.which;
      out_msg->pen_touch.pen_state = (u32)src->ptouch.pen_state;
      out_msg->pen_touch.x = src->ptouch.x;
      out_msg->pen_touch.y = src->ptouch.y;
      out_msg->pen_touch.eraser = src->ptouch.eraser ? 1 : 0;
      out_msg->pen_touch.down = src->ptouch.down ? 1 : 0;
      return 1;

    case SDL_EVENT_PEN_BUTTON_DOWN:
    case SDL_EVENT_PEN_BUTTON_UP:
      out_msg->pen_button.window_id = (u32)src->pbutton.windowID;
      out_msg->pen_button.device.type = DEVICE_TYPE_TABLET;
      out_msg->pen_button.device.instance = (u64)src->pbutton.which;
      out_msg->pen_button.pen_id = (u32)src->pbutton.which;
      out_msg->pen_button.pen_state = (u32)src->pbutton.pen_state;
      out_msg->pen_button.x = src->pbutton.x;
      out_msg->pen_button.y = src->pbutton.y;
      out_msg->pen_button.button = (u8)src->pbutton.button;
      out_msg->pen_button.down = src->pbutton.down ? 1 : 0;
      return 1;

    case SDL_EVENT_PEN_AXIS:
      out_msg->pen_axis.window_id = (u32)src->paxis.windowID;
      out_msg->pen_axis.device.type = DEVICE_TYPE_TABLET;
      out_msg->pen_axis.device.instance = (u64)src->paxis.which;
      out_msg->pen_axis.pen_id = (u32)src->paxis.which;
      out_msg->pen_axis.pen_state = (u32)src->paxis.pen_state;
      out_msg->pen_axis.x = src->paxis.x;
      out_msg->pen_axis.y = src->paxis.y;
      out_msg->pen_axis.axis = (u32)src->paxis.axis;
      out_msg->pen_axis.value = src->paxis.value;
      return 1;

    case SDL_EVENT_DROP_FILE:
    case SDL_EVENT_DROP_TEXT:
    case SDL_EVENT_DROP_BEGIN:
    case SDL_EVENT_DROP_COMPLETE:
    case SDL_EVENT_DROP_POSITION:
      out_msg->drop.window_id = (u32)src->drop.windowID;
      out_msg->drop.x = src->drop.x;
      out_msg->drop.y = src->drop.y;
      out_msg->drop.source = src->drop.source;
      out_msg->drop.data = src->drop.data;
      return 1;

    case SDL_EVENT_CLIPBOARD_UPDATE:
      out_msg->clipboard.owner = src->clipboard.owner ? 1 : 0;
      out_msg->clipboard.num_mime_types = (i32)src->clipboard.num_mime_types;
      out_msg->clipboard.mime_types = (cstr8 const*)src->clipboard.mime_types;
      return 1;

    case SDL_EVENT_SENSOR_UPDATE:
      out_msg->sensor.sensor_id = (u32)src->sensor.which;
      for (sz index = 0; index < count_of(out_msg->sensor.data); index += 1) {
        out_msg->sensor.data[index] = src->sensor.data[index];
      }
      out_msg->sensor.sensor_timestamp = (u64)src->sensor.sensor_timestamp;
      return 1;

    default:
      if (src->type >= SDL_EVENT_USER && src->user.code == (Sint32)MSG_PAYLOAD_CODE && src->user.data1 != NULL) {
        msg* payload = (msg*)src->user.data1;
        *out_msg = *payload;
        SDL_free(payload);
        return 1;
      }

      if (src->type >= SDL_EVENT_USER) {
        out_msg->custom.window_id = (u32)src->user.windowID;
        out_msg->custom.code = (i32)src->user.code;
        out_msg->custom.data1 = src->user.data1;
        out_msg->custom.data2 = src->user.data2;
      }
      return 1;
  }
}

func b32 msg_to_sdl_event(const msg* src, SDL_Event* out_event) {
  if (!src || !out_event) {
    return 0;
  }

  *out_event = (SDL_Event) {0};
  out_event->type = src->type;

  if (src->type >= SDL_EVENT_USER) {
    out_event->user.type = (Uint32)src->type;
    out_event->user.timestamp = (Uint64)src->timestamp;
    out_event->user.windowID = (SDL_WindowID)src->custom.window_id;
    out_event->user.code = (Sint32)src->custom.code;
    out_event->user.data1 = src->custom.data1;
    out_event->user.data2 = src->custom.data2;
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
      out_event->display.displayID = (SDL_DisplayID)src->display.display_id;
      out_event->display.data1 = (Sint32)src->display.data1;
      out_event->display.data2 = (Sint32)src->display.data2;
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
      out_event->window.windowID = (SDL_WindowID)src->window.window_id;
      out_event->window.data1 = (Sint32)src->window.data1;
      out_event->window.data2 = (Sint32)src->window.data2;
      return 1;

    case SDL_EVENT_KEYBOARD_ADDED:
    case SDL_EVENT_KEYBOARD_REMOVED:
      out_event->kdevice.type = (SDL_EventType)src->type;
      out_event->kdevice.timestamp = (Uint64)src->timestamp;
      out_event->kdevice.which = (SDL_KeyboardID)src->keyboard_device.device.instance;
      return 1;

    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
      out_event->key.type = (SDL_EventType)src->type;
      out_event->key.timestamp = (Uint64)src->timestamp;
      out_event->key.windowID = (SDL_WindowID)src->keyboard.window_id;
      out_event->key.which = (SDL_KeyboardID)src->keyboard.device.instance;
      out_event->key.scancode = (SDL_Scancode)src->keyboard.scancode;
      out_event->key.key = (SDL_Keycode)src->keyboard.keycode;
      out_event->key.mod = (SDL_Keymod)src->keyboard.modifiers;
      out_event->key.raw = (Uint16)src->keyboard.raw;
      out_event->key.down = src->keyboard.down != 0;
      out_event->key.repeat = src->keyboard.repeat != 0;
      return 1;

    case SDL_EVENT_TEXT_EDITING:
      out_event->edit.type = (SDL_EventType)src->type;
      out_event->edit.timestamp = (Uint64)src->timestamp;
      out_event->edit.windowID = (SDL_WindowID)src->text_editing.window_id;
      out_event->edit.text = src->text_editing.text;
      out_event->edit.start = (Sint32)src->text_editing.start;
      out_event->edit.length = (Sint32)src->text_editing.length;
      return 1;

    case SDL_EVENT_TEXT_EDITING_CANDIDATES:
      out_event->edit_candidates.type = (SDL_EventType)src->type;
      out_event->edit_candidates.timestamp = (Uint64)src->timestamp;
      out_event->edit_candidates.windowID = (SDL_WindowID)src->text_editing_candidates.window_id;
      out_event->edit_candidates.candidates = (const char* const*)src->text_editing_candidates.candidates;
      out_event->edit_candidates.num_candidates = (Sint32)src->text_editing_candidates.num_candidates;
      out_event->edit_candidates.selected_candidate = (Sint32)src->text_editing_candidates.selected_candidate;
      out_event->edit_candidates.horizontal = src->text_editing_candidates.horizontal != 0;
      return 1;

    case SDL_EVENT_TEXT_INPUT:
      out_event->text.type = (SDL_EventType)src->type;
      out_event->text.timestamp = (Uint64)src->timestamp;
      out_event->text.windowID = (SDL_WindowID)src->text_input.window_id;
      out_event->text.text = src->text_input.text;
      return 1;

    default:
      break;
  }

  switch (src->type) {
    case SDL_EVENT_MOUSE_ADDED:
    case SDL_EVENT_MOUSE_REMOVED:
      out_event->mdevice.type = (SDL_EventType)src->type;
      out_event->mdevice.timestamp = (Uint64)src->timestamp;
      out_event->mdevice.which = (SDL_MouseID)src->mouse_device.device.instance;
      return 1;

    case SDL_EVENT_MOUSE_MOTION:
      out_event->motion.type = (SDL_EventType)src->type;
      out_event->motion.timestamp = (Uint64)src->timestamp;
      out_event->motion.windowID = (SDL_WindowID)src->mouse_motion.window_id;
      out_event->motion.which = (SDL_MouseID)src->mouse_motion.device.instance;
      out_event->motion.state = (SDL_MouseButtonFlags)src->mouse_motion.button_mask;
      out_event->motion.x = src->mouse_motion.x;
      out_event->motion.y = src->mouse_motion.y;
      out_event->motion.xrel = src->mouse_motion.xrel;
      out_event->motion.yrel = src->mouse_motion.yrel;
      return 1;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
      out_event->button.type = (SDL_EventType)src->type;
      out_event->button.timestamp = (Uint64)src->timestamp;
      out_event->button.windowID = (SDL_WindowID)src->mouse_button.window_id;
      out_event->button.which = (SDL_MouseID)src->mouse_button.device.instance;
      out_event->button.button = (Uint8)src->mouse_button.button;
      out_event->button.down = src->mouse_button.down != 0;
      out_event->button.clicks = (Uint8)src->mouse_button.clicks;
      out_event->button.x = src->mouse_button.x;
      out_event->button.y = src->mouse_button.y;
      return 1;

    case SDL_EVENT_MOUSE_WHEEL:
      out_event->wheel.type = (SDL_EventType)src->type;
      out_event->wheel.timestamp = (Uint64)src->timestamp;
      out_event->wheel.windowID = (SDL_WindowID)src->mouse_wheel.window_id;
      out_event->wheel.which = (SDL_MouseID)src->mouse_wheel.device.instance;
      out_event->wheel.x = src->mouse_wheel.x;
      out_event->wheel.y = src->mouse_wheel.y;
      out_event->wheel.direction = (SDL_MouseWheelDirection)src->mouse_wheel.direction;
      out_event->wheel.mouse_x = src->mouse_wheel.mouse_x;
      out_event->wheel.mouse_y = src->mouse_wheel.mouse_y;
      return 1;

    case SDL_EVENT_JOYSTICK_ADDED:
    case SDL_EVENT_JOYSTICK_REMOVED:
    case SDL_EVENT_JOYSTICK_UPDATE_COMPLETE:
      out_event->jdevice.type = (SDL_EventType)src->type;
      out_event->jdevice.timestamp = (Uint64)src->timestamp;
      out_event->jdevice.which = (SDL_JoystickID)src->joystick_device.device.instance;
      return 1;

    case SDL_EVENT_JOYSTICK_AXIS_MOTION:
      out_event->jaxis.type = (SDL_EventType)src->type;
      out_event->jaxis.timestamp = (Uint64)src->timestamp;
      out_event->jaxis.which = (SDL_JoystickID)src->joystick_axis.device.instance;
      out_event->jaxis.axis = (Uint8)src->joystick_axis.axis;
      out_event->jaxis.value = (Sint16)src->joystick_axis.value;
      return 1;

    case SDL_EVENT_JOYSTICK_BALL_MOTION:
      out_event->jball.type = (SDL_EventType)src->type;
      out_event->jball.timestamp = (Uint64)src->timestamp;
      out_event->jball.which = (SDL_JoystickID)src->joystick_ball.device.instance;
      out_event->jball.ball = (Uint8)src->joystick_ball.ball;
      out_event->jball.xrel = (Sint16)src->joystick_ball.xrel;
      out_event->jball.yrel = (Sint16)src->joystick_ball.yrel;
      return 1;
  }

  switch (src->type) {
    case SDL_EVENT_JOYSTICK_HAT_MOTION:
      out_event->jhat.type = (SDL_EventType)src->type;
      out_event->jhat.timestamp = (Uint64)src->timestamp;
      out_event->jhat.which = (SDL_JoystickID)src->joystick_hat.device.instance;
      out_event->jhat.hat = (Uint8)src->joystick_hat.hat;
      out_event->jhat.value = (Uint8)src->joystick_hat.value;
      return 1;

    case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
    case SDL_EVENT_JOYSTICK_BUTTON_UP:
      out_event->jbutton.type = (SDL_EventType)src->type;
      out_event->jbutton.timestamp = (Uint64)src->timestamp;
      out_event->jbutton.which = (SDL_JoystickID)src->joystick_button.device.instance;
      out_event->jbutton.button = (Uint8)src->joystick_button.button;
      out_event->jbutton.down = src->joystick_button.down != 0;
      return 1;

    case SDL_EVENT_JOYSTICK_BATTERY_UPDATED:
      out_event->jbattery.type = (SDL_EventType)src->type;
      out_event->jbattery.timestamp = (Uint64)src->timestamp;
      out_event->jbattery.which = (SDL_JoystickID)src->joystick_battery.device.instance;
      out_event->jbattery.state = (SDL_PowerState)src->joystick_battery.state;
      out_event->jbattery.percent = (int)src->joystick_battery.percent;
      return 1;

    case SDL_EVENT_GAMEPAD_ADDED:
    case SDL_EVENT_GAMEPAD_REMOVED:
    case SDL_EVENT_GAMEPAD_REMAPPED:
    case SDL_EVENT_GAMEPAD_UPDATE_COMPLETE:
    case SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED:
      out_event->gdevice.type = (SDL_EventType)src->type;
      out_event->gdevice.timestamp = (Uint64)src->timestamp;
      out_event->gdevice.which = (SDL_JoystickID)src->gamepad_device.device.instance;
      return 1;

    case SDL_EVENT_GAMEPAD_AXIS_MOTION:
      out_event->gaxis.type = (SDL_EventType)src->type;
      out_event->gaxis.timestamp = (Uint64)src->timestamp;
      out_event->gaxis.which = (SDL_JoystickID)src->gamepad_axis.device.instance;
      out_event->gaxis.axis = (Uint8)src->gamepad_axis.axis;
      out_event->gaxis.value = (Sint16)src->gamepad_axis.value;
      return 1;

    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
    case SDL_EVENT_GAMEPAD_BUTTON_UP:
      out_event->gbutton.type = (SDL_EventType)src->type;
      out_event->gbutton.timestamp = (Uint64)src->timestamp;
      out_event->gbutton.which = (SDL_JoystickID)src->gamepad_button.device.instance;
      out_event->gbutton.button = (Uint8)src->gamepad_button.button;
      out_event->gbutton.down = src->gamepad_button.down != 0;
      return 1;

    case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
    case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
    case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
      out_event->gtouchpad.type = (SDL_EventType)src->type;
      out_event->gtouchpad.timestamp = (Uint64)src->timestamp;
      out_event->gtouchpad.which = (SDL_JoystickID)src->gamepad_touchpad.device.instance;
      out_event->gtouchpad.touchpad = (Sint32)src->gamepad_touchpad.touchpad;
      out_event->gtouchpad.finger = (Sint32)src->gamepad_touchpad.finger;
      out_event->gtouchpad.x = src->gamepad_touchpad.x;
      out_event->gtouchpad.y = src->gamepad_touchpad.y;
      out_event->gtouchpad.pressure = src->gamepad_touchpad.pressure;
      return 1;

    case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:
      out_event->gsensor.type = (SDL_EventType)src->type;
      out_event->gsensor.timestamp = (Uint64)src->timestamp;
      out_event->gsensor.which = (SDL_JoystickID)src->gamepad_sensor.device.instance;
      out_event->gsensor.sensor = (Sint32)src->gamepad_sensor.sensor;
      out_event->gsensor.data[0] = src->gamepad_sensor.data[0];
      out_event->gsensor.data[1] = src->gamepad_sensor.data[1];
      out_event->gsensor.data[2] = src->gamepad_sensor.data[2];
      out_event->gsensor.sensor_timestamp = (Uint64)src->gamepad_sensor.sensor_timestamp;
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
      out_event->adevice.which = (SDL_AudioDeviceID)src->audio_device.audio_device_id;
      out_event->adevice.recording = src->audio_device.recording != 0;
      return 1;

    case SDL_EVENT_CAMERA_DEVICE_ADDED:
    case SDL_EVENT_CAMERA_DEVICE_REMOVED:
    case SDL_EVENT_CAMERA_DEVICE_APPROVED:
    case SDL_EVENT_CAMERA_DEVICE_DENIED:
      out_event->cdevice.type = (SDL_EventType)src->type;
      out_event->cdevice.timestamp = (Uint64)src->timestamp;
      out_event->cdevice.which = (SDL_CameraID)src->camera_device.camera_id;
      return 1;

    case SDL_EVENT_RENDER_TARGETS_RESET:
    case SDL_EVENT_RENDER_DEVICE_RESET:
    case SDL_EVENT_RENDER_DEVICE_LOST:
      out_event->render.type = (SDL_EventType)src->type;
      out_event->render.timestamp = (Uint64)src->timestamp;
      out_event->render.windowID = (SDL_WindowID)src->render.window_id;
      return 1;

    case SDL_EVENT_FINGER_DOWN:
    case SDL_EVENT_FINGER_UP:
    case SDL_EVENT_FINGER_MOTION:
    case SDL_EVENT_FINGER_CANCELED:
      out_event->tfinger.type = (SDL_EventType)src->type;
      out_event->tfinger.timestamp = (Uint64)src->timestamp;
      out_event->tfinger.touchID = (SDL_TouchID)src->touch.device.instance;
      out_event->tfinger.fingerID = (SDL_FingerID)src->touch.finger_id;
      out_event->tfinger.x = src->touch.x;
      out_event->tfinger.y = src->touch.y;
      out_event->tfinger.dx = src->touch.dx;
      out_event->tfinger.dy = src->touch.dy;
      out_event->tfinger.pressure = src->touch.pressure;
      out_event->tfinger.windowID = (SDL_WindowID)src->touch.window_id;
      return 1;

    case SDL_EVENT_PEN_PROXIMITY_IN:
    case SDL_EVENT_PEN_PROXIMITY_OUT:
      out_event->pproximity.type = (SDL_EventType)src->type;
      out_event->pproximity.timestamp = (Uint64)src->timestamp;
      out_event->pproximity.windowID = (SDL_WindowID)src->pen_proximity.window_id;
      out_event->pproximity.which = (SDL_PenID)src->pen_proximity.pen_id;
      return 1;

    case SDL_EVENT_PEN_MOTION:
      out_event->pmotion.type = (SDL_EventType)src->type;
      out_event->pmotion.timestamp = (Uint64)src->timestamp;
      out_event->pmotion.windowID = (SDL_WindowID)src->pen_motion.window_id;
      out_event->pmotion.which = (SDL_PenID)src->pen_motion.pen_id;
      out_event->pmotion.pen_state = (SDL_PenInputFlags)src->pen_motion.pen_state;
      out_event->pmotion.x = src->pen_motion.x;
      out_event->pmotion.y = src->pen_motion.y;
      return 1;

    case SDL_EVENT_PEN_DOWN:
    case SDL_EVENT_PEN_UP:
      out_event->ptouch.type = (SDL_EventType)src->type;
      out_event->ptouch.timestamp = (Uint64)src->timestamp;
      out_event->ptouch.windowID = (SDL_WindowID)src->pen_touch.window_id;
      out_event->ptouch.which = (SDL_PenID)src->pen_touch.pen_id;
      out_event->ptouch.pen_state = (SDL_PenInputFlags)src->pen_touch.pen_state;
      out_event->ptouch.x = src->pen_touch.x;
      out_event->ptouch.y = src->pen_touch.y;
      out_event->ptouch.eraser = src->pen_touch.eraser != 0;
      out_event->ptouch.down = src->pen_touch.down != 0;
      return 1;

    case SDL_EVENT_PEN_BUTTON_DOWN:
    case SDL_EVENT_PEN_BUTTON_UP:
      out_event->pbutton.type = (SDL_EventType)src->type;
      out_event->pbutton.timestamp = (Uint64)src->timestamp;
      out_event->pbutton.windowID = (SDL_WindowID)src->pen_button.window_id;
      out_event->pbutton.which = (SDL_PenID)src->pen_button.pen_id;
      out_event->pbutton.pen_state = (SDL_PenInputFlags)src->pen_button.pen_state;
      out_event->pbutton.x = src->pen_button.x;
      out_event->pbutton.y = src->pen_button.y;
      out_event->pbutton.button = (Uint8)src->pen_button.button;
      out_event->pbutton.down = src->pen_button.down != 0;
      return 1;

    case SDL_EVENT_PEN_AXIS:
      out_event->paxis.type = (SDL_EventType)src->type;
      out_event->paxis.timestamp = (Uint64)src->timestamp;
      out_event->paxis.windowID = (SDL_WindowID)src->pen_axis.window_id;
      out_event->paxis.which = (SDL_PenID)src->pen_axis.pen_id;
      out_event->paxis.pen_state = (SDL_PenInputFlags)src->pen_axis.pen_state;
      out_event->paxis.x = src->pen_axis.x;
      out_event->paxis.y = src->pen_axis.y;
      out_event->paxis.axis = (SDL_PenAxis)src->pen_axis.axis;
      out_event->paxis.value = src->pen_axis.value;
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
      out_event->drop.windowID = (SDL_WindowID)src->drop.window_id;
      out_event->drop.x = src->drop.x;
      out_event->drop.y = src->drop.y;
      out_event->drop.source = src->drop.source;
      out_event->drop.data = src->drop.data;
      return 1;

    case SDL_EVENT_CLIPBOARD_UPDATE:
      out_event->clipboard.type = (SDL_EventType)src->type;
      out_event->clipboard.timestamp = (Uint64)src->timestamp;
      out_event->clipboard.owner = src->clipboard.owner != 0;
      out_event->clipboard.num_mime_types = (Sint32)src->clipboard.num_mime_types;
      out_event->clipboard.mime_types = (const char**)src->clipboard.mime_types;
      return 1;

    case SDL_EVENT_SENSOR_UPDATE:
      out_event->sensor.type = (SDL_EventType)src->type;
      out_event->sensor.timestamp = (Uint64)src->timestamp;
      out_event->sensor.which = (SDL_SensorID)src->sensor.sensor_id;
      for (sz index = 0; index < count_of(src->sensor.data); index += 1) {
        out_event->sensor.data[index] = src->sensor.data[index];
      }
      out_event->sensor.sensor_timestamp = (Uint64)src->sensor.sensor_timestamp;
      return 1;

    default:
      out_event->common.type = (Uint32)src->type;
      out_event->common.timestamp = (Uint64)src->timestamp;
      return 1;
  }
}

func void msg_pump(void) {
  // Pumps the process-global queue.
  SDL_PumpEvents();
  msg_refresh_synthetic_device_messages();
}

func b32 msg_poll(msg* out_msg) {
  SDL_Event native_event;

  msg_refresh_synthetic_device_messages();
  if (!out_msg || !SDL_PollEvent(&native_event) || !msg_from_sdl(&native_event, out_msg)) {
    return 0;
  }

  msg_notify_internal_listeners(out_msg);
  return 1;
}

func b32 msg_wait(msg* out_msg) {
  SDL_Event native_event;

  if (!out_msg) {
    return 0;
  }

  msg_refresh_synthetic_device_messages();
  if (SDL_PollEvent(&native_event) && msg_from_sdl(&native_event, out_msg)) {
    msg_notify_internal_listeners(out_msg);
    return 1;
  }

  if (!SDL_WaitEvent(&native_event) || !msg_from_sdl(&native_event, out_msg)) {
    return 0;
  }

  msg_notify_internal_listeners(out_msg);
  return 1;
}

func b32 msg_wait_timeout(msg* out_msg, i32 timeout_ms) {
  SDL_Event native_event;

  if (!out_msg) {
    return 0;
  }

  msg_refresh_synthetic_device_messages();
  if (SDL_PollEvent(&native_event) && msg_from_sdl(&native_event, out_msg)) {
    msg_notify_internal_listeners(out_msg);
    return 1;
  }

  if (!SDL_WaitEventTimeout(&native_event, (Sint32)timeout_ms) || !msg_from_sdl(&native_event, out_msg)) {
    return 0;
  }

  msg_notify_internal_listeners(out_msg);
  return 1;
}

func b32 _msg_post(const msg* src, callsite site) {
  SDL_Event native_event;
  msg posted_msg;
  msg* payload = NULL;

  if (!src) {
    return 0;
  }

  posted_msg = *src;
  posted_msg.post_site = site;

  if (!msg_dispatch_handlers(&posted_msg, MSG_HANDLER_STAGE_BEFORE_POST)) {
    return 0;
  }

  if (posted_msg.type >= MSG_TYPE_USER) {
    payload = (msg*)SDL_malloc(sizeof(*payload));
    if (!payload) {
      return 0;
    }
    *payload = posted_msg;

    native_event = (SDL_Event) {0};
    native_event.user.type = posted_msg.type;
    native_event.user.timestamp = (Uint64)posted_msg.timestamp;
    native_event.user.windowID = (SDL_WindowID)posted_msg.custom.window_id;
    native_event.user.code = (Sint32)MSG_PAYLOAD_CODE;
    native_event.user.data1 = payload;
    native_event.user.data2 = NULL;
  } else if (!msg_to_sdl_event(&posted_msg, &native_event)) {
    return 0;
  }

  // Push into SDL's process-global queue.
  if (!SDL_PushEvent(&native_event)) {
    if (payload != NULL) {
      SDL_free(payload);
    }
    (void)msg_dispatch_handlers(&posted_msg, MSG_HANDLER_STAGE_POST_FAILED);
    return 0;
  }

  (void)msg_dispatch_handlers(&posted_msg, MSG_HANDLER_STAGE_AFTER_POST);
  msg_notify_internal_listeners(&posted_msg);
  return 1;
}

func u32 msg_register_custom_range(sz count) {
  if (!count || count > (sz)I32_MAX) {
    return 0;
  }

  return (u32)SDL_RegisterEvents((int)count);
}

func u64 msg_add_handler(const msg_handler_desc* desc) {
  if (desc == NULL || desc->handler_fn == NULL || msg_handler_count >= MSG_HANDLER_CAP) {
    return 0;
  }

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
  return handler_id;
}

func b32 msg_remove_handler(u64 handler_id) {
  if (handler_id == 0) {
    return 0;
  }

  for (u32 index = 0; index < msg_handler_count; index += 1) {
    if (msg_handler_entries[index].handler_id != handler_id) {
      continue;
    }

    for (u32 move_index = index + 1; move_index < msg_handler_count; move_index += 1) {
      msg_handler_entries[move_index - 1] = msg_handler_entries[move_index];
    }

    msg_handler_count -= 1;
    msg_handler_entries[msg_handler_count] = (msg_handler_entry) {0};
    return 1;
  }

  return 0;
}

func void msg_clear_handlers(void) {
  for (u32 index = 0; index < msg_handler_count; index += 1) {
    msg_handler_entries[index] = (msg_handler_entry) {0};
  }

  msg_handler_count = 0;
  msg_handler_next_id = 1;
}

func b32 msg_from_native(const void* native_event, msg* out_msg) {
  return msg_from_sdl((const SDL_Event*)native_event, out_msg);
}

func b32 msg_to_native(const msg* src, void* native_event) {
  return msg_to_sdl_event(src, (SDL_Event*)native_event);
}
