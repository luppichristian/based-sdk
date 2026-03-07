// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/assert.h"
#include "../basic/codespace.h"
#include "../basic/log.h"
#include "../basic/primitive_types.h"
#include "../filesystem/pathwatch.h"
#include "../strings/cstrings.h"
#include "devices.h"
#include "gamepads.h"
#include "mouse.h"
#include "tablet.h"

typedef struct msg msg;
typedef struct ctx ctx;
typedef struct global_ctx global_ctx;

// Event types mirrored from the backend event system.
typedef enum msg_core_type {
  MSG_CORE_TYPE_NONE = 0,
  MSG_CORE_TYPE_QUIT = 0x100,
  MSG_CORE_TYPE_TERMINATING = 0x101,
  MSG_CORE_TYPE_LOW_MEMORY = 0x102,
  MSG_CORE_TYPE_WILL_ENTER_BACKGROUND = 0x103,
  MSG_CORE_TYPE_DID_ENTER_BACKGROUND = 0x104,
  MSG_CORE_TYPE_WILL_ENTER_FOREGROUND = 0x105,
  MSG_CORE_TYPE_DID_ENTER_FOREGROUND = 0x106,
  MSG_CORE_TYPE_LOCALE_CHANGED = 0x107,
  MSG_CORE_TYPE_SYSTEM_THEME_CHANGED = 0x108,
  MSG_CORE_TYPE_DISPLAY_ORIENTATION = 0x151,
  MSG_CORE_TYPE_DISPLAY_ADDED = 0x152,
  MSG_CORE_TYPE_DISPLAY_REMOVED = 0x153,
  MSG_CORE_TYPE_DISPLAY_MOVED = 0x154,
  MSG_CORE_TYPE_DISPLAY_DESKTOP_MODE_CHANGED = 0x155,
  MSG_CORE_TYPE_DISPLAY_CURRENT_MODE_CHANGED = 0x156,
  MSG_CORE_TYPE_DISPLAY_CONTENT_SCALE_CHANGED = 0x157,
  MSG_CORE_TYPE_WINDOW_SHOWN = 0x202,
  MSG_CORE_TYPE_WINDOW_HIDDEN = 0x203,
  MSG_CORE_TYPE_WINDOW_EXPOSED = 0x204,
  MSG_CORE_TYPE_WINDOW_MOVED = 0x205,
  MSG_CORE_TYPE_WINDOW_RESIZED = 0x206,
  MSG_CORE_TYPE_WINDOW_PIXEL_SIZE_CHANGED = 0x207,
  MSG_CORE_TYPE_WINDOW_METAL_VIEW_RESIZED = 0x208,
  MSG_CORE_TYPE_WINDOW_MINIMIZED = 0x209,
  MSG_CORE_TYPE_WINDOW_MAXIMIZED = 0x20A,
  MSG_CORE_TYPE_WINDOW_RESTORED = 0x20B,
  MSG_CORE_TYPE_WINDOW_MOUSE_ENTER = 0x20C,
  MSG_CORE_TYPE_WINDOW_MOUSE_LEAVE = 0x20D,
  MSG_CORE_TYPE_WINDOW_FOCUS_GAINED = 0x20E,
  MSG_CORE_TYPE_WINDOW_FOCUS_LOST = 0x20F,
  MSG_CORE_TYPE_WINDOW_CLOSE_REQUESTED = 0x210,
  MSG_CORE_TYPE_WINDOW_HIT_TEST = 0x211,
  MSG_CORE_TYPE_WINDOW_ICCPROF_CHANGED = 0x212,
  MSG_CORE_TYPE_WINDOW_DISPLAY_CHANGED = 0x213,
  MSG_CORE_TYPE_WINDOW_DISPLAY_SCALE_CHANGED = 0x214,
  MSG_CORE_TYPE_WINDOW_SAFE_AREA_CHANGED = 0x215,
  MSG_CORE_TYPE_WINDOW_OCCLUDED = 0x216,
  MSG_CORE_TYPE_WINDOW_ENTER_FULLSCREEN = 0x217,
  MSG_CORE_TYPE_WINDOW_LEAVE_FULLSCREEN = 0x218,
  MSG_CORE_TYPE_WINDOW_DESTROYED = 0x219,
  MSG_CORE_TYPE_WINDOW_HDR_STATE_CHANGED = 0x21A,
  MSG_CORE_TYPE_KEY_DOWN = 0x300,
  MSG_CORE_TYPE_KEY_UP = 0x301,
  MSG_CORE_TYPE_TEXT_EDITING = 0x302,
  MSG_CORE_TYPE_TEXT_INPUT = 0x303,
  MSG_CORE_TYPE_KEYMAP_CHANGED = 0x304,
  MSG_CORE_TYPE_KEYBOARD_ADDED = 0x305,
  MSG_CORE_TYPE_KEYBOARD_REMOVED = 0x306,
  MSG_CORE_TYPE_TEXT_EDITING_CANDIDATES = 0x307,
  MSG_CORE_TYPE_MOUSE_MOTION = 0x400,
  MSG_CORE_TYPE_MOUSE_BUTTON_DOWN = 0x401,
  MSG_CORE_TYPE_MOUSE_BUTTON_UP = 0x402,
  MSG_CORE_TYPE_MOUSE_WHEEL = 0x403,
  MSG_CORE_TYPE_MOUSE_ADDED = 0x404,
  MSG_CORE_TYPE_MOUSE_REMOVED = 0x405,
  MSG_CORE_TYPE_JOYSTICK_AXIS_MOTION = 0x600,
  MSG_CORE_TYPE_JOYSTICK_BALL_MOTION = 0x601,
  MSG_CORE_TYPE_JOYSTICK_HAT_MOTION = 0x602,
  MSG_CORE_TYPE_JOYSTICK_BUTTON_DOWN = 0x603,
  MSG_CORE_TYPE_JOYSTICK_BUTTON_UP = 0x604,
  MSG_CORE_TYPE_JOYSTICK_ADDED = 0x605,
  MSG_CORE_TYPE_JOYSTICK_REMOVED = 0x606,
  MSG_CORE_TYPE_JOYSTICK_BATTERY_UPDATED = 0x607,
  MSG_CORE_TYPE_JOYSTICK_UPDATE_COMPLETE = 0x608,
  MSG_CORE_TYPE_GAMEPAD_AXIS_MOTION = 0x650,
  MSG_CORE_TYPE_GAMEPAD_BUTTON_DOWN = 0x651,
  MSG_CORE_TYPE_GAMEPAD_BUTTON_UP = 0x652,
  MSG_CORE_TYPE_GAMEPAD_ADDED = 0x653,
  MSG_CORE_TYPE_GAMEPAD_REMOVED = 0x654,
  MSG_CORE_TYPE_GAMEPAD_REMAPPED = 0x655,
  MSG_CORE_TYPE_GAMEPAD_TOUCHPAD_DOWN = 0x656,
  MSG_CORE_TYPE_GAMEPAD_TOUCHPAD_MOTION = 0x657,
  MSG_CORE_TYPE_GAMEPAD_TOUCHPAD_UP = 0x658,
  MSG_CORE_TYPE_GAMEPAD_SENSOR_UPDATE = 0x659,
  MSG_CORE_TYPE_GAMEPAD_UPDATE_COMPLETE = 0x65A,
  MSG_CORE_TYPE_GAMEPAD_STEAM_HANDLE_UPDATED = 0x65B,
  MSG_CORE_TYPE_FINGER_DOWN = 0x700,
  MSG_CORE_TYPE_FINGER_UP = 0x701,
  MSG_CORE_TYPE_FINGER_MOTION = 0x702,
  MSG_CORE_TYPE_FINGER_CANCELED = 0x703,
  MSG_CORE_TYPE_CLIPBOARD_UPDATE = 0x900,
  MSG_CORE_TYPE_DROP_FILE = 0x1000,
  MSG_CORE_TYPE_DROP_TEXT = 0x1001,
  MSG_CORE_TYPE_DROP_BEGIN = 0x1002,
  MSG_CORE_TYPE_DROP_COMPLETE = 0x1003,
  MSG_CORE_TYPE_DROP_POSITION = 0x1004,
  MSG_CORE_TYPE_AUDIO_DEVICE_ADDED = 0x1100,
  MSG_CORE_TYPE_AUDIO_DEVICE_REMOVED = 0x1101,
  MSG_CORE_TYPE_AUDIO_DEVICE_FORMAT_CHANGED = 0x1102,
  MSG_CORE_TYPE_SENSOR_UPDATE = 0x1200,
  MSG_CORE_TYPE_PEN_PROXIMITY_IN = 0x1300,
  MSG_CORE_TYPE_PEN_PROXIMITY_OUT = 0x1301,
  MSG_CORE_TYPE_PEN_DOWN = 0x1302,
  MSG_CORE_TYPE_PEN_UP = 0x1303,
  MSG_CORE_TYPE_PEN_BUTTON_DOWN = 0x1304,
  MSG_CORE_TYPE_PEN_BUTTON_UP = 0x1305,
  MSG_CORE_TYPE_PEN_MOTION = 0x1306,
  MSG_CORE_TYPE_PEN_AXIS = 0x1307,
  MSG_CORE_TYPE_CAMERA_DEVICE_ADDED = 0x1400,
  MSG_CORE_TYPE_CAMERA_DEVICE_REMOVED = 0x1401,
  MSG_CORE_TYPE_CAMERA_DEVICE_APPROVED = 0x1402,
  MSG_CORE_TYPE_CAMERA_DEVICE_DENIED = 0x1403,
  MSG_CORE_TYPE_RENDER_TARGETS_RESET = 0x2000,
  MSG_CORE_TYPE_RENDER_DEVICE_RESET = 0x2001,
  MSG_CORE_TYPE_RENDER_DEVICE_LOST = 0x2002,
  MSG_CORE_TYPE_USER = 0x8000,
  MSG_CORE_TYPE_OBJECT_LIFECYCLE = MSG_CORE_TYPE_USER + 1,
  MSG_CORE_TYPE_THREAD_CTX = MSG_CORE_TYPE_USER + 2,
  MSG_CORE_TYPE_PATHWATCH = MSG_CORE_TYPE_USER + 3,
  MSG_CORE_TYPE_LOG = MSG_CORE_TYPE_USER + 4,
  MSG_CORE_TYPE_ASSERT = MSG_CORE_TYPE_USER + 5,
  MSG_CORE_TYPE_TOUCH_ADDED = MSG_CORE_TYPE_USER + 6,
  MSG_CORE_TYPE_TOUCH_REMOVED = MSG_CORE_TYPE_USER + 7,
  MSG_CORE_TYPE_TABLET_ADDED = MSG_CORE_TYPE_USER + 8,
  MSG_CORE_TYPE_TABLET_REMOVED = MSG_CORE_TYPE_USER + 9,
  MSG_CORE_TYPE_GLOBAL_CTX = MSG_CORE_TYPE_USER + 10,
} msg_core_type;

