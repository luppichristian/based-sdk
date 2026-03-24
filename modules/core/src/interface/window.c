// MIT License
// Copyright (c) 2026 Christian Luppi

#include "interface/window.h"
#include "containers/hash_map.h"
#include "context/thread_ctx.h"
#include "memory/vmem.h"
#include "../internal.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"
#include "icon_internal.h"

global_var hash_map window_creation_monitor_map = {0};
global_var b32 window_creation_monitor_map_init = false;

// =========================================================================
// Internal Helpers
// =========================================================================

func SDL_Window* window_resolve(window window_id) {
  profile_func_begin;
  if (!window_id_is_valid(window_id)) {
    thread_log_warn("Rejected window resolve for invalid id");
    profile_func_end;
    return NULL;
  }

  SDL_Window* window_ptr = SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(window_id));
  if (window_ptr == NULL) {
    thread_log_warn("Failed to resolve window id=%llu", (unsigned long long)window_to_native_id(window_id));
  }
  profile_func_end;
  return window_ptr;
}

func b32 window_creation_monitor_map_ensure(void) {
  profile_func_begin;
  if (!window_creation_monitor_map_init) {
    window_creation_monitor_map = hash_map_create(16, vmem_get_allocator());
    window_creation_monitor_map_init = window_creation_monitor_map.slots != NULL;
    if (!window_creation_monitor_map_init) {
      thread_log_error("Failed to initialize window creation monitor map");
    }
  }

  profile_func_end;
  return window_creation_monitor_map_init;
}

func b32 window_monitor_from_native(SDL_DisplayID native_monitor_id, monitor* out_monitor_id) {
  profile_func_begin;
  if (out_monitor_id != NULL) {
    *out_monitor_id = NULL;
  }

  if (native_monitor_id == 0 || out_monitor_id == NULL) {
    profile_func_end;
    return false;
  }

  *out_monitor_id = monitor_from_native_id((up)native_monitor_id);
  profile_func_end;
  return true;
}

func b32 window_query_current_monitor(SDL_Window* window_ptr, monitor* out_monitor_id) {
  profile_func_begin;
  if (out_monitor_id != NULL) {
    *out_monitor_id = NULL;
  }

  if (window_ptr == NULL || out_monitor_id == NULL) {
    profile_func_end;
    return false;
  }

  SDL_DisplayID native_monitor_id = SDL_GetDisplayForWindow(window_ptr);
  if (native_monitor_id == 0) {
    thread_log_error("Failed to resolve monitor for window error=%s", SDL_GetError());
    profile_func_end;
    return false;
  }

  b32 result = window_monitor_from_native(native_monitor_id, out_monitor_id);
  profile_func_end;
  return result;
}

func void window_remember_creation_monitor(window window_id, monitor monitor_id) {
  profile_func_begin;
  if (!window_creation_monitor_map_ensure() || !window_id_is_valid(window_id) || !monitor_id_is_valid(monitor_id)) {
    profile_func_end;
    return;
  }

  if (!hash_map_set(&window_creation_monitor_map, (u64)window_to_native_id(window_id), monitor_id)) {
    thread_log_warn("Failed to store creation monitor for window id=%llu", (unsigned long long)window_to_native_id(window_id));
  }
  profile_func_end;
}

func monitor window_lookup_creation_monitor(window window_id) {
  profile_func_begin;
  if (!window_creation_monitor_map_ensure() || !window_id_is_valid(window_id)) {
    profile_func_end;
    return NULL;
  }

  monitor result = (monitor)hash_map_get(&window_creation_monitor_map, (u64)window_to_native_id(window_id));
  profile_func_end;
  return result;
}

func void window_forget_creation_monitor(window window_id) {
  profile_func_begin;
  if (window_creation_monitor_map_init && window_id_is_valid(window_id)) {
    hash_map_remove(&window_creation_monitor_map, (u64)window_to_native_id(window_id));
  }
  profile_func_end;
}

func b32 window_set_border_state(SDL_Window* window_ptr, b32 bordered) {
  profile_func_begin;
  b32 result = SDL_SetWindowBordered(window_ptr, bordered != 0) ? true : false;
  if (!result) {
    thread_log_error("Failed to set window border state bordered=%u error=%s", (u32)bordered, SDL_GetError());
  }
  profile_func_end;
  return result;
}

func b32 window_set_fullscreen_state(SDL_Window* window_ptr, b32 enabled) {
  profile_func_begin;
  b32 result = SDL_SetWindowFullscreen(window_ptr, enabled != 0) ? true : false;
  if (!result) {
    thread_log_error("Failed to set window fullscreen enabled=%u error=%s", (u32)enabled, SDL_GetError());
  }
  profile_func_end;
  return result;
}

