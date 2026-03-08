// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/msg.h"
#include "input/msg_core.h"
#include "basic/profiler.h"

// =========================================================================
// Core Message Packing
// =========================================================================

func void msg_fill_core_raw(msg* src, u32 default_type, const void* core_data_ptr, sz core_data_size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (src == NULL || core_data_ptr == NULL || core_data_size > size_of(src->data)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  src->category = MSG_CATEGORY_CORE;
  if (src->type == MSG_CORE_TYPE_NONE) {
    src->type = default_type;
  }

  for (sz byte_idx = 0; byte_idx < size_of(src->data); byte_idx += 1) {
    src->data[byte_idx] = 0;
  }

  const u8* src_bytes = (const u8*)core_data_ptr;
  for (sz byte_idx = 0; byte_idx < core_data_size; byte_idx += 1) {
    src->data[byte_idx] = src_bytes[byte_idx];
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}

func b32 msg_core_is_valid(const msg* src, b32 matches_type) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return src != NULL && src->category == MSG_CATEGORY_CORE && matches_type;
}

#define MSG_CORE_DEFINE_MATCHER_RANGE(func_name, min_type, max_type) \
  func b32 func_name(u32 type) {                                     \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);                      \
    TracyCZoneEnd(__tracy_zone_ctx);                                 \
    return type >= (min_type) && type <= (max_type);                 \
  }

#define MSG_CORE_DEFINE_MATCHER_ONE(func_name, type_value) \
  func b32 func_name(u32 type) {                           \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);            \
    TracyCZoneEnd(__tracy_zone_ctx);                       \
    return type == (type_value);                           \
  }

#define MSG_CORE_DEFINE_MATCHER_TWO(func_name, type_a, type_b) \
  func b32 func_name(u32 type) {                               \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);                \
    TracyCZoneEnd(__tracy_zone_ctx);                           \
    return type == (type_a) || type == (type_b);               \
  }

#define MSG_CORE_DEFINE_MATCHER_THREE(func_name, type_a, type_b, type_c) \
  func b32 func_name(u32 type) {                                         \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);                          \
    TracyCZoneEnd(__tracy_zone_ctx);                                     \
    return type == (type_a) || type == (type_b) || type == (type_c);     \
  }

#define MSG_CORE_DEFINE_MATCHER_FIVE(func_name, type_a, type_b, type_c, type_d, type_e) \
  func b32 func_name(u32 type) {                                                        \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);                                         \
    TracyCZoneEnd(__tracy_zone_ctx);                                                    \
    return type == (type_a) || type == (type_b) || type == (type_c) ||                  \
           type == (type_d) || type == (type_e);                                        \
  }

