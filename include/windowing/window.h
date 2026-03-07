// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../strings/cstrings.h"
#include "monitor.h"

typedef void* window;

// Identifier conversion helpers.
func b32 window_id_is_valid(window src);
func window window_from_native_id(up native_id);
func up window_to_native_id(window src);

// Global window enumeration.
func sz window_get_count(void);
func b32 window_get_id(sz idx, window* out_id);
func b32 window_is_valid(window id);

// Creation and destruction.
func window window_create(cstr8 title, i32 width, i32 height, u64 flags);
func b32 window_destroy(window id);

// Visibility.
func b32 window_show(window id);
func b32 window_hide(window id);

// Position.
func b32 window_move(window id, i32 xpos, i32 ypos);
func b32 window_get_position(window id, i32* out_xpos, i32* out_ypos);

// Size.
func b32 window_resize(window id, i32 width, i32 height);
func b32 window_get_size(window id, i32* out_width, i32* out_height);

// Fullscreen state.
func b32 window_set_fullscreen(window id, b32 enabled);
func b32 window_is_fullscreen(window id);

// Window-state controls.
func b32 window_minimize(window id);
func b32 window_maximize(window id);
func b32 window_restore(window id);
func b32 window_focus(window id);

// Metadata.
func cstr8 window_get_title(window id);
func b32 window_set_title(window id, cstr8 title);
func b32 window_get_monitor_id(window id, monitor* out_monitor_id);


