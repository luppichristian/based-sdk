// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../include/input/audio_device.h"
#include "../include/input/camera.h"
#include "../include/input/devices.h"
#include "../include/input/joystick.h"
#include "../include/input/keyboard.h"
#include "../include/input/mouse.h"
#include "../include/input/msg.h"
#include "../include/input/sensor.h"
#include "../include/interface/monitor.h"
#include "../include/interface/window.h"

struct SDL_Window;

// This header is internal to the core module and is not part of the public API.

// =========================================================================
c_begin;
// =========================================================================

func audio_device audio_device_from_native_id(up native_id, audio_device_type audio_type);
func up audio_device_to_native_id(audio_device src);

func camera camera_from_native_id(up native_id);
func up camera_to_native_id(camera src);

func device devices_make_id(device_type type, u64 instance);
func device devices_make_audio_device(u64 native_id, audio_device_type audio_type);
func u64 devices_get_instance(device src);
func u64 devices_get_audio_native_id(device src);
func void devices_update_runtime(device src, b32 connected, void* handle);
func void devices_touch_state(device src);
func void devices_set_focus(device src);
func device devices_get_focused_device(device_type type);
func void devices_set_battery(device src, battery_state state);

func b32 msg_from_native(const void* native_event, msg* out_msg);
func b32 msg_to_native(const msg* src, void* native_event);

func b32 input_native_backend_init(void);
func void input_native_backend_quit(void);
func void input_native_backend_pump(void);
func void input_native_backend_on_window_created(window window_id);
func void input_native_backend_on_window_destroyed(window window_id);
func device input_native_backend_resolve_keyboard_device(const void* native_event, device fallback_device);
func device input_native_backend_resolve_mouse_motion_device(const void* native_event, device fallback_device);
func device input_native_backend_resolve_mouse_button_device(const void* native_event, device fallback_device);
func device input_native_backend_resolve_mouse_wheel_device(const void* native_event, device fallback_device);
func device input_native_backend_resolve_pen_device(const void* native_event, device fallback_device);
func device input_native_backend_resolve_touch_device(const void* native_event, device fallback_device);
func sz input_native_backend_get_device_count(device_type type);
func device input_native_backend_get_device(device_type type, sz idx);
func b32 input_native_backend_get_info(device src, device_info* out_info);

func b32 keyboard_internal_scancode_is_valid(u32 scancode);
func u32 keyboard_internal_scancode_from_vkey(vkey key);
func vkey keyboard_internal_vkey_from_scancode(u32 scancode);
func mouse_button mouse_button_from_sdl(u8 button);
func i32 keyboard_internal_keycode_from_vkey(vkey key, keymod modifiers, b32 key_event);

func sensor sensor_from_native_id(up native_id);
func up sensor_to_native_id(sensor src);

func joystick joystick_from_native_id(up native_id);
func up joystick_to_native_id(joystick src);
func battery_state joystick_battery_state_from_native(i32 native_state);
func i32 joystick_battery_state_to_native(battery_state state);

func b32 keyboard_internal_on_msg(msg* src, void* user_data);
func b32 mouse_internal_on_msg(msg* src, void* user_data);
func b32 gamepad_internal_on_msg(msg* src, void* user_data);
func b32 joystick_internal_on_msg(msg* src, void* user_data);
func b32 tablet_internal_on_msg(msg* src, void* user_data);
func b32 msg_handle_runtime_internal(msg* src, void* user_data);

func monitor monitor_from_native_id(up native_id);
func up monitor_to_native_id(monitor src);

func struct SDL_Window* window_resolve(window window_id);
func window window_from_native_id(up native_id);
func up window_to_native_id(window src);

// Translates one native SDL event into a message, dispatches it immediately
// through msg_post, and writes the translated message to out_msg.
func b32 _msg_post_native(const void* native_event, msg* out_msg, callsite site);
#define msg_post_native(native_event, out_msg) _msg_post_native((native_event), (out_msg), CALLSITE_HERE)

// Thread lifecycle helpers used by entry/runtime shutdown.
func u32 core_thread_active_count(void);
func b32 core_thread_wait_idle(u32 timeout_ms);

// =========================================================================
c_end;
// =========================================================================
