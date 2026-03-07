// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/clipboard.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "strings/cstrings.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

func void clipboard_clear_output(c8* out_text, sz out_capacity) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!out_text || !out_capacity) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  out_text[0] = '\0';
  TracyCZoneEnd(__tracy_zone_ctx);
}

func b32 clipboard_has_text(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_HasClipboardText() ? 1 : 0;
}

func b32 clipboard_set_text(cstr8 src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!src) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(src != NULL);

  b32 success = SDL_SetClipboardText(src) ? 1 : 0;
  if (!success) {
    thread_log_warn("clipboard_set_text: failed");
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return success;
}

func b32 clipboard_get_text(c8* out_text, sz out_capacity) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  c8* source_text = NULL;
  b32 result = 0;

  if (!out_text || !out_capacity) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(out_text != NULL);

  clipboard_clear_output(out_text, out_capacity);

  source_text = SDL_GetClipboardText();
  if (!source_text) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  cstr8_copy(out_text, out_capacity, source_text);
  result = 1;
  SDL_free(source_text);
  thread_log_trace("clipboard_get_text: size=%zu", (size_t)cstr8_len(out_text));
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func b32 clipboard_clear(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_ClearClipboardData() ? 1 : 0;
}
