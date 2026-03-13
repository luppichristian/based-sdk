// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/clipboard.h"
#include "basic/assert.h"
#include "memory/memops.h"
#include "context/thread_ctx.h"
#include "strings/cstrings.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

typedef struct clipboard_data_state {
  c8* mime_type;
  sz data_size;
  u8* data_ptr;
} clipboard_data_state;

func void clipboard_clear_output(c8* out_text, sz out_capacity) {
  profile_func_begin;
  if (!out_text || !out_capacity) {
    profile_func_end;
    return;
  }

  out_text[0] = '\0';
  profile_func_end;
}

func const void* SDLCALL clipboard_data_callback(void* userdata, cstr8 mime_type, size_t* size) {
  clipboard_data_state* state = (clipboard_data_state*)userdata;

  if (!size) {
    return NULL;
  }

  *size = 0;
  if (!state || !mime_type || !state->mime_type || !cstr8_cmp(state->mime_type, mime_type)) {
    return NULL;
  }

  *size = (size_t)state->data_size;
  return state->data_ptr;
}

func void SDLCALL clipboard_data_cleanup(void* userdata) {
  clipboard_data_state* state = (clipboard_data_state*)userdata;
  if (!state) {
    return;
  }

  SDL_free(state);
}

func clipboard_data_state* clipboard_data_state_create(cstr8 mime_type, buffer src_data) {
  profile_func_begin;
  sz mime_size = cstr8_len(mime_type) + 1;
  sz total_size = size_of(clipboard_data_state) + mime_size + src_data.size;
  clipboard_data_state* state = (clipboard_data_state*)SDL_malloc(total_size);
  if (!state) {
    profile_func_end;
    return NULL;
  }

  state->mime_type = (c8*)(state + 1);
  state->data_size = src_data.size;
  state->data_ptr = (u8*)(state->mime_type + mime_size);
  cstr8_cpy(state->mime_type, mime_size, mime_type);
  if (src_data.size > 0) {
    mem_cpy(state->data_ptr, src_data.ptr, src_data.size);
  }

  profile_func_end;
  return state;
}

func b32 clipboard_has_text(void) {
  return SDL_HasClipboardText() ? true : false;
}

func b32 clipboard_has_data(cstr8 mime_type) {
  if (!mime_type || cstr8_is_empty(mime_type)) {
    return false;
  }

  return SDL_HasClipboardData(mime_type) ? true : false;
}

func b32 clipboard_set_text(cstr8 src) {
  profile_func_begin;
  if (!src) {
    thread_log_error("Rejected clipboard text set for NULL input");
    profile_func_end;
    return false;
  }
  assert(src != NULL);

  b32 success = SDL_SetClipboardText(src) ? true : false;
  if (!success) {
    thread_log_warn("Failed to set clipboard text error=%s", SDL_GetError());
  } else {
    thread_log_trace("Set clipboard text size=%zu", (size_t)cstr8_len(src));
  }
  profile_func_end;
  return success;
}

func b32 clipboard_set_data(cstr8 mime_type, buffer src_data) {
  profile_func_begin;
  clipboard_data_state* state = NULL;
  cstr8 mime_types[1] = {0};
  b32 success = false;

  if (!mime_type || cstr8_is_empty(mime_type)) {
    thread_log_error("Rejected clipboard data set for invalid mime type");
    profile_func_end;
    return false;
  }

  if (src_data.size > 0 && !src_data.ptr) {
    thread_log_error("Rejected clipboard data set for invalid source buffer mime=%s", mime_type);
    profile_func_end;
    return false;
  }

  state = clipboard_data_state_create(mime_type, src_data);
  if (!state) {
    thread_log_warn("Failed to allocate clipboard data storage mime=%s size=%zu", mime_type, (size_t)src_data.size);
    profile_func_end;
    return false;
  }

  mime_types[0] = state->mime_type;
  success = SDL_SetClipboardData(clipboard_data_callback, clipboard_data_cleanup, state, mime_types, count_of(mime_types)) ? true : false;
  if (!success) {
    thread_log_warn("Failed to set clipboard data mime=%s error=%s", mime_type, SDL_GetError());
    clipboard_data_cleanup(state);
  } else {
    thread_log_trace("Set clipboard data mime=%s size=%zu", mime_type, (size_t)src_data.size);
  }

  profile_func_end;
  return success;
}

func b32 clipboard_get_text(c8* out_text, sz out_capacity) {
  profile_func_begin;
  c8* source_text = NULL;
  b32 result = false;

  if (!out_text || !out_capacity) {
    thread_log_error("Rejected clipboard text read with invalid output buffer");
    profile_func_end;
    return false;
  }
  assert(out_text != NULL);

  clipboard_clear_output(out_text, out_capacity);

  source_text = SDL_GetClipboardText();
  if (!source_text) {
    thread_log_warn("Failed to get clipboard text error=%s", SDL_GetError());
    profile_func_end;
    return false;
  }

  cstr8_cpy(out_text, out_capacity, source_text);
  result = true;
  SDL_free(source_text);
  thread_log_trace("Read clipboard text size=%zu", (size_t)cstr8_len(out_text));
  profile_func_end;
  return result;
}

func b32 clipboard_get_data(cstr8 mime_type, buffer out_data, sz* out_size) {
  profile_func_begin;
  void* data_ptr = NULL;
  size_t data_size = 0;

  if (!mime_type || cstr8_is_empty(mime_type)) {
    thread_log_error("Rejected clipboard data read for invalid mime type");
    profile_func_end;
    return false;
  }

  if (out_data.size > 0 && !out_data.ptr) {
    thread_log_error("Rejected clipboard data read with invalid output buffer mime=%s", mime_type);
    profile_func_end;
    return false;
  }

  data_ptr = SDL_GetClipboardData(mime_type, &data_size);
  if (!data_ptr) {
    if (out_size) {
      *out_size = 0;
    }
    thread_log_warn("Failed to get clipboard data mime=%s error=%s", mime_type, SDL_GetError());
    profile_func_end;
    return false;
  }

  if (out_size) {
    *out_size = (sz)data_size;
  }

  if (out_data.size > 0 && (sz)data_size > out_data.size) {
    thread_log_warn("Clipboard data buffer too small mime=%s required=%zu available=%zu", mime_type, data_size, (size_t)out_data.size);
    SDL_free(data_ptr);
    profile_func_end;
    return false;
  }

  if (data_size > 0 && out_data.ptr) {
    mem_cpy(out_data.ptr, data_ptr, (sz)data_size);
  }

  SDL_free(data_ptr);
  thread_log_trace("Read clipboard data mime=%s size=%zu", mime_type, data_size);
  profile_func_end;
  return true;
}

func b32 clipboard_clear(void) {
  profile_func_begin;
  b32 success = SDL_ClearClipboardData() ? true : false;
  if (!success) {
    thread_log_warn("Failed to clear clipboard data error=%s", SDL_GetError());
  } else {
    thread_log_trace("Cleared clipboard data");
  }
  profile_func_end;
  return success;
}