MSG_CORE_DEFINE_MATCHER_RANGE(msg_core_type_is_monitor, MSG_CORE_TYPE_MONITOR_ORIENTATION, MSG_CORE_TYPE_MONITOR_CONTENT_SCALE_CHANGED)
MSG_CORE_DEFINE_MATCHER_RANGE(msg_core_type_is_window, MSG_CORE_TYPE_WINDOW_SHOWN, MSG_CORE_TYPE_WINDOW_HDR_STATE_CHANGED)
MSG_CORE_DEFINE_MATCHER_TWO(msg_core_type_is_keyboard_device, MSG_CORE_TYPE_KEYBOARD_ADDED, MSG_CORE_TYPE_KEYBOARD_REMOVED)
MSG_CORE_DEFINE_MATCHER_TWO(msg_core_type_is_keyboard, MSG_CORE_TYPE_KEY_DOWN, MSG_CORE_TYPE_KEY_UP)
MSG_CORE_DEFINE_MATCHER_ONE(msg_core_type_is_text_editing, MSG_CORE_TYPE_TEXT_EDITING)
MSG_CORE_DEFINE_MATCHER_ONE(msg_core_type_is_text_editing_candidates, MSG_CORE_TYPE_TEXT_EDITING_CANDIDATES)
MSG_CORE_DEFINE_MATCHER_ONE(msg_core_type_is_text_input, MSG_CORE_TYPE_TEXT_INPUT)
MSG_CORE_DEFINE_MATCHER_TWO(msg_core_type_is_mouse_device, MSG_CORE_TYPE_MOUSE_ADDED, MSG_CORE_TYPE_MOUSE_REMOVED)
MSG_CORE_DEFINE_MATCHER_ONE(msg_core_type_is_mouse_motion, MSG_CORE_TYPE_MOUSE_MOTION)
MSG_CORE_DEFINE_MATCHER_TWO(msg_core_type_is_mouse_button, MSG_CORE_TYPE_MOUSE_BUTTON_DOWN, MSG_CORE_TYPE_MOUSE_BUTTON_UP)
MSG_CORE_DEFINE_MATCHER_ONE(msg_core_type_is_mouse_wheel, MSG_CORE_TYPE_MOUSE_WHEEL)
MSG_CORE_DEFINE_MATCHER_THREE(msg_core_type_is_joystick_device, MSG_CORE_TYPE_JOYSTICK_ADDED, MSG_CORE_TYPE_JOYSTICK_REMOVED, MSG_CORE_TYPE_JOYSTICK_UPDATE_COMPLETE)
MSG_CORE_DEFINE_MATCHER_ONE(msg_core_type_is_joystick_axis, MSG_CORE_TYPE_JOYSTICK_AXIS_MOTION)
MSG_CORE_DEFINE_MATCHER_ONE(msg_core_type_is_joystick_ball, MSG_CORE_TYPE_JOYSTICK_BALL_MOTION)
MSG_CORE_DEFINE_MATCHER_ONE(msg_core_type_is_joystick_hat, MSG_CORE_TYPE_JOYSTICK_HAT_MOTION)
MSG_CORE_DEFINE_MATCHER_TWO(msg_core_type_is_joystick_button, MSG_CORE_TYPE_JOYSTICK_BUTTON_DOWN, MSG_CORE_TYPE_JOYSTICK_BUTTON_UP)
MSG_CORE_DEFINE_MATCHER_ONE(msg_core_type_is_joystick_battery, MSG_CORE_TYPE_JOYSTICK_BATTERY_UPDATED)
MSG_CORE_DEFINE_MATCHER_FIVE(msg_core_type_is_gamepad_device, MSG_CORE_TYPE_GAMEPAD_ADDED, MSG_CORE_TYPE_GAMEPAD_REMOVED, MSG_CORE_TYPE_GAMEPAD_REMAPPED, MSG_CORE_TYPE_GAMEPAD_UPDATE_COMPLETE, MSG_CORE_TYPE_GAMEPAD_STEAM_HANDLE_UPDATED)
MSG_CORE_DEFINE_MATCHER_ONE(msg_core_type_is_gamepad_axis, MSG_CORE_TYPE_GAMEPAD_AXIS_MOTION)
MSG_CORE_DEFINE_MATCHER_TWO(msg_core_type_is_gamepad_button, MSG_CORE_TYPE_GAMEPAD_BUTTON_DOWN, MSG_CORE_TYPE_GAMEPAD_BUTTON_UP)
MSG_CORE_DEFINE_MATCHER_RANGE(msg_core_type_is_gamepad_touchpad, MSG_CORE_TYPE_GAMEPAD_TOUCHPAD_DOWN, MSG_CORE_TYPE_GAMEPAD_TOUCHPAD_UP)
MSG_CORE_DEFINE_MATCHER_ONE(msg_core_type_is_gamepad_sensor, MSG_CORE_TYPE_GAMEPAD_SENSOR_UPDATE)
MSG_CORE_DEFINE_MATCHER_RANGE(msg_core_type_is_audio_device, MSG_CORE_TYPE_AUDIO_DEVICE_ADDED, MSG_CORE_TYPE_AUDIO_DEVICE_FORMAT_CHANGED)
MSG_CORE_DEFINE_MATCHER_RANGE(msg_core_type_is_camera_device, MSG_CORE_TYPE_CAMERA_DEVICE_ADDED, MSG_CORE_TYPE_CAMERA_DEVICE_DENIED)
MSG_CORE_DEFINE_MATCHER_RANGE(msg_core_type_is_render, MSG_CORE_TYPE_RENDER_TARGETS_RESET, MSG_CORE_TYPE_RENDER_DEVICE_LOST)
MSG_CORE_DEFINE_MATCHER_TWO(msg_core_type_is_touch_device, MSG_CORE_TYPE_TOUCH_ADDED, MSG_CORE_TYPE_TOUCH_REMOVED)
MSG_CORE_DEFINE_MATCHER_RANGE(msg_core_type_is_touch, MSG_CORE_TYPE_FINGER_DOWN, MSG_CORE_TYPE_FINGER_CANCELED)
MSG_CORE_DEFINE_MATCHER_TWO(msg_core_type_is_tablet_device, MSG_CORE_TYPE_TABLET_ADDED, MSG_CORE_TYPE_TABLET_REMOVED)
MSG_CORE_DEFINE_MATCHER_TWO(msg_core_type_is_pen_proximity, MSG_CORE_TYPE_PEN_PROXIMITY_IN, MSG_CORE_TYPE_PEN_PROXIMITY_OUT)
MSG_CORE_DEFINE_MATCHER_ONE(msg_core_type_is_pen_motion, MSG_CORE_TYPE_PEN_MOTION)
MSG_CORE_DEFINE_MATCHER_TWO(msg_core_type_is_pen_touch, MSG_CORE_TYPE_PEN_DOWN, MSG_CORE_TYPE_PEN_UP)
MSG_CORE_DEFINE_MATCHER_TWO(msg_core_type_is_pen_button, MSG_CORE_TYPE_PEN_BUTTON_DOWN, MSG_CORE_TYPE_PEN_BUTTON_UP)
MSG_CORE_DEFINE_MATCHER_ONE(msg_core_type_is_pen_axis, MSG_CORE_TYPE_PEN_AXIS)
MSG_CORE_DEFINE_MATCHER_RANGE(msg_core_type_is_drop, MSG_CORE_TYPE_DROP_FILE, MSG_CORE_TYPE_DROP_POSITION)
MSG_CORE_DEFINE_MATCHER_ONE(msg_core_type_is_clipboard, MSG_CORE_TYPE_CLIPBOARD_UPDATE)
MSG_CORE_DEFINE_MATCHER_ONE(msg_core_type_is_sensor, MSG_CORE_TYPE_SENSOR_UPDATE)
MSG_CORE_DEFINE_MATCHER_ONE(msg_core_type_is_object_lifecycle, MSG_CORE_TYPE_OBJECT_LIFECYCLE)
MSG_CORE_DEFINE_MATCHER_ONE(msg_core_type_is_thread_ctx, MSG_CORE_TYPE_THREAD_CTX)
MSG_CORE_DEFINE_MATCHER_ONE(msg_core_type_is_pathwatch, MSG_CORE_TYPE_PATHWATCH)
MSG_CORE_DEFINE_MATCHER_ONE(msg_core_type_is_log, MSG_CORE_TYPE_LOG)
MSG_CORE_DEFINE_MATCHER_ONE(msg_core_type_is_assert, MSG_CORE_TYPE_ASSERT)
MSG_CORE_DEFINE_MATCHER_ONE(msg_core_type_is_global_ctx, MSG_CORE_TYPE_GLOBAL_CTX)