func b32 window_has_flag(SDL_Window* window_ptr, SDL_WindowFlags flag) {
  if (window_ptr == NULL) {
    return false;
  }

  return (SDL_GetWindowFlags(window_ptr) & flag) != 0 ? true : false;
}

func b32 window_apply_mode(SDL_Window* window_ptr, window_mode mode) {
  profile_func_begin;
  if (window_ptr == NULL) {
    profile_func_end;
    return false;
  }

  switch (mode) {
    case WINDOW_MODE_WINDOWED:
      if (!window_set_fullscreen_state(window_ptr, false) || !window_set_border_state(window_ptr, true)) {
        profile_func_end;
        return false;
      }
      profile_func_end;
      return true;
    case WINDOW_MODE_UNDECORATED:
      if (!window_set_fullscreen_state(window_ptr, false) || !window_set_border_state(window_ptr, false)) {
        profile_func_end;
        return false;
      }
      profile_func_end;
      return true;
    case WINDOW_MODE_FULLSCREEN: {
      SDL_DisplayID native_monitor_id = SDL_GetDisplayForWindow(window_ptr);
      if (native_monitor_id == 0) {
        thread_log_error("Failed to resolve window monitor for fullscreen mode error=%s", SDL_GetError());
        profile_func_end;
        return false;
      }

      const SDL_DisplayMode* fullscreen_mode = SDL_GetCurrentDisplayMode(native_monitor_id);
      if (fullscreen_mode == NULL) {
        thread_log_error("Failed to query fullscreen display mode error=%s", SDL_GetError());
        profile_func_end;
        return false;
      }

      if (!SDL_SetWindowFullscreenMode(window_ptr, fullscreen_mode)) {
        thread_log_error("Failed to set exclusive fullscreen mode error=%s", SDL_GetError());
        profile_func_end;
        return false;
      }

      if (!window_set_fullscreen_state(window_ptr, true)) {
        profile_func_end;
        return false;
      }
      profile_func_end;
      return true;
    }
    case WINDOW_MODE_WINDOWED_FULLSCREEN:
      if (!SDL_SetWindowFullscreenMode(window_ptr, NULL)) {
        thread_log_error("Failed to set borderless fullscreen mode error=%s", SDL_GetError());
        profile_func_end;
        return false;
      }

      if (!window_set_fullscreen_state(window_ptr, true)) {
        profile_func_end;
        return false;
      }
      profile_func_end;
      return true;
    default:
      thread_log_error("Rejected unsupported window mode=%u", (u32)mode);
      profile_func_end;
      return false;
  }
}

// =========================================================================
// Identifier Conversion Helpers
// =========================================================================

func b32 window_id_is_valid(window src) {
  return src != NULL;
}

func window window_from_native_id(up native_id) {
  return (window)(up)native_id;
}

func up window_to_native_id(window src) {
  return (up)src;
}

// =========================================================================
// Global Window Enumeration
// =========================================================================

func sz window_get_total_count(void) {
  profile_func_begin;
  int count = 0;
  SDL_Window** windows = SDL_GetWindows(&count);

  if (windows != NULL) {
    SDL_free(windows);
  }

  profile_func_end;
  return count > 0 ? (sz)count : 0;
}

func window window_get_from_idx(sz idx) {
  profile_func_begin;
  int count = 0;
  window out_window_id = NULL;
  SDL_Window** windows = SDL_GetWindows(&count);
  b32 found = windows != NULL && idx < (sz)count && windows[idx] != NULL;
  if (found) {
    window_from_native_id((up)SDL_GetWindowID(windows[idx]));
  }

  if (windows != NULL) {
    SDL_free(windows);
  }

  profile_func_end;
  return out_window_id;
}

// =========================================================================
// Creation and Destruction
// =========================================================================

func b32 window_is_valid(window window_id) {
  return window_resolve(window_id) != NULL;
}