typedef enum msg_core_object_event_kind {
  MSG_CORE_OBJECT_EVENT_CREATE = 1,
  MSG_CORE_OBJECT_EVENT_DESTROY = 2,
} msg_core_object_event_kind;

typedef enum msg_core_object_type {
  MSG_CORE_OBJECT_TYPE_THREAD = 1,
  MSG_CORE_OBJECT_TYPE_THREAD_GROUP = 2,
  MSG_CORE_OBJECT_TYPE_MUTEX = 3,
  MSG_CORE_OBJECT_TYPE_CONDVAR = 4,
  MSG_CORE_OBJECT_TYPE_RWLOCK = 5,
  MSG_CORE_OBJECT_TYPE_SEMAPHORE = 6,
  MSG_CORE_OBJECT_TYPE_SPINLOCK = 7,
  MSG_CORE_OBJECT_TYPE_ARENA = 8,
  MSG_CORE_OBJECT_TYPE_HEAP = 9,
  MSG_CORE_OBJECT_TYPE_POOL = 10,
  MSG_CORE_OBJECT_TYPE_RING = 11,
  MSG_CORE_OBJECT_TYPE_PROCESS = 12,
  MSG_CORE_OBJECT_TYPE_ARCHIVE = 13,
  MSG_CORE_OBJECT_TYPE_PATHWATCH = 14,
  MSG_CORE_OBJECT_TYPE_LOG_STATE = 15,
  MSG_CORE_OBJECT_TYPE_FILEMAP = 16,
  MSG_CORE_OBJECT_TYPE_FILESTREAM = 17,
  MSG_CORE_OBJECT_TYPE_PIPE = 18,
} msg_core_object_type;

