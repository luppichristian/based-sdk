// MIT License
// Copyright (c) 2026 Christian Luppi

#include "windowing/display.h"
#include "../sdl3_include.h"

func b32 display_id_is_valid(display_id src) {
  return src.opaque != 0;
}

func display_id display_from_native_id(u64 native_id) {
  display_id result = {0};
  result.opaque = native_id;
  return result;
}

func u64 display_to_native_id(display_id src) {
  return src.opaque;
}

func sz display_get_count(void) {
  int count = 0;
  SDL_DisplayID* ids = SDL_GetDisplays(&count);

  if (ids) {
    SDL_free(ids);
  }

  return count > 0 ? (sz)count : 0;
}

func b32 display_get_id(sz idx, display_id* out_id) {
  int count = 0;
  SDL_DisplayID* ids = SDL_GetDisplays(&count);
  b32 found = ids != NULL && idx < (sz)count;

  if (out_id) {
    *out_id = (display_id) {0};
  }

  if (found && out_id) {
    *out_id = display_from_native_id((u64)ids[idx]);
  }

  if (ids) {
    SDL_free(ids);
  }

  return found;
}

func display_id display_get_primary_id(void) {
  return display_from_native_id((u64)SDL_GetPrimaryDisplay());
}

func cstr8 display_get_name(display_id id) {
  if (!display_id_is_valid(id)) {
    return NULL;
  }

  return SDL_GetDisplayName((SDL_DisplayID)display_to_native_id(id));
}

func b32 display_get_bounds(display_id id, display_rect* out_rect) {
  SDL_Rect bounds = {0};

  if (!display_id_is_valid(id) || out_rect == NULL ||
      !SDL_GetDisplayBounds((SDL_DisplayID)display_to_native_id(id), &bounds)) {
    return 0;
  }

  out_rect->x = bounds.x;
  out_rect->y = bounds.y;
  out_rect->width = bounds.w;
  out_rect->height = bounds.h;
  return 1;
}

func b32 display_get_usable_bounds(display_id id, display_rect* out_rect) {
  SDL_Rect bounds = {0};

  if (!display_id_is_valid(id) || out_rect == NULL ||
      !SDL_GetDisplayUsableBounds((SDL_DisplayID)display_to_native_id(id), &bounds)) {
    return 0;
  }

  out_rect->x = bounds.x;
  out_rect->y = bounds.y;
  out_rect->width = bounds.w;
  out_rect->height = bounds.h;
  return 1;
}