// =========================================================================
// Typed Accessor Generation
// =========================================================================

#define MSG_CORE_DEFINE_ACCESSORS(data_type, fill_name, get_name, default_type_expr, matcher_name) \
  func void fill_name(msg* src, const data_type* core_data) {                                      \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);                                                    \
    if (core_data == NULL) {                                                                       \
      TracyCZoneEnd(__tracy_zone_ctx);                                                             \
      return;                                                                                      \
    }                                                                                              \
    msg_fill_core_raw(src, (default_type_expr), core_data, size_of(*core_data));                   \
    TracyCZoneEnd(__tracy_zone_ctx);                                                               \
  }                                                                                                \
                                                                                                   \
  func data_type* get_name(msg* src) {                                                             \
    TracyCZoneN(__tracy_zone_ctx, __func__, 1);                                                    \
    if (!msg_core_is_valid(src, matcher_name(src != NULL ? src->type : MSG_CORE_TYPE_NONE))) {     \
      TracyCZoneEnd(__tracy_zone_ctx);                                                             \
      return NULL;                                                                                 \
    }                                                                                              \
    TracyCZoneEnd(__tracy_zone_ctx);                                                               \
    return (data_type*)src->data;                                                                  \
  }

MSG_CORE_DEFINE_ACCESSORS(msg_core_monitor_data, msg_core_fill_monitor, msg_core_get_monitor, MSG_CORE_TYPE_MONITOR_ORIENTATION, msg_core_type_is_monitor)
MSG_CORE_DEFINE_ACCESSORS(msg_core_window_data, msg_core_fill_window, msg_core_get_window, MSG_CORE_TYPE_WINDOW_SHOWN, msg_core_type_is_window)
MSG_CORE_DEFINE_ACCESSORS(msg_core_keyboard_device_data, msg_core_fill_keyboard_device, msg_core_get_keyboard_device, MSG_CORE_TYPE_KEYBOARD_ADDED, msg_core_type_is_keyboard_device)
MSG_CORE_DEFINE_ACCESSORS(msg_core_keyboard_data, msg_core_fill_keyboard, msg_core_get_keyboard, core_data->down ? MSG_CORE_TYPE_KEY_DOWN : MSG_CORE_TYPE_KEY_UP, msg_core_type_is_keyboard)
MSG_CORE_DEFINE_ACCESSORS(msg_core_text_editing_data, msg_core_fill_text_editing, msg_core_get_text_editing, MSG_CORE_TYPE_TEXT_EDITING, msg_core_type_is_text_editing)
MSG_CORE_DEFINE_ACCESSORS(msg_core_text_editing_candidates_data, msg_core_fill_text_editing_candidates, msg_core_get_text_editing_candidates, MSG_CORE_TYPE_TEXT_EDITING_CANDIDATES, msg_core_type_is_text_editing_candidates)
MSG_CORE_DEFINE_ACCESSORS(msg_core_text_input_data, msg_core_fill_text_input, msg_core_get_text_input, MSG_CORE_TYPE_TEXT_INPUT, msg_core_type_is_text_input)
MSG_CORE_DEFINE_ACCESSORS(msg_core_mouse_device_data, msg_core_fill_mouse_device, msg_core_get_mouse_device, MSG_CORE_TYPE_MOUSE_ADDED, msg_core_type_is_mouse_device)
MSG_CORE_DEFINE_ACCESSORS(msg_core_mouse_motion_data, msg_core_fill_mouse_motion, msg_core_get_mouse_motion, MSG_CORE_TYPE_MOUSE_MOTION, msg_core_type_is_mouse_motion)
MSG_CORE_DEFINE_ACCESSORS(msg_core_mouse_button_data, msg_core_fill_mouse_button, msg_core_get_mouse_button, core_data->down ? MSG_CORE_TYPE_MOUSE_BUTTON_DOWN : MSG_CORE_TYPE_MOUSE_BUTTON_UP, msg_core_type_is_mouse_button)
MSG_CORE_DEFINE_ACCESSORS(msg_core_mouse_wheel_data, msg_core_fill_mouse_wheel, msg_core_get_mouse_wheel, MSG_CORE_TYPE_MOUSE_WHEEL, msg_core_type_is_mouse_wheel)
MSG_CORE_DEFINE_ACCESSORS(msg_core_joystick_device_data, msg_core_fill_joystick_device, msg_core_get_joystick_device, MSG_CORE_TYPE_JOYSTICK_ADDED, msg_core_type_is_joystick_device)
MSG_CORE_DEFINE_ACCESSORS(msg_core_joystick_axis_data, msg_core_fill_joystick_axis, msg_core_get_joystick_axis, MSG_CORE_TYPE_JOYSTICK_AXIS_MOTION, msg_core_type_is_joystick_axis)
MSG_CORE_DEFINE_ACCESSORS(msg_core_joystick_ball_data, msg_core_fill_joystick_ball, msg_core_get_joystick_ball, MSG_CORE_TYPE_JOYSTICK_BALL_MOTION, msg_core_type_is_joystick_ball)
MSG_CORE_DEFINE_ACCESSORS(msg_core_joystick_hat_data, msg_core_fill_joystick_hat, msg_core_get_joystick_hat, MSG_CORE_TYPE_JOYSTICK_HAT_MOTION, msg_core_type_is_joystick_hat)
MSG_CORE_DEFINE_ACCESSORS(msg_core_joystick_button_data, msg_core_fill_joystick_button, msg_core_get_joystick_button, core_data->down ? MSG_CORE_TYPE_JOYSTICK_BUTTON_DOWN : MSG_CORE_TYPE_JOYSTICK_BUTTON_UP, msg_core_type_is_joystick_button)
MSG_CORE_DEFINE_ACCESSORS(msg_core_joystick_battery_data, msg_core_fill_joystick_battery, msg_core_get_joystick_battery, MSG_CORE_TYPE_JOYSTICK_BATTERY_UPDATED, msg_core_type_is_joystick_battery)
MSG_CORE_DEFINE_ACCESSORS(msg_core_gamepad_device_data, msg_core_fill_gamepad_device, msg_core_get_gamepad_device, MSG_CORE_TYPE_GAMEPAD_ADDED, msg_core_type_is_gamepad_device)
MSG_CORE_DEFINE_ACCESSORS(msg_core_gamepad_axis_data, msg_core_fill_gamepad_axis, msg_core_get_gamepad_axis, MSG_CORE_TYPE_GAMEPAD_AXIS_MOTION, msg_core_type_is_gamepad_axis)
MSG_CORE_DEFINE_ACCESSORS(msg_core_gamepad_button_data, msg_core_fill_gamepad_button, msg_core_get_gamepad_button, core_data->down ? MSG_CORE_TYPE_GAMEPAD_BUTTON_DOWN : MSG_CORE_TYPE_GAMEPAD_BUTTON_UP, msg_core_type_is_gamepad_button)
MSG_CORE_DEFINE_ACCESSORS(msg_core_gamepad_touchpad_data, msg_core_fill_gamepad_touchpad, msg_core_get_gamepad_touchpad, MSG_CORE_TYPE_GAMEPAD_TOUCHPAD_MOTION, msg_core_type_is_gamepad_touchpad)
MSG_CORE_DEFINE_ACCESSORS(msg_core_gamepad_sensor_data, msg_core_fill_gamepad_sensor, msg_core_get_gamepad_sensor, MSG_CORE_TYPE_GAMEPAD_SENSOR_UPDATE, msg_core_type_is_gamepad_sensor)
MSG_CORE_DEFINE_ACCESSORS(msg_core_audio_device_data, msg_core_fill_audio_device, msg_core_get_audio_device, MSG_CORE_TYPE_AUDIO_DEVICE_ADDED, msg_core_type_is_audio_device)
MSG_CORE_DEFINE_ACCESSORS(msg_core_camera_device_data, msg_core_fill_camera_device, msg_core_get_camera_device, MSG_CORE_TYPE_CAMERA_DEVICE_ADDED, msg_core_type_is_camera_device)
MSG_CORE_DEFINE_ACCESSORS(msg_core_render_data, msg_core_fill_render, msg_core_get_render, MSG_CORE_TYPE_RENDER_TARGETS_RESET, msg_core_type_is_render)
MSG_CORE_DEFINE_ACCESSORS(msg_core_touch_device_data, msg_core_fill_touch_device, msg_core_get_touch_device, MSG_CORE_TYPE_TOUCH_ADDED, msg_core_type_is_touch_device)
MSG_CORE_DEFINE_ACCESSORS(msg_core_touch_data, msg_core_fill_touch, msg_core_get_touch, MSG_CORE_TYPE_FINGER_MOTION, msg_core_type_is_touch)
MSG_CORE_DEFINE_ACCESSORS(msg_core_tablet_device_data, msg_core_fill_tablet_device, msg_core_get_tablet_device, MSG_CORE_TYPE_TABLET_ADDED, msg_core_type_is_tablet_device)
MSG_CORE_DEFINE_ACCESSORS(msg_core_pen_proximity_data, msg_core_fill_pen_proximity, msg_core_get_pen_proximity, MSG_CORE_TYPE_PEN_PROXIMITY_IN, msg_core_type_is_pen_proximity)
MSG_CORE_DEFINE_ACCESSORS(msg_core_pen_motion_data, msg_core_fill_pen_motion, msg_core_get_pen_motion, MSG_CORE_TYPE_PEN_MOTION, msg_core_type_is_pen_motion)
MSG_CORE_DEFINE_ACCESSORS(msg_core_pen_touch_data, msg_core_fill_pen_touch, msg_core_get_pen_touch, core_data->down ? MSG_CORE_TYPE_PEN_DOWN : MSG_CORE_TYPE_PEN_UP, msg_core_type_is_pen_touch)
MSG_CORE_DEFINE_ACCESSORS(msg_core_pen_button_data, msg_core_fill_pen_button, msg_core_get_pen_button, core_data->down ? MSG_CORE_TYPE_PEN_BUTTON_DOWN : MSG_CORE_TYPE_PEN_BUTTON_UP, msg_core_type_is_pen_button)
MSG_CORE_DEFINE_ACCESSORS(msg_core_pen_axis_data, msg_core_fill_pen_axis, msg_core_get_pen_axis, MSG_CORE_TYPE_PEN_AXIS, msg_core_type_is_pen_axis)
MSG_CORE_DEFINE_ACCESSORS(msg_core_drop_data, msg_core_fill_drop, msg_core_get_drop, MSG_CORE_TYPE_DROP_FILE, msg_core_type_is_drop)
MSG_CORE_DEFINE_ACCESSORS(msg_core_clipboard_data, msg_core_fill_clipboard, msg_core_get_clipboard, MSG_CORE_TYPE_CLIPBOARD_UPDATE, msg_core_type_is_clipboard)
MSG_CORE_DEFINE_ACCESSORS(msg_core_sensor_data, msg_core_fill_sensor, msg_core_get_sensor, MSG_CORE_TYPE_SENSOR_UPDATE, msg_core_type_is_sensor)
MSG_CORE_DEFINE_ACCESSORS(msg_core_object_lifecycle_data, msg_core_fill_object_lifecycle, msg_core_get_object_lifecycle, MSG_CORE_TYPE_OBJECT_LIFECYCLE, msg_core_type_is_object_lifecycle)
MSG_CORE_DEFINE_ACCESSORS(msg_core_thread_ctx_data, msg_core_fill_thread_ctx, msg_core_get_thread_ctx, MSG_CORE_TYPE_THREAD_CTX, msg_core_type_is_thread_ctx)
MSG_CORE_DEFINE_ACCESSORS(msg_core_pathwatch_data, msg_core_fill_pathwatch, msg_core_get_pathwatch, MSG_CORE_TYPE_PATHWATCH, msg_core_type_is_pathwatch)
MSG_CORE_DEFINE_ACCESSORS(msg_core_log_data, msg_core_fill_log, msg_core_get_log, MSG_CORE_TYPE_LOG, msg_core_type_is_log)
MSG_CORE_DEFINE_ACCESSORS(msg_core_assert_data, msg_core_fill_assert, msg_core_get_assert, MSG_CORE_TYPE_ASSERT, msg_core_type_is_assert)
MSG_CORE_DEFINE_ACCESSORS(msg_core_global_ctx_data, msg_core_fill_global_ctx, msg_core_get_global_ctx, MSG_CORE_TYPE_GLOBAL_CTX, msg_core_type_is_global_ctx)