typedef enum msg_core_thread_ctx_event_kind {
  MSG_CORE_THREAD_CTX_EVENT_INIT = 1,
  MSG_CORE_THREAD_CTX_EVENT_QUIT = 2,
} msg_core_thread_ctx_event_kind;

typedef enum msg_core_pathwatch_event_kind {
  MSG_CORE_PATHWATCH_EVENT_ITEM = 1,
  MSG_CORE_PATHWATCH_EVENT_MISSED = 2,
} msg_core_pathwatch_event_kind;

typedef enum msg_core_global_ctx_event_kind {
  MSG_CORE_GLOBAL_CTX_EVENT_INIT = 1,
  MSG_CORE_GLOBAL_CTX_EVENT_QUIT = 2,
} msg_core_global_ctx_event_kind;

#define MSG_LOG_TEXT_CAP    512
#define MSG_ASSERT_TEXT_CAP 512

// Display event payload.
typedef struct msg_core_display_data {
  u32 display_id;
  i32 data1;
  i32 data2;
} msg_core_display_data;

// Window event payload.
typedef struct msg_core_window_data {
  u32 window_id;
  i32 data1;
  i32 data2;
} msg_core_window_data;

// Keyboard device connection payload.
typedef struct msg_core_keyboard_device_data {
  device_id device;
} msg_core_keyboard_device_data;

