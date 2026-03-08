// MIT License
// Copyright (c) 2026 Christian Luppi

#include "windowing/window.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

// =========================================================================
// Internal Helpers
// =========================================================================

func SDL_Window* window_resolve(window id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!window_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  SDL_Window* window_ptr = SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(id));
  TracyCZoneEnd(__tracy_zone_ctx);
  return window_ptr;
}

// =========================================================================
// Identifier Conversion Helpers
// =========================================================================

func b32 window_id_is_valid(window src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return src != NULL;
}

func window window_from_native_id(up native_id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (window)(up)native_id;
}

func up window_to_native_id(window src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (up)src;
}

// =========================================================================
// Global Window Enumeration
// =========================================================================

func sz window_get_count(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  int count = 0;
  SDL_Window** windows = SDL_GetWindows(&count);

  if (windows) {
    SDL_free(windows);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return count > 0 ? (sz)count : 0;
}

func b32 window_get_id(sz idx, window* out_id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  int count = 0;
  SDL_Window** windows = SDL_GetWindows(&count);
  b32 found = windows != NULL && idx < (sz)count && windows[idx] != NULL;

  if (out_id) {
    *out_id = NULL;
  }

  if (found && out_id) {
    *out_id = window_from_native_id((up)SDL_GetWindowID(windows[idx]));
  }

  if (windows) {
    SDL_free(windows);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return found;
}

// =========================================================================
// Creation and Destruction
// =========================================================================

func b32 window_is_valid(window id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return window_resolve(id) != NULL;
}

func window window_create(cstr8 title, i32 width, i32 height, u64 flags) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Window* window_ptr = SDL_CreateWindow(title != NULL ? title : "", width, height, (SDL_WindowFlags)flags);
  if (window_ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }

  window result = window_from_native_id((up)SDL_GetWindowID(window_ptr));
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func b32 window_destroy(window id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  SDL_DestroyWindow(window_ptr);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

// =========================================================================
// Visibility
// =========================================================================

func b32 window_show(window id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  SDL_ShowWindow(window_ptr);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 window_hide(window id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  SDL_HideWindow(window_ptr);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

// =========================================================================
// Position
// =========================================================================

func b32 window_move(window id, i32 xpos, i32 ypos) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  SDL_SetWindowPosition(window_ptr, xpos, ypos);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 window_get_position(window id, i32* out_xpos, i32* out_ypos) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL || out_xpos == NULL || out_ypos == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  SDL_GetWindowPosition(window_ptr, out_xpos, out_ypos);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

// =========================================================================
// Size
// =========================================================================

func b32 window_resize(window id, i32 width, i32 height) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  SDL_SetWindowSize(window_ptr, width, height);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 window_get_size(window id, i32* out_width, i32* out_height) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL || out_width == NULL || out_height == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  SDL_GetWindowSize(window_ptr, out_width, out_height);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

// =========================================================================
// Fullscreen State
// =========================================================================

func b32 window_set_fullscreen(window id, b32 enabled) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  b32 result = SDL_SetWindowFullscreen(window_ptr, enabled != 0);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func b32 window_is_fullscreen(window id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  b32 result = (SDL_GetWindowFlags(window_ptr) & SDL_WINDOW_FULLSCREEN) != 0 ? 1 : 0;
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

// =========================================================================
// Window-State Controls
// =========================================================================

func b32 window_minimize(window id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  SDL_MinimizeWindow(window_ptr);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 window_maximize(window id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  SDL_MaximizeWindow(window_ptr);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 window_restore(window id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  SDL_RestoreWindow(window_ptr);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 window_focus(window id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  SDL_RaiseWindow(window_ptr);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

// =========================================================================
// Metadata
// =========================================================================

func cstr8 window_get_title(window id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_GetWindowTitle(window_ptr);
}

func b32 window_set_title(window id, cstr8 title) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Window* window_ptr = window_resolve(id);
  if (window_ptr == NULL || title == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  SDL_SetWindowTitle(window_ptr, title);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 window_get_monitor_id(window id, monitor* out_monitor_id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Window* window_ptr = window_resolve(id);
  SDL_DisplayID native_monitor_id = 0;

  if (out_monitor_id) {
    *out_monitor_id = NULL;
  }

  if (window_ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  native_monitor_id = SDL_GetDisplayForWindow(window_ptr);
  if (native_monitor_id == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (out_monitor_id) {
    *out_monitor_id = monitor_from_native_id((up)native_monitor_id);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}