func window window_create(cstr8 title, i32 width, i32 height, window_mode mode, u64 flags) {
  profile_func_begin;
  u64 sanitized_flags = flags & ~(u64)(SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS);
  SDL_Window* window_ptr = SDL_CreateWindow(title != NULL ? title : "", width, height, (SDL_WindowFlags)sanitized_flags);
  if (window_ptr == NULL) {
    thread_log_error("Failed to create window title=%s width=%d height=%d flags=0x%llx error=%s",
                     title != NULL ? title : "",
                     width,
                     height,
                     (unsigned long long)sanitized_flags,
                     SDL_GetError());
    profile_func_end;
    return NULL;
  }

  window result = window_from_native_id((up)SDL_GetWindowID(window_ptr));
  monitor creation_monitor_id = NULL;
  if (window_query_current_monitor(window_ptr, &creation_monitor_id)) {
    window_remember_creation_monitor(result, creation_monitor_id);
  }

  if (!window_apply_mode(window_ptr, mode)) {
    thread_log_error("Failed to apply initial window mode=%u error=%s", (u32)mode, SDL_GetError());
    window_forget_creation_monitor(result);
    SDL_DestroyWindow(window_ptr);
    profile_func_end;
    return NULL;
  }

  input_native_backend_on_window_created(result);

  thread_log_info("Created window id=%llu title=%s width=%d height=%d mode=%u",
                  (unsigned long long)window_to_native_id(result),
                  title != NULL ? title : "",
                  width,
                  height,
                  (u32)mode);
  profile_func_end;
  return result;
}

func b32 window_destroy(window window_id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  if (window_ptr == NULL) {
    thread_log_error("Rejected window destroy for invalid window id=%llu", (unsigned long long)window_to_native_id(window_id));
    profile_func_end;
    return false;
  }

  input_native_backend_on_window_destroyed(window_id);
  window_forget_creation_monitor(window_id);
  SDL_DestroyWindow(window_ptr);
  thread_log_info("Destroyed window id=%llu", (unsigned long long)window_to_native_id(window_id));
  profile_func_end;
  return true;
}

// =========================================================================
// Visibility
// =========================================================================

func b32 window_is_hidden(window window_id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  b32 result = window_has_flag(window_ptr, SDL_WINDOW_HIDDEN);
  profile_func_end;
  return result;
}

func b32 window_show(window window_id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  if (window_ptr == NULL) {
    profile_func_end;
    return false;
  }

  b32 result = SDL_ShowWindow(window_ptr) ? true : false;
  if (!result) {
    thread_log_error("Failed to show window id=%llu error=%s", (unsigned long long)window_to_native_id(window_id), SDL_GetError());
  }
  profile_func_end;
  return result;
}

func b32 window_hide(window window_id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  if (window_ptr == NULL) {
    profile_func_end;
    return false;
  }

  b32 result = SDL_HideWindow(window_ptr) ? true : false;
  if (!result) {
    thread_log_error("Failed to hide window id=%llu error=%s", (unsigned long long)window_to_native_id(window_id), SDL_GetError());
  }
  profile_func_end;
  return result;
}

// =========================================================================
// Position
// =========================================================================

func b32 window_set_pos(window window_id, i32 xpos, i32 ypos) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  if (window_ptr == NULL) {
    profile_func_end;
    return false;
  }

  b32 result = SDL_SetWindowPosition(window_ptr, xpos, ypos) ? true : false;
  if (!result) {
    thread_log_error("Failed to set window pos id=%llu xpos=%d ypos=%d error=%s",
                     (unsigned long long)window_to_native_id(window_id),
                     xpos,
                     ypos,
                     SDL_GetError());
  }
  profile_func_end;
  return result;
}

func b32 window_get_pos(window window_id, i32* out_xpos, i32* out_ypos) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  if (window_ptr == NULL || out_xpos == NULL || out_ypos == NULL) {
    profile_func_end;
    return false;
  }

  b32 result = SDL_GetWindowPosition(window_ptr, out_xpos, out_ypos) ? true : false;
  if (!result) {
    thread_log_error("Failed to get window pos id=%llu error=%s", (unsigned long long)window_to_native_id(window_id), SDL_GetError());
  }
  profile_func_end;
  return result;
}

// =========================================================================
// Size
// =========================================================================

func b32 window_set_size(window window_id, i32 width, i32 height) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  if (window_ptr == NULL) {
    profile_func_end;
    return false;
  }

  b32 result = SDL_SetWindowSize(window_ptr, width, height) ? true : false;
  if (!result) {
    thread_log_error("Failed to set window size id=%llu width=%d height=%d error=%s",
                     (unsigned long long)window_to_native_id(window_id),
                     width,
                     height,
                     SDL_GetError());
  }
  profile_func_end;
  return result;
}

func b32 window_get_size(window window_id, i32* out_width, i32* out_height) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  if (window_ptr == NULL || out_width == NULL || out_height == NULL) {
    profile_func_end;
    return false;
  }

  b32 result = SDL_GetWindowSize(window_ptr, out_width, out_height) ? true : false;
  if (!result) {
    thread_log_error("Failed to get window size id=%llu error=%s", (unsigned long long)window_to_native_id(window_id), SDL_GetError());
  }
  profile_func_end;
  return result;
}