// Key press and release payload.
typedef struct msg_core_keyboard_data {
  u32 window_id;
  device_id device;
  u32 scancode;
  i32 keycode;
  u16 modifiers;
  u16 raw;
  b32 down;
  b32 repeat;
} msg_core_keyboard_data;

// IME editing payload.
typedef struct msg_core_text_editing_data {
  u32 window_id;
  cstr8 text;
  i32 start;
  i32 length;
} msg_core_text_editing_data;

// IME candidate list payload.
typedef struct msg_core_text_editing_candidates_data {
  u32 window_id;
  cstr8 const* candidates;
  i32 num_candidates;
  i32 selected_candidate;
  b32 horizontal;
} msg_core_text_editing_candidates_data;

// Text input payload.
typedef struct msg_core_text_input_data {
  u32 window_id;
  cstr8 text;
} msg_core_text_input_data;

// Mouse device connection payload.
typedef struct msg_core_mouse_device_data {
  device_id device;
} msg_core_mouse_device_data;

// Mouse motion payload.
typedef struct msg_core_mouse_motion_data {
  u32 window_id;
  device_id device;
  u32 button_mask;
  f32 x;
  f32 y;
  f32 xrel;
  f32 yrel;
} msg_core_mouse_motion_data;

// Mouse button payload.
typedef struct msg_core_mouse_button_data {
  u32 window_id;
  device_id device;
  mouse_button button;
  b32 down;
  u8 clicks;
  f32 x;
  f32 y;
} msg_core_mouse_button_data;

