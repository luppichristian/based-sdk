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