// =========================================================================
// Window Mode
// =========================================================================

func b32 window_set_mode(window window_id, window_mode mode) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  if (window_ptr == NULL) {
    profile_func_end;
    return false;
  }

  b32 result = window_apply_mode(window_ptr, mode);
  profile_func_end;
  return result;
}

func window_mode window_get_mode(window window_id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  if (window_ptr == NULL) {
    profile_func_end;
    return WINDOW_MODE_WINDOWED;
  }

  SDL_WindowFlags flags = SDL_GetWindowFlags(window_ptr);
  if ((flags & SDL_WINDOW_FULLSCREEN) != 0) {
    const SDL_DisplayMode* fullscreen_mode = SDL_GetWindowFullscreenMode(window_ptr);
    profile_func_end;
    return fullscreen_mode != NULL ? WINDOW_MODE_FULLSCREEN : WINDOW_MODE_WINDOWED_FULLSCREEN;
  }

  profile_func_end;
  return (flags & SDL_WINDOW_BORDERLESS) != 0 ? WINDOW_MODE_UNDECORATED : WINDOW_MODE_WINDOWED;
}

// =========================================================================
// Window-State Controls
// =========================================================================

func b32 window_is_minimized(window window_id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  b32 result = window_has_flag(window_ptr, SDL_WINDOW_MINIMIZED);
  profile_func_end;
  return result;
}

func b32 window_is_maximized(window window_id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  b32 result = window_has_flag(window_ptr, SDL_WINDOW_MAXIMIZED);
  profile_func_end;
  return result;
}

func b32 window_has_input_focus(window window_id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  b32 result = window_has_flag(window_ptr, SDL_WINDOW_INPUT_FOCUS);
  profile_func_end;
  return result;
}

func b32 window_has_mouse_focus(window window_id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  b32 result = window_has_flag(window_ptr, SDL_WINDOW_MOUSE_FOCUS);
  profile_func_end;
  return result;
}

func b32 window_is_topmost(window window_id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  b32 result = window_has_flag(window_ptr, SDL_WINDOW_ALWAYS_ON_TOP);
  profile_func_end;
  return result;
}

func b32 window_minimize(window window_id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  if (window_ptr == NULL) {
    profile_func_end;
    return false;
  }

  b32 result = SDL_MinimizeWindow(window_ptr) ? true : false;
  if (!result) {
    thread_log_error("Failed to minimize window id=%llu error=%s", (unsigned long long)window_to_native_id(window_id), SDL_GetError());
  }
  profile_func_end;
  return result;
}

func b32 window_maximize(window window_id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  if (window_ptr == NULL) {
    profile_func_end;
    return false;
  }

  b32 result = SDL_MaximizeWindow(window_ptr) ? true : false;
  if (!result) {
    thread_log_error("Failed to maximize window id=%llu error=%s", (unsigned long long)window_to_native_id(window_id), SDL_GetError());
  }
  profile_func_end;
  return result;
}

func b32 window_restore(window window_id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  if (window_ptr == NULL) {
    profile_func_end;
    return false;
  }

  b32 result = SDL_RestoreWindow(window_ptr) ? true : false;
  if (!result) {
    thread_log_error("Failed to restore window id=%llu error=%s", (unsigned long long)window_to_native_id(window_id), SDL_GetError());
  }
  profile_func_end;
  return result;
}

func b32 window_focus(window window_id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  if (window_ptr == NULL) {
    profile_func_end;
    return false;
  }

  b32 result = SDL_RaiseWindow(window_ptr) ? true : false;
  if (!result) {
    thread_log_error("Failed to focus window id=%llu error=%s", (unsigned long long)window_to_native_id(window_id), SDL_GetError());
  }
  profile_func_end;
  return result;
}

func b32 window_set_topmost(window window_id, b32 enabled) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  if (window_ptr == NULL) {
    profile_func_end;
    return false;
  }

  b32 result = SDL_SetWindowAlwaysOnTop(window_ptr, enabled != 0) ? true : false;
  if (!result) {
    thread_log_error("Failed to set window topmost enabled=%u error=%s", (u32)enabled, SDL_GetError());
  }
  profile_func_end;
  return result;
}

func window window_get_keyboard_focus(void) {
  profile_func_begin;
  SDL_Window* window_ptr = SDL_GetKeyboardFocus();
  window result = window_ptr != NULL ? window_from_native_id((up)SDL_GetWindowID(window_ptr)) : NULL;
  profile_func_end;
  return result;
}