// Mouse wheel payload.
typedef struct msg_core_mouse_wheel_data {
  u32 window_id;
  device_id device;
  f32 x;
  f32 y;
  u32 direction;
  f32 mouse_x;
  f32 mouse_y;
} msg_core_mouse_wheel_data;

// Joystick device connection payload.
typedef struct msg_core_joystick_device_data {
  device_id device;
} msg_core_joystick_device_data;

// Joystick axis payload.
typedef struct msg_core_joystick_axis_data {
  device_id device;
  u8 axis;
  i16 value;
} msg_core_joystick_axis_data;

// Joystick trackball payload.
typedef struct msg_core_joystick_ball_data {
  device_id device;
  u8 ball;
  i16 xrel;
  i16 yrel;
} msg_core_joystick_ball_data;

// Joystick hat payload.
typedef struct msg_core_joystick_hat_data {
  device_id device;
  u8 hat;
  u8 value;
} msg_core_joystick_hat_data;

// Joystick button payload.
typedef struct msg_core_joystick_button_data {
  device_id device;
  u8 button;
  b32 down;
} msg_core_joystick_button_data;

// Joystick battery payload.
typedef struct msg_core_joystick_battery_data {
  device_id device;
  i32 state;
  i32 percent;
} msg_core_joystick_battery_data;

// Gamepad device connection payload.
typedef struct msg_core_gamepad_device_data {
  device_id device;
} msg_core_gamepad_device_data;

// Gamepad axis payload.
typedef struct msg_core_gamepad_axis_data {
  device_id device;
  gamepad_axis axis;
  i16 value;
} msg_core_gamepad_axis_data;

// Gamepad button payload.
typedef struct msg_core_gamepad_button_data {
  device_id device;
  gamepad_button button;
  b32 down;
} msg_core_gamepad_button_data;

// Gamepad touchpad payload.
typedef struct msg_core_gamepad_touchpad_data {
  device_id device;
  i32 touchpad;
  i32 finger;
  f32 x;
  f32 y;
  f32 pressure;
} msg_core_gamepad_touchpad_data;

// Gamepad sensor payload.
typedef struct msg_core_gamepad_sensor_data {
  device_id device;
  i32 sensor;
  f32 data[3];
  u64 sensor_timestamp;
} msg_core_gamepad_sensor_data;

// Audio device payload.
typedef struct msg_core_audio_device_data {
  u32 audio_device_id;
  b32 recording;
} msg_core_audio_device_data;

// Camera device payload.
typedef struct msg_core_camera_device_data {
  u32 camera_id;
} msg_core_camera_device_data;

// Render reset payload.
typedef struct msg_core_render_data {
  u32 window_id;
} msg_core_render_data;

// Touch device connection payload.
typedef struct msg_core_touch_device_data {
  device_id device;
} msg_core_touch_device_data;

// Tablet device connection payload.
typedef struct msg_core_tablet_device_data {
  device_id device;
} msg_core_tablet_device_data;

// Touch contact payload.
typedef struct msg_core_touch_data {
  device_id device;
  u64 finger_id;
  f32 x;
  f32 y;
  f32 dx;
  f32 dy;
  f32 pressure;
  u32 window_id;
} msg_core_touch_data;

// Pen proximity payload.
typedef struct msg_core_pen_proximity_data {
  u32 window_id;
  device_id device;
  u32 pen_id;
} msg_core_pen_proximity_data;

// Pen motion payload.
typedef struct msg_core_pen_motion_data {
  u32 window_id;
  device_id device;
  u32 pen_id;
  u32 pen_state;
  f32 x;
  f32 y;
} msg_core_pen_motion_data;

// Pen touch payload.
typedef struct msg_core_pen_touch_data {
  u32 window_id;
  device_id device;
  u32 pen_id;
  u32 pen_state;
  f32 x;
  f32 y;
  b32 eraser;
  b32 down;
} msg_core_pen_touch_data;

// Pen button payload.
typedef struct msg_core_pen_button_data {
  u32 window_id;
  device_id device;
  u32 pen_id;
  u32 pen_state;
  f32 x;
  f32 y;
  u8 button;
  b32 down;
} msg_core_pen_button_data;

