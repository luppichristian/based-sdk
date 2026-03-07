// MIT License
// Copyright (c) 2026 Christian Luppi

#include "windowing/display.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

func b32 display_id_is_valid(display src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return src != NULL;
}

func display display_from_native_id(up native_id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (display)(up)native_id;
}

func up display_to_native_id(display src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (up)src;
}

func sz display_get_count(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  int count = 0;
  SDL_DisplayID* ids = SDL_GetDisplays(&count);

  if (ids) {
    SDL_free(ids);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return count > 0 ? (sz)count : 0;
}

func b32 display_get_id(sz idx, display* out_id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  int count = 0;
  SDL_DisplayID* ids = SDL_GetDisplays(&count);
  b32 found = ids != NULL && idx < (sz)count;

  if (out_id) {
    *out_id = NULL;
  }

  if (found && out_id) {
    *out_id = display_from_native_id((up)ids[idx]);
  }

  if (ids) {
    SDL_free(ids);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return found;
}

func display display_get_primary_id(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return display_from_native_id((up)SDL_GetPrimaryDisplay());
}

func cstr8 display_get_name(display id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!display_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_GetDisplayName((SDL_DisplayID)display_to_native_id(id));
}

func b32 display_get_bounds(display id, display_rect* out_rect) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Rect bounds = {0};

  if (!display_id_is_valid(id) || out_rect == NULL ||
      !SDL_GetDisplayBounds((SDL_DisplayID)display_to_native_id(id), &bounds)) {
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

func b32 display_get_usable_bounds(display id, display_rect* out_rect) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Rect bounds = {0};

  if (!display_id_is_valid(id) || out_rect == NULL ||
      !SDL_GetDisplayUsableBounds((SDL_DisplayID)display_to_native_id(id), &bounds)) {
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

func b32 display_mode_from_native(const SDL_DisplayMode* native_mode, display_mode* out_mode) {
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

func sz display_get_mode_count(display id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!display_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  int mode_count = 0;
  SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes((SDL_DisplayID)display_to_native_id(id), &mode_count);
  if (modes != NULL) {
    SDL_free(modes);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return mode_count > 0 ? (sz)mode_count : 0;
}

func b32 display_get_mode(display id, sz idx, display_mode* out_mode) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (out_mode == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  int mode_count = 0;
  SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes((SDL_DisplayID)display_to_native_id(id), &mode_count);
  b32 ok = modes != NULL && idx < (sz)mode_count;

  if (!ok) {
    if (modes != NULL) {
      SDL_free(modes);
    }
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  b32 result = display_mode_from_native(modes[idx], out_mode);
  SDL_free(modes);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func b32 display_get_current_mode(display id, display_mode* out_mode) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!display_id_is_valid(id) || out_mode == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  const SDL_DisplayMode* native_mode = SDL_GetCurrentDisplayMode((SDL_DisplayID)display_to_native_id(id));
  b32 result = display_mode_from_native(native_mode, out_mode);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func b32 display_get_desktop_mode(display id, display_mode* out_mode) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!display_id_is_valid(id) || out_mode == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  const SDL_DisplayMode* native_mode = SDL_GetDesktopDisplayMode((SDL_DisplayID)display_to_native_id(id));
  b32 result = display_mode_from_native(native_mode, out_mode);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func f32 display_get_refresh_rate(display id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  display_mode mode = {0};
  if (!display_get_current_mode(id, &mode)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0.0f;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return mode.refresh_rate;
}

func f32 display_get_content_scale(display id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!display_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1.0f;
  }

  f32 scale = SDL_GetDisplayContentScale((SDL_DisplayID)display_to_native_id(id));
  if (scale <= 0.0f) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1.0f;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return scale;
}

func display_orientation display_get_orientation(display id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!display_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return DISPLAY_ORIENTATION_UNKNOWN;
  }

  SDL_DisplayOrientation orientation = SDL_GetCurrentDisplayOrientation((SDL_DisplayID)display_to_native_id(id));
  switch (orientation) {
    case SDL_ORIENTATION_LANDSCAPE:
      TracyCZoneEnd(__tracy_zone_ctx);
      return DISPLAY_ORIENTATION_LANDSCAPE;
    case SDL_ORIENTATION_LANDSCAPE_FLIPPED:
      TracyCZoneEnd(__tracy_zone_ctx);
      return DISPLAY_ORIENTATION_LANDSCAPE_FLIPPED;
    case SDL_ORIENTATION_PORTRAIT:
      TracyCZoneEnd(__tracy_zone_ctx);
      return DISPLAY_ORIENTATION_PORTRAIT;
    case SDL_ORIENTATION_PORTRAIT_FLIPPED:
      TracyCZoneEnd(__tracy_zone_ctx);
      return DISPLAY_ORIENTATION_PORTRAIT_FLIPPED;
    case SDL_ORIENTATION_UNKNOWN:
    default:
      TracyCZoneEnd(__tracy_zone_ctx);
      return DISPLAY_ORIENTATION_UNKNOWN;
  }
}