func window window_get_cursor_focus(void) {
  profile_func_begin;
  SDL_Window* window_ptr = SDL_GetMouseFocus();
  window result = window_ptr != NULL ? window_from_native_id((up)SDL_GetWindowID(window_ptr)) : NULL;
  profile_func_end;
  return result;
}

// =========================================================================
// Metadata
// =========================================================================

func cstr8 window_get_title(window window_id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  if (window_ptr == NULL) {
    profile_func_end;
    return NULL;
  }

  profile_func_end;
  return SDL_GetWindowTitle(window_ptr);
}

func b32 window_set_title(window window_id, cstr8 title) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  if (window_ptr == NULL || title == NULL) {
    profile_func_end;
    return false;
  }

  b32 result = SDL_SetWindowTitle(window_ptr, title) ? true : false;
  if (!result) {
    thread_log_error("Failed to set window title id=%llu title=%s error=%s",
                     (unsigned long long)window_to_native_id(window_id),
                     title,
                     SDL_GetError());
  }
  profile_func_end;
  return result;
}

func b32 window_set_icon(window window_id, icon icon_id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  SDL_Surface* surface = icon_internal_get_surface(icon_id);
  if (window_ptr == NULL || surface == NULL) {
    if (window_ptr != NULL && icon_id_is_valid(icon_id)) {
      thread_log_warn("Rejected non-RGBA icon for window id=%llu", (unsigned long long)window_to_native_id(window_id));
    }
    profile_func_end;
    return false;
  }

  b32 result = SDL_SetWindowIcon(window_ptr, surface) ? true : false;
  if (!result) {
    thread_log_error("Failed to set window icon id=%llu error=%s", (unsigned long long)window_to_native_id(window_id), SDL_GetError());
  }
  profile_func_end;
  return result;
}

func b32 window_get_monitor(window window_id, monitor* out_monitor_id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  if (out_monitor_id != NULL) {
    *out_monitor_id = NULL;
  }

  if (window_ptr == NULL || out_monitor_id == NULL) {
    profile_func_end;
    return false;
  }

  b32 result = window_query_current_monitor(window_ptr, out_monitor_id);
  profile_func_end;
  return result;
}

func b32 window_get_creation_monitor(window window_id, monitor* out_monitor_id) {
  profile_func_begin;
  if (out_monitor_id != NULL) {
    *out_monitor_id = NULL;
  }

  if (!window_id_is_valid(window_id) || out_monitor_id == NULL) {
    profile_func_end;
    return false;
  }

  *out_monitor_id = window_lookup_creation_monitor(window_id);
  if (!monitor_id_is_valid(*out_monitor_id)) {
    b32 result = window_get_monitor(window_id, out_monitor_id);
    profile_func_end;
    return result;
  }

  profile_func_end;
  return true;
}

func b32 window_center_in_monitor(
    window window_id,
    monitor monitor_id,
    window_center_axis axis_mask) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  if (window_ptr == NULL) {
    profile_func_end;
    return false;
  }

  if (axis_mask == WINDOW_CENTER_AXIS_NONE) {
    profile_func_end;
    return true;
  }

  monitor target_monitor_id = monitor_id;
  if (!monitor_id_is_valid(target_monitor_id) && !window_get_monitor(window_id, &target_monitor_id)) {
    target_monitor_id = monitor_get_primary_id();
  }
  if (!monitor_id_is_valid(target_monitor_id)) {
    profile_func_end;
    return false;
  }

  r2_i32 bounds = monitor_get_usable_bounds(target_monitor_id);
  v2_i32 bounds_dim = lm2_v2_sub_i32(bounds.max, bounds.min);

  i32 xpos = 0;
  i32 ypos = 0;
  i32 width = 0;
  i32 height = 0;
  if (!window_get_pos(window_id, &xpos, &ypos) || !window_get_size(window_id, &width, &height)) {
    profile_func_end;
    return false;
  }

  if ((axis_mask & WINDOW_CENTER_AXIS_HORIZONTAL) != 0) {
    xpos = bounds.min.x + ((bounds_dim.x - width) / 2);
  }
  if ((axis_mask & WINDOW_CENTER_AXIS_VERTICAL) != 0) {
    ypos = bounds.min.y + ((bounds_dim.y - height) / 2);
  }

  b32 result = SDL_SetWindowPosition(window_ptr, xpos, ypos) ? true : false;
  if (!result) {
    thread_log_error("Failed to center window id=%llu xpos=%d ypos=%d error=%s",
                     (unsigned long long)window_to_native_id(window_id),
                     xpos,
                     ypos,
                     SDL_GetError());
  }
  profile_func_end;
  return result;
}
