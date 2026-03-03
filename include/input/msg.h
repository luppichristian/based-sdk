// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// =========================================================================
// Input Messages
// =========================================================================

// Event types mirrored from the backend event system.
typedef enum msg_type {
  MSG_TYPE_NONE = 0,
  MSG_TYPE_QUIT = 0x100,
  MSG_TYPE_TERMINATING = 0x101,
  MSG_TYPE_LOW_MEMORY = 0x102,
  MSG_TYPE_WILL_ENTER_BACKGROUND = 0x103,
  MSG_TYPE_DID_ENTER_BACKGROUND = 0x104,
  MSG_TYPE_WILL_ENTER_FOREGROUND = 0x105,
  MSG_TYPE_DID_ENTER_FOREGROUND = 0x106,
  MSG_TYPE_LOCALE_CHANGED = 0x107,
  MSG_TYPE_SYSTEM_THEME_CHANGED = 0x108,
  MSG_TYPE_DISPLAY_ORIENTATION = 0x151,
  MSG_TYPE_DISPLAY_ADDED = 0x152,
  MSG_TYPE_DISPLAY_REMOVED = 0x153,
  MSG_TYPE_DISPLAY_MOVED = 0x154,
  MSG_TYPE_DISPLAY_DESKTOP_MODE_CHANGED = 0x155,
  MSG_TYPE_DISPLAY_CURRENT_MODE_CHANGED = 0x156,
  MSG_TYPE_DISPLAY_CONTENT_SCALE_CHANGED = 0x157,
  MSG_TYPE_WINDOW_SHOWN = 0x202,
  MSG_TYPE_WINDOW_HIDDEN = 0x203,
  MSG_TYPE_WINDOW_EXPOSED = 0x204,
  MSG_TYPE_WINDOW_MOVED = 0x205,
  MSG_TYPE_WINDOW_RESIZED = 0x206,
  MSG_TYPE_WINDOW_PIXEL_SIZE_CHANGED = 0x207,
  MSG_TYPE_WINDOW_METAL_VIEW_RESIZED = 0x208,
  MSG_TYPE_WINDOW_MINIMIZED = 0x209,
  MSG_TYPE_WINDOW_MAXIMIZED = 0x20A,
  MSG_TYPE_WINDOW_RESTORED = 0x20B,
  MSG_TYPE_WINDOW_MOUSE_ENTER = 0x20C,
  MSG_TYPE_WINDOW_MOUSE_LEAVE = 0x20D,
  MSG_TYPE_WINDOW_FOCUS_GAINED = 0x20E,
  MSG_TYPE_WINDOW_FOCUS_LOST = 0x20F,
  MSG_TYPE_WINDOW_CLOSE_REQUESTED = 0x210,
  MSG_TYPE_WINDOW_HIT_TEST = 0x211,
  MSG_TYPE_WINDOW_ICCPROF_CHANGED = 0x212,
  MSG_TYPE_WINDOW_DISPLAY_CHANGED = 0x213,
  MSG_TYPE_WINDOW_DISPLAY_SCALE_CHANGED = 0x214,
  MSG_TYPE_WINDOW_SAFE_AREA_CHANGED = 0x215,
  MSG_TYPE_WINDOW_OCCLUDED = 0x216,
  MSG_TYPE_WINDOW_ENTER_FULLSCREEN = 0x217,
  MSG_TYPE_WINDOW_LEAVE_FULLSCREEN = 0x218,
  MSG_TYPE_WINDOW_DESTROYED = 0x219,
  MSG_TYPE_WINDOW_HDR_STATE_CHANGED = 0x21A,
  MSG_TYPE_KEY_DOWN = 0x300,
  MSG_TYPE_KEY_UP = 0x301,
  MSG_TYPE_TEXT_EDITING = 0x302,
  MSG_TYPE_TEXT_INPUT = 0x303,
  MSG_TYPE_KEYMAP_CHANGED = 0x304,
  MSG_TYPE_KEYBOARD_ADDED = 0x305,
  MSG_TYPE_KEYBOARD_REMOVED = 0x306,
  MSG_TYPE_TEXT_EDITING_CANDIDATES = 0x307,
  MSG_TYPE_MOUSE_MOTION = 0x400,
  MSG_TYPE_MOUSE_BUTTON_DOWN = 0x401,
  MSG_TYPE_MOUSE_BUTTON_UP = 0x402,
  MSG_TYPE_MOUSE_WHEEL = 0x403,
  MSG_TYPE_MOUSE_ADDED = 0x404,
  MSG_TYPE_MOUSE_REMOVED = 0x405,
  MSG_TYPE_JOYSTICK_AXIS_MOTION = 0x600,
  MSG_TYPE_JOYSTICK_BALL_MOTION = 0x601,
  MSG_TYPE_JOYSTICK_HAT_MOTION = 0x602,
  MSG_TYPE_JOYSTICK_BUTTON_DOWN = 0x603,
  MSG_TYPE_JOYSTICK_BUTTON_UP = 0x604,
  MSG_TYPE_JOYSTICK_ADDED = 0x605,
  MSG_TYPE_JOYSTICK_REMOVED = 0x606,
  MSG_TYPE_JOYSTICK_BATTERY_UPDATED = 0x607,
  MSG_TYPE_JOYSTICK_UPDATE_COMPLETE = 0x608,
  MSG_TYPE_GAMEPAD_AXIS_MOTION = 0x650,
  MSG_TYPE_GAMEPAD_BUTTON_DOWN = 0x651,
  MSG_TYPE_GAMEPAD_BUTTON_UP = 0x652,
  MSG_TYPE_GAMEPAD_ADDED = 0x653,
  MSG_TYPE_GAMEPAD_REMOVED = 0x654,
  MSG_TYPE_GAMEPAD_REMAPPED = 0x655,
  MSG_TYPE_GAMEPAD_TOUCHPAD_DOWN = 0x656,
  MSG_TYPE_GAMEPAD_TOUCHPAD_MOTION = 0x657,
  MSG_TYPE_GAMEPAD_TOUCHPAD_UP = 0x658,
  MSG_TYPE_GAMEPAD_SENSOR_UPDATE = 0x659,
  MSG_TYPE_GAMEPAD_UPDATE_COMPLETE = 0x65A,
  MSG_TYPE_GAMEPAD_STEAM_HANDLE_UPDATED = 0x65B,
  MSG_TYPE_FINGER_DOWN = 0x700,
  MSG_TYPE_FINGER_UP = 0x701,
  MSG_TYPE_FINGER_MOTION = 0x702,
  MSG_TYPE_FINGER_CANCELED = 0x703,
  MSG_TYPE_CLIPBOARD_UPDATE = 0x900,
  MSG_TYPE_DROP_FILE = 0x1000,
  MSG_TYPE_DROP_TEXT = 0x1001,
  MSG_TYPE_DROP_BEGIN = 0x1002,
  MSG_TYPE_DROP_COMPLETE = 0x1003,
  MSG_TYPE_DROP_POSITION = 0x1004,
  MSG_TYPE_AUDIO_DEVICE_ADDED = 0x1100,
  MSG_TYPE_AUDIO_DEVICE_REMOVED = 0x1101,
  MSG_TYPE_AUDIO_DEVICE_FORMAT_CHANGED = 0x1102,
  MSG_TYPE_SENSOR_UPDATE = 0x1200,
  MSG_TYPE_PEN_PROXIMITY_IN = 0x1300,
  MSG_TYPE_PEN_PROXIMITY_OUT = 0x1301,
  MSG_TYPE_PEN_DOWN = 0x1302,
  MSG_TYPE_PEN_UP = 0x1303,
  MSG_TYPE_PEN_BUTTON_DOWN = 0x1304,
  MSG_TYPE_PEN_BUTTON_UP = 0x1305,
  MSG_TYPE_PEN_MOTION = 0x1306,
  MSG_TYPE_PEN_AXIS = 0x1307,
  MSG_TYPE_CAMERA_DEVICE_ADDED = 0x1400,
  MSG_TYPE_CAMERA_DEVICE_REMOVED = 0x1401,
  MSG_TYPE_CAMERA_DEVICE_APPROVED = 0x1402,
  MSG_TYPE_CAMERA_DEVICE_DENIED = 0x1403,
  MSG_TYPE_RENDER_TARGETS_RESET = 0x2000,
  MSG_TYPE_RENDER_DEVICE_RESET = 0x2001,
  MSG_TYPE_RENDER_DEVICE_LOST = 0x2002,
  MSG_TYPE_USER = 0x8000,
} msg_type;