// Pen axis payload.
typedef struct msg_core_pen_axis_data {
  u32 window_id;
  device_id device;
  u32 pen_id;
  u32 pen_state;
  f32 x;
  f32 y;
  tablet_axis axis;
  f32 value;
} msg_core_pen_axis_data;

// Drag-and-drop payload.
typedef struct msg_core_drop_data {
  u32 window_id;
  f32 x;
  f32 y;
  cstr8 source;
  cstr8 data;
} msg_core_drop_data;

// Clipboard update payload.
typedef struct msg_core_clipboard_data {
  b32 owner;
  i32 num_mime_types;
  cstr8 const* mime_types;
} msg_core_clipboard_data;

// Generic sensor payload.
typedef struct msg_core_sensor_data {
  u32 sensor_id;
  f32 data[6];
  u64 sensor_timestamp;
} msg_core_sensor_data;

typedef struct msg_core_object_lifecycle_data {
  msg_core_object_type object_type;
  msg_core_object_event_kind event_kind;
  void* object_ptr;
} msg_core_object_lifecycle_data;

typedef struct msg_core_thread_ctx_data {
  msg_core_thread_ctx_event_kind event_kind;
  u64 thread_id;
  ctx* ctx_ptr;
} msg_core_thread_ctx_data;

typedef struct msg_core_pathwatch_data {
  msg_core_pathwatch_event_kind event_kind;
  i64 pathwatch_id;
  i64 watch_id;
  pathwatch_action action;
} msg_core_pathwatch_data;

typedef struct msg_core_log_data {
  log_state* state_ptr;
  log_level level;
  callsite source_site;
  str8_medium text;
} msg_core_log_data;

typedef struct msg_core_global_ctx_data {
  msg_core_global_ctx_event_kind event_kind;
  global_ctx* global_ctx_ptr;
} msg_core_global_ctx_data;

typedef struct msg_core_assert_data {
  assert_mode mode;
  callsite source_site;
  str8_medium text;
} msg_core_assert_data;

typedef union msg_core_data {
  msg_core_display_data display;
  msg_core_window_data window;
  msg_core_keyboard_device_data keyboard_device;
  msg_core_keyboard_data keyboard;
  msg_core_text_editing_data text_editing;
  msg_core_text_editing_candidates_data text_editing_candidates;
  msg_core_text_input_data text_input;
  msg_core_mouse_device_data mouse_device;
  msg_core_mouse_motion_data mouse_motion;
  msg_core_mouse_button_data mouse_button;
  msg_core_mouse_wheel_data mouse_wheel;
  msg_core_joystick_device_data joystick_device;
  msg_core_joystick_axis_data joystick_axis;
  msg_core_joystick_ball_data joystick_ball;
  msg_core_joystick_hat_data joystick_hat;
  msg_core_joystick_button_data joystick_button;
  msg_core_joystick_battery_data joystick_battery;
  msg_core_gamepad_device_data gamepad_device;
  msg_core_gamepad_axis_data gamepad_axis;
  msg_core_gamepad_button_data gamepad_button;
  msg_core_gamepad_touchpad_data gamepad_touchpad;
  msg_core_gamepad_sensor_data gamepad_sensor;
  msg_core_audio_device_data audio_device;
  msg_core_camera_device_data camera_device;
  msg_core_render_data render;
  msg_core_touch_device_data touch_device;
  msg_core_touch_data touch;
  msg_core_tablet_device_data tablet_device;
  msg_core_pen_proximity_data pen_proximity;
  msg_core_pen_motion_data pen_motion;
  msg_core_pen_touch_data pen_touch;
  msg_core_pen_button_data pen_button;
  msg_core_pen_axis_data pen_axis;
  msg_core_drop_data drop;
  msg_core_clipboard_data clipboard;
  msg_core_sensor_data sensor;
  msg_core_object_lifecycle_data object_lifecycle;
  msg_core_thread_ctx_data thread_ctx;
  msg_core_pathwatch_data pathwatch;
  msg_core_log_data log;
  msg_core_assert_data assert;
  msg_core_global_ctx_data global_ctx;
} msg_core_data;

