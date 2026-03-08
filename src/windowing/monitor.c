// MIT License
// Copyright (c) 2026 Christian Luppi

#include "windowing/monitor.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

// =========================================================================
// Identifier Conversion Helpers
// =========================================================================

func b32 monitor_id_is_valid(monitor src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return src != NULL;
}

func monitor monitor_from_native_id(up native_id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (monitor)(up)native_id;
}

func up monitor_to_native_id(monitor src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (up)src;
}

// =========================================================================
// Global Monitor Enumeration
// =========================================================================

func sz monitor_get_count(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  int count = 0;
  SDL_DisplayID* ids = SDL_GetDisplays(&count);

  if (ids) {
    SDL_free(ids);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return count > 0 ? (sz)count : 0;
}

func b32 monitor_get_id(sz idx, monitor* out_id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  int count = 0;
  SDL_DisplayID* ids = SDL_GetDisplays(&count);
  b32 found = ids != NULL && idx < (sz)count;

  if (out_id) {
    *out_id = NULL;
  }

  if (found && out_id) {
    *out_id = monitor_from_native_id((up)ids[idx]);
  }

  if (ids) {
    SDL_free(ids);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return found;
}

func monitor monitor_get_primary_id(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return monitor_from_native_id((up)SDL_GetPrimaryDisplay());
}

func cstr8 monitor_get_name(monitor id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!monitor_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_GetDisplayName((SDL_DisplayID)monitor_to_native_id(id));
}

// =========================================================================
// Monitor Geometry
// =========================================================================

func b32 monitor_get_bounds(monitor id, monitor_rect* out_rect) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Rect bounds = {0};

  if (!monitor_id_is_valid(id) || out_rect == NULL ||
      !SDL_GetDisplayBounds((SDL_DisplayID)monitor_to_native_id(id), &bounds)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  out_rect->x = bounds.x;
  out_rect->y = bounds.y;
  out_rect->width = bounds.w;
  out_rect->height = bounds.h;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 monitor_get_usable_bounds(monitor id, monitor_rect* out_rect) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Rect bounds = {0};

  if (!monitor_id_is_valid(id) || out_rect == NULL ||
      !SDL_GetDisplayUsableBounds((SDL_DisplayID)monitor_to_native_id(id), &bounds)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  out_rect->x = bounds.x;
  out_rect->y = bounds.y;
  out_rect->width = bounds.w;
  out_rect->height = bounds.h;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

// =========================================================================
// Display Mode Enumeration
// =========================================================================

func b32 monitor_mode_from_native(const SDL_DisplayMode* native_mode, monitor_mode* out_mode) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (native_mode == NULL || out_mode == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  out_mode->width = native_mode->w;
  out_mode->height = native_mode->h;
  out_mode->pixel_format = native_mode->format;
  out_mode->refresh_rate = native_mode->refresh_rate;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func sz monitor_get_mode_count(monitor id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!monitor_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  int mode_count = 0;
  SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes((SDL_DisplayID)monitor_to_native_id(id), &mode_count);
  if (modes != NULL) {
    SDL_free(modes);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return mode_count > 0 ? (sz)mode_count : 0;
}

func b32 monitor_get_mode(monitor id, sz idx, monitor_mode* out_mode) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (out_mode == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  int mode_count = 0;
  SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes((SDL_DisplayID)monitor_to_native_id(id), &mode_count);
  b32 ok = modes != NULL && idx < (sz)mode_count;

  if (!ok) {
    if (modes != NULL) {
      SDL_free(modes);
    }
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  b32 result = monitor_mode_from_native(modes[idx], out_mode);
  SDL_free(modes);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func b32 monitor_get_current_mode(monitor id, monitor_mode* out_mode) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!monitor_id_is_valid(id) || out_mode == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  const SDL_DisplayMode* native_mode = SDL_GetCurrentDisplayMode((SDL_DisplayID)monitor_to_native_id(id));
  b32 result = monitor_mode_from_native(native_mode, out_mode);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func b32 monitor_get_desktop_mode(monitor id, monitor_mode* out_mode) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!monitor_id_is_valid(id) || out_mode == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  const SDL_DisplayMode* native_mode = SDL_GetDesktopDisplayMode((SDL_DisplayID)monitor_to_native_id(id));
  b32 result = monitor_mode_from_native(native_mode, out_mode);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

// =========================================================================
// Convenience Queries
// =========================================================================

func f32 monitor_get_refresh_rate(monitor id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  monitor_mode mode = {0};
  if (!monitor_get_current_mode(id, &mode)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0.0f;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return mode.refresh_rate;
}

func f32 monitor_get_content_scale(monitor id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!monitor_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1.0f;
  }

  f32 scale = SDL_GetDisplayContentScale((SDL_DisplayID)monitor_to_native_id(id));
  if (scale <= 0.0f) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1.0f;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return scale;
}

func monitor_orientation monitor_get_orientation(monitor id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!monitor_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return MONITOR_ORIENTATION_UNKNOWN;
  }

  SDL_DisplayOrientation orientation = SDL_GetCurrentDisplayOrientation((SDL_DisplayID)monitor_to_native_id(id));
  switch (orientation) {
    case SDL_ORIENTATION_LANDSCAPE:
      TracyCZoneEnd(__tracy_zone_ctx);
      return MONITOR_ORIENTATION_LANDSCAPE;
    case SDL_ORIENTATION_LANDSCAPE_FLIPPED:
      TracyCZoneEnd(__tracy_zone_ctx);
      return MONITOR_ORIENTATION_LANDSCAPE_FLIPPED;
    case SDL_ORIENTATION_PORTRAIT:
      TracyCZoneEnd(__tracy_zone_ctx);
      return MONITOR_ORIENTATION_PORTRAIT;
    case SDL_ORIENTATION_PORTRAIT_FLIPPED:
      TracyCZoneEnd(__tracy_zone_ctx);
      return MONITOR_ORIENTATION_PORTRAIT_FLIPPED;
    case SDL_ORIENTATION_UNKNOWN:
    default:
      TracyCZoneEnd(__tracy_zone_ctx);
      return MONITOR_ORIENTATION_UNKNOWN;
  }
}