// Display event payload.
typedef struct msg_display_data {
  u32 display_id;
  i32 data1;
  i32 data2;
} msg_display_data;

// Window event payload.
typedef struct msg_window_data {
  u32 window_id;
  i32 data1;
  i32 data2;
} msg_window_data;

// Keyboard device connection payload.
typedef struct msg_keyboard_device_data {
  u32 keyboard_id;
} msg_keyboard_device_data;

// Key press and release payload.
typedef struct msg_keyboard_data {
  u32 window_id;
  u32 keyboard_id;
  u32 scancode;
  i32 keycode;
  u16 modifiers;
  u16 raw;
  b32 down;
  b32 repeat;
} msg_keyboard_data;

// IME editing payload.
typedef struct msg_text_editing_data {
  u32 window_id;
  const c8* text;
  i32 start;
  i32 length;
} msg_text_editing_data;

// IME candidate list payload.
typedef struct msg_text_editing_candidates_data {
  u32 window_id;
  const c8* const* candidates;
  i32 num_candidates;
  i32 selected_candidate;
  b32 horizontal;
} msg_text_editing_candidates_data;

// Text input payload.
typedef struct msg_text_input_data {
  u32 window_id;
  const c8* text;
} msg_text_input_data;

// Mouse device connection payload.
typedef struct msg_mouse_device_data {
  u32 mouse_id;
} msg_mouse_device_data;