func void msg_core_fill_display(msg* src, const msg_core_display_data* core_data);
func void msg_core_fill_window(msg* src, const msg_core_window_data* core_data);
func void msg_core_fill_keyboard_device(msg* src, const msg_core_keyboard_device_data* core_data);
func void msg_core_fill_keyboard(msg* src, const msg_core_keyboard_data* core_data);
func void msg_core_fill_text_editing(msg* src, const msg_core_text_editing_data* core_data);
func void msg_core_fill_text_editing_candidates(msg* src, const msg_core_text_editing_candidates_data* core_data);
func void msg_core_fill_text_input(msg* src, const msg_core_text_input_data* core_data);
func void msg_core_fill_mouse_device(msg* src, const msg_core_mouse_device_data* core_data);
func void msg_core_fill_mouse_motion(msg* src, const msg_core_mouse_motion_data* core_data);
func void msg_core_fill_mouse_button(msg* src, const msg_core_mouse_button_data* core_data);
func void msg_core_fill_mouse_wheel(msg* src, const msg_core_mouse_wheel_data* core_data);
func void msg_core_fill_joystick_device(msg* src, const msg_core_joystick_device_data* core_data);
func void msg_core_fill_joystick_axis(msg* src, const msg_core_joystick_axis_data* core_data);
func void msg_core_fill_joystick_ball(msg* src, const msg_core_joystick_ball_data* core_data);
func void msg_core_fill_joystick_hat(msg* src, const msg_core_joystick_hat_data* core_data);
func void msg_core_fill_joystick_button(msg* src, const msg_core_joystick_button_data* core_data);
func void msg_core_fill_joystick_battery(msg* src, const msg_core_joystick_battery_data* core_data);
func void msg_core_fill_gamepad_device(msg* src, const msg_core_gamepad_device_data* core_data);
func void msg_core_fill_gamepad_axis(msg* src, const msg_core_gamepad_axis_data* core_data);
func void msg_core_fill_gamepad_button(msg* src, const msg_core_gamepad_button_data* core_data);
func void msg_core_fill_gamepad_touchpad(msg* src, const msg_core_gamepad_touchpad_data* core_data);
func void msg_core_fill_gamepad_sensor(msg* src, const msg_core_gamepad_sensor_data* core_data);
func void msg_core_fill_audio_device(msg* src, const msg_core_audio_device_data* core_data);
func void msg_core_fill_camera_device(msg* src, const msg_core_camera_device_data* core_data);
func void msg_core_fill_render(msg* src, const msg_core_render_data* core_data);
func void msg_core_fill_touch_device(msg* src, const msg_core_touch_device_data* core_data);
func void msg_core_fill_touch(msg* src, const msg_core_touch_data* core_data);
func void msg_core_fill_tablet_device(msg* src, const msg_core_tablet_device_data* core_data);
func void msg_core_fill_pen_proximity(msg* src, const msg_core_pen_proximity_data* core_data);
func void msg_core_fill_pen_motion(msg* src, const msg_core_pen_motion_data* core_data);
func void msg_core_fill_pen_touch(msg* src, const msg_core_pen_touch_data* core_data);
func void msg_core_fill_pen_button(msg* src, const msg_core_pen_button_data* core_data);
func void msg_core_fill_pen_axis(msg* src, const msg_core_pen_axis_data* core_data);
func void msg_core_fill_drop(msg* src, const msg_core_drop_data* core_data);
func void msg_core_fill_clipboard(msg* src, const msg_core_clipboard_data* core_data);
func void msg_core_fill_sensor(msg* src, const msg_core_sensor_data* core_data);
func void msg_core_fill_object_lifecycle(msg* src, const msg_core_object_lifecycle_data* core_data);
func void msg_core_fill_thread_ctx(msg* src, const msg_core_thread_ctx_data* core_data);
func void msg_core_fill_pathwatch(msg* src, const msg_core_pathwatch_data* core_data);
func void msg_core_fill_log(msg* src, const msg_core_log_data* core_data);
func void msg_core_fill_assert(msg* src, const msg_core_assert_data* core_data);
func void msg_core_fill_global_ctx(msg* src, const msg_core_global_ctx_data* core_data);