// Mouse motion payload.
typedef struct msg_mouse_motion_data {
  u32 window_id;
  u32 mouse_id;
  u32 button_mask;
  f32 x;
  f32 y;
  f32 xrel;
  f32 yrel;
} msg_mouse_motion_data;

// Mouse button payload.
typedef struct msg_mouse_button_data {
  u32 window_id;
  u32 mouse_id;
  u8 button;
  b32 down;
  u8 clicks;
  f32 x;
  f32 y;
} msg_mouse_button_data;

// Mouse wheel payload.
typedef struct msg_mouse_wheel_data {
  u32 window_id;
  u32 mouse_id;
  f32 x;
  f32 y;
  u32 direction;
  f32 mouse_x;
  f32 mouse_y;
} msg_mouse_wheel_data;

// Joystick device connection payload.
typedef struct msg_joystick_device_data {
  u32 joystick_id;
} msg_joystick_device_data;

// Joystick axis payload.
typedef struct msg_joystick_axis_data {
  u32 joystick_id;
  u8 axis;
  i16 value;
} msg_joystick_axis_data;

// Joystick trackball payload.
typedef struct msg_joystick_ball_data {
  u32 joystick_id;
  u8 ball;
  i16 xrel;
  i16 yrel;
} msg_joystick_ball_data;

// Joystick hat payload.
typedef struct msg_joystick_hat_data {
  u32 joystick_id;
  u8 hat;
  u8 value;
} msg_joystick_hat_data;

// Joystick button payload.
typedef struct msg_joystick_button_data {
  u32 joystick_id;
  u8 button;
  b32 down;
} msg_joystick_button_data;

// Joystick battery payload.
typedef struct msg_joystick_battery_data {
  u32 joystick_id;
  i32 state;
  i32 percent;
} msg_joystick_battery_data;

// Gamepad device connection payload.
typedef struct msg_gamepad_device_data {
  u32 gamepad_id;
} msg_gamepad_device_data;

// Gamepad axis payload.
typedef struct msg_gamepad_axis_data {
  u32 gamepad_id;
  u8 axis;
  i16 value;
} msg_gamepad_axis_data;

// Gamepad button payload.
typedef struct msg_gamepad_button_data {
  u32 gamepad_id;
  u8 button;
  b32 down;
} msg_gamepad_button_data;

// Gamepad touchpad payload.
typedef struct msg_gamepad_touchpad_data {
  u32 gamepad_id;
  i32 touchpad;
  i32 finger;
  f32 x;
  f32 y;
  f32 pressure;
} msg_gamepad_touchpad_data;

// Gamepad sensor payload.
typedef struct msg_gamepad_sensor_data {
  u32 gamepad_id;
  i32 sensor;
  f32 data[3];
  u64 sensor_timestamp;
} msg_gamepad_sensor_data;

// Audio device payload.
typedef struct msg_audio_device_data {
  u32 audio_device_id;
  b32 recording;
} msg_audio_device_data;

// Camera device payload.
typedef struct msg_camera_device_data {
  u32 camera_id;
} msg_camera_device_data;

// Render reset payload.
typedef struct msg_render_data {
  u32 window_id;
} msg_render_data;

// Touch event payload.
typedef struct msg_touch_data {
  u64 touch_id;
  u64 finger_id;
  f32 x;
  f32 y;
  f32 dx;
  f32 dy;
  f32 pressure;
  u32 window_id;
} msg_touch_data;

// Pen proximity payload.
typedef struct msg_pen_proximity_data {
  u32 window_id;
  u32 pen_id;
} msg_pen_proximity_data;