func msg_core_display_data* msg_core_get_display(msg* src);
func msg_core_window_data* msg_core_get_window(msg* src);
func msg_core_keyboard_device_data* msg_core_get_keyboard_device(msg* src);
func msg_core_keyboard_data* msg_core_get_keyboard(msg* src);
func msg_core_text_editing_data* msg_core_get_text_editing(msg* src);
func msg_core_text_editing_candidates_data* msg_core_get_text_editing_candidates(msg* src);
func msg_core_text_input_data* msg_core_get_text_input(msg* src);
func msg_core_mouse_device_data* msg_core_get_mouse_device(msg* src);
func msg_core_mouse_motion_data* msg_core_get_mouse_motion(msg* src);
func msg_core_mouse_button_data* msg_core_get_mouse_button(msg* src);
func msg_core_mouse_wheel_data* msg_core_get_mouse_wheel(msg* src);
func msg_core_joystick_device_data* msg_core_get_joystick_device(msg* src);
func msg_core_joystick_axis_data* msg_core_get_joystick_axis(msg* src);
func msg_core_joystick_ball_data* msg_core_get_joystick_ball(msg* src);
func msg_core_joystick_hat_data* msg_core_get_joystick_hat(msg* src);
func msg_core_joystick_button_data* msg_core_get_joystick_button(msg* src);
func msg_core_joystick_battery_data* msg_core_get_joystick_battery(msg* src);
func msg_core_gamepad_device_data* msg_core_get_gamepad_device(msg* src);
func msg_core_gamepad_axis_data* msg_core_get_gamepad_axis(msg* src);
func msg_core_gamepad_button_data* msg_core_get_gamepad_button(msg* src);
func msg_core_gamepad_touchpad_data* msg_core_get_gamepad_touchpad(msg* src);
func msg_core_gamepad_sensor_data* msg_core_get_gamepad_sensor(msg* src);
func msg_core_audio_device_data* msg_core_get_audio_device(msg* src);
func msg_core_camera_device_data* msg_core_get_camera_device(msg* src);
func msg_core_render_data* msg_core_get_render(msg* src);
func msg_core_touch_device_data* msg_core_get_touch_device(msg* src);
func msg_core_touch_data* msg_core_get_touch(msg* src);
func msg_core_tablet_device_data* msg_core_get_tablet_device(msg* src);
func msg_core_pen_proximity_data* msg_core_get_pen_proximity(msg* src);
func msg_core_pen_motion_data* msg_core_get_pen_motion(msg* src);
func msg_core_pen_touch_data* msg_core_get_pen_touch(msg* src);
func msg_core_pen_button_data* msg_core_get_pen_button(msg* src);
func msg_core_pen_axis_data* msg_core_get_pen_axis(msg* src);
func msg_core_drop_data* msg_core_get_drop(msg* src);
func msg_core_clipboard_data* msg_core_get_clipboard(msg* src);
func msg_core_sensor_data* msg_core_get_sensor(msg* src);
func msg_core_object_lifecycle_data* msg_core_get_object_lifecycle(msg* src);
func msg_core_thread_ctx_data* msg_core_get_thread_ctx(msg* src);
func msg_core_pathwatch_data* msg_core_get_pathwatch(msg* src);
func msg_core_log_data* msg_core_get_log(msg* src);
func msg_core_assert_data* msg_core_get_assert(msg* src);
func msg_core_global_ctx_data* msg_core_get_global_ctx(msg* src);