// Pen motion payload.
typedef struct msg_pen_motion_data {
  u32 window_id;
  u32 pen_id;
  u32 pen_state;
  f32 x;
  f32 y;
} msg_pen_motion_data;

// Pen touch payload.
typedef struct msg_pen_touch_data {
  u32 window_id;
  u32 pen_id;
  u32 pen_state;
  f32 x;
  f32 y;
  b32 eraser;
  b32 down;
} msg_pen_touch_data;

// Pen button payload.
typedef struct msg_pen_button_data {
  u32 window_id;
  u32 pen_id;
  u32 pen_state;
  f32 x;
  f32 y;
  u8 button;
  b32 down;
} msg_pen_button_data;

// Pen axis payload.
typedef struct msg_pen_axis_data {
  u32 window_id;
  u32 pen_id;
  u32 pen_state;
  f32 x;
  f32 y;
  u32 axis;
  f32 value;
} msg_pen_axis_data;

// Drag-and-drop payload.
typedef struct msg_drop_data {
  u32 window_id;
  f32 x;
  f32 y;
  const c8* source;
  const c8* data;
} msg_drop_data;

// Clipboard update payload.
typedef struct msg_clipboard_data {
  b32 owner;
  i32 num_mime_types;
  const c8* const* mime_types;
} msg_clipboard_data;

// Generic sensor payload.
typedef struct msg_sensor_data {
  u32 sensor_id;
  f32 data[6];
  u64 sensor_timestamp;
} msg_sensor_data;

// User-posted custom payload.
typedef struct msg_custom_data {
  u32 window_id;
  i32 code;
  void* data1;
  void* data2;
} msg_custom_data;

// Normalized event record used by the input message queue.
typedef struct msg {
  u32 type;
  u64 timestamp;
  union {
    msg_display_data display;
    msg_window_data window;
    msg_keyboard_device_data keyboard_device;
    msg_keyboard_data keyboard;
    msg_text_editing_data text_editing;
    msg_text_editing_candidates_data text_editing_candidates;
    msg_text_input_data text_input;
    msg_mouse_device_data mouse_device;
    msg_mouse_motion_data mouse_motion;
    msg_mouse_button_data mouse_button;
    msg_mouse_wheel_data mouse_wheel;
    msg_joystick_device_data joystick_device;
    msg_joystick_axis_data joystick_axis;
    msg_joystick_ball_data joystick_ball;
    msg_joystick_hat_data joystick_hat;
    msg_joystick_button_data joystick_button;
    msg_joystick_battery_data joystick_battery;
    msg_gamepad_device_data gamepad_device;
    msg_gamepad_axis_data gamepad_axis;
    msg_gamepad_button_data gamepad_button;
    msg_gamepad_touchpad_data gamepad_touchpad;
    msg_gamepad_sensor_data gamepad_sensor;
    msg_audio_device_data audio_device;
    msg_camera_device_data camera_device;
    msg_render_data render;
    msg_touch_data touch;
    msg_pen_proximity_data pen_proximity;
    msg_pen_motion_data pen_motion;
    msg_pen_touch_data pen_touch;
    msg_pen_button_data pen_button;
    msg_pen_axis_data pen_axis;
    msg_drop_data drop;
    msg_clipboard_data clipboard;
    msg_sensor_data sensor;
    msg_custom_data custom;
  };
} msg;

// Callback invoked after a message is posted through msg_post.
typedef void (*msg_post_callback)(const msg* src, void* user_data);

// Processes backend events and refreshes the internal queue.
func void msg_pump(void);

// Pops the next queued message into out_msg. Returns 1 on success, 0 otherwise.
func b32 msg_poll(msg* out_msg);

// Waits indefinitely for the next message and writes it to out_msg.
func b32 msg_wait(msg* out_msg);

// Waits up to timeout_ms for the next message and writes it to out_msg.
func b32 msg_wait_timeout(msg* out_msg, i32 timeout_ms);

// Posts src into the backend queue. Returns 1 on success, 0 otherwise.
func b32 msg_post(const msg* src);

// Reserves count consecutive custom event types and returns the first type value.
func u32 msg_register_custom_range(sz count);

// Registers a callback invoked after successful msg_post calls.
func void msg_set_post_callback(msg_post_callback callback, void* user_data);

// Converts a backend-native event object into a msg. Returns 1 on success, 0 otherwise.
func b32 msg_from_native(const void* native_event, msg* out_msg);

// Converts src into a backend-native event object. Returns 1 on success, 0 otherwise.
func b32 msg_to_native(const msg* src, void* native_event);
