// MIT License
// Copyright (c) 2026 Christian Luppi

#include "windowing/dialog.h"
#include "context/thread_ctx.h"
#include "../internal.h"
#include "basic/profiler.h"
#include "windowing/window.h"
#include "../sdl3_include.h"
#include "basic/safe.h"

typedef struct dialog_callback_ctx {
  dialog_file_callback* callback;
  void* user_data;
} dialog_callback_ctx;

// =========================================================================
// Internal Helpers
// =========================================================================

func SDL_Window* dialog_resolve_window(window owner) {
  profile_func_begin;
  if (!window_id_is_valid(owner)) {
    thread_log_warn("Dialog owner window is invalid");
    profile_func_end;
    return NULL;
  }
  SDL_Window* owner_window = SDL_GetWindowFromID((SDL_WindowID)window_to_native_id(owner));
  profile_func_end;
  return owner_window;
}

func SDL_MessageBoxFlags dialog_to_sdl_message_flags(dialog_message_kind message_kind) {
  profile_func_begin;
  switch (message_kind) {
    case DIALOG_MESSAGE_KIND_WARNING:
      profile_func_end;
      return SDL_MESSAGEBOX_WARNING;
    case DIALOG_MESSAGE_KIND_ERROR:
      profile_func_end;
      return SDL_MESSAGEBOX_ERROR;
    case DIALOG_MESSAGE_KIND_INFORMATION:
    default:
      profile_func_end;
      return SDL_MESSAGEBOX_INFORMATION;
  }
}

func SDL_MessageBoxFlags dialog_to_sdl_message_box_flags(u32 message_box_flags) {
  profile_func_begin;
  SDL_MessageBoxFlags sdl_flags = 0;
  if ((message_box_flags & DIALOG_MESSAGE_BOX_FLAG_ERROR) != 0) {
    sdl_flags |= SDL_MESSAGEBOX_ERROR;
  }
  if ((message_box_flags & DIALOG_MESSAGE_BOX_FLAG_WARNING) != 0) {
    sdl_flags |= SDL_MESSAGEBOX_WARNING;
  }
  if ((message_box_flags & DIALOG_MESSAGE_BOX_FLAG_INFORMATION) != 0) {
    sdl_flags |= SDL_MESSAGEBOX_INFORMATION;
  }
  if ((message_box_flags & DIALOG_MESSAGE_BOX_FLAG_BUTTONS_LEFT_TO_RIGHT) != 0) {
    sdl_flags |= SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT;
  }
  if ((message_box_flags & DIALOG_MESSAGE_BOX_FLAG_BUTTONS_RIGHT_TO_LEFT) != 0) {
    sdl_flags |= SDL_MESSAGEBOX_BUTTONS_RIGHT_TO_LEFT;
  }
  profile_func_end;
  return sdl_flags;
}

func SDL_MessageBoxButtonFlags dialog_to_sdl_message_box_button_flags(u32 button_flags) {
  profile_func_begin;
  SDL_MessageBoxButtonFlags sdl_flags = 0;
  if ((button_flags & DIALOG_MESSAGE_BOX_BUTTON_FLAG_RETURNKEY_DEFAULT) != 0) {
    sdl_flags |= SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
  }
  if ((button_flags & DIALOG_MESSAGE_BOX_BUTTON_FLAG_ESCAPEKEY_DEFAULT) != 0) {
    sdl_flags |= SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
  }
  profile_func_end;
  return sdl_flags;
}

func SDL_MessageBoxColor dialog_to_sdl_message_box_color(dialog_message_box_color color) {
  profile_func_begin;
  SDL_MessageBoxColor sdl_color = {(Uint8)color.red, (Uint8)color.green, (Uint8)color.blue};
  profile_func_end;
  return sdl_color;
}

func dialog_callback_ctx* dialog_make_callback_ctx(dialog_file_callback* callback, void* user_data) {
  profile_func_begin;
  if (callback == NULL) {
    profile_func_end;
    return NULL;
  }

  dialog_callback_ctx* callback_ctx = (dialog_callback_ctx*)SDL_malloc(size_of(dialog_callback_ctx));
  if (callback_ctx == NULL) {
    thread_log_error("Failed to allocate dialog callback context");
    profile_func_end;
    return NULL;
  }

  callback_ctx->callback = callback;
  callback_ctx->user_data = user_data;
  profile_func_end;
  return callback_ctx;
}

func void SDLCALL dialog_file_callback_bridge(
    void* user_data,
    const c8* const* file_list,
    int filter_idx) {
  profile_func_begin;
  dialog_callback_ctx* callback_ctx = (dialog_callback_ctx*)user_data;
  if (callback_ctx == NULL || callback_ctx->callback == NULL) {
    thread_log_error("Dialog callback bridge received invalid context user_data=%p", user_data);
    if (callback_ctx != NULL) {
      SDL_free(callback_ctx);
    }
    profile_func_end;
    return;
  }

  b32 has_error = file_list == NULL;
  b32 is_cancelled = file_list != NULL && file_list[0] == NULL;
  thread_log_trace("Dialog callback completed filter_idx=%d cancelled=%u error=%u",
                   filter_idx,
                   (u32)is_cancelled,
                   (u32)has_error);
  callback_ctx->callback(callback_ctx->user_data, (cstr8 const*)file_list, (i32)filter_idx, is_cancelled, has_error);
  SDL_free(callback_ctx);
  profile_func_end;
}

// =========================================================================
// Public API
// =========================================================================

func b32 dialog_show_message(
    window owner,
    dialog_message_kind message_kind,
    cstr8 title,
    cstr8 message) {
  profile_func_begin;
  SDL_Window* owner_window = dialog_resolve_window(owner);
  b32 result = SDL_ShowSimpleMessageBox(
      dialog_to_sdl_message_flags(message_kind),
      title != NULL ? title : "Message",
      message != NULL ? message : "",
      owner_window);
  if (!result) {
    thread_log_error("Failed to show simple dialog title=%s error=%s",
                     title != NULL ? title : "Message",
                     SDL_GetError());
  }
  profile_func_end;
  return result;
}

func b32 dialog_show_message_box(
    window owner,
    const dialog_message_box* message_box,
    i32* out_button_id) {
  profile_func_begin;
  if (message_box == NULL) {
    thread_log_error("Rejected dialog message box because descriptor is NULL");
    profile_func_end;
    return false;
  }
  if (message_box->button_count > 0 && message_box->buttons == NULL) {
    thread_log_error("Rejected dialog message box because buttons are missing count=%zu", (size_t)message_box->button_count);
    profile_func_end;
    return false;
  }
  if (message_box->button_count > (sz)I32_MAX) {
    thread_log_error("Rejected dialog message box because button count is too large count=%zu", (size_t)message_box->button_count);
    profile_func_end;
    return false;
  }

  SDL_Window* owner_window = dialog_resolve_window(owner);
  int button_count = (int)message_box->button_count;

  SDL_MessageBoxButtonData* sdl_buttons = NULL;
  if (button_count > 0) {
    sdl_buttons = (SDL_MessageBoxButtonData*)SDL_malloc((sz)button_count * size_of(SDL_MessageBoxButtonData));
    if (sdl_buttons == NULL) {
      thread_log_error("Failed to allocate dialog buttons count=%d", button_count);
      profile_func_end;
      return false;
    }

    safe_for (int idx = 0; idx < button_count; ++idx) {
      const dialog_message_box_button* button = &message_box->buttons[idx];
      sdl_buttons[idx].flags = dialog_to_sdl_message_box_button_flags(button->flags);
      sdl_buttons[idx].buttonID = button->button_id;
      sdl_buttons[idx].text = button->text != NULL ? button->text : "";
    }
  }

  SDL_MessageBoxColorScheme sdl_color_scheme = {0};
  const SDL_MessageBoxColorScheme* sdl_color_scheme_ptr = NULL;
  if (message_box->color_scheme != NULL) {
    sdl_color_scheme.colors[SDL_MESSAGEBOX_COLOR_BACKGROUND] =
        dialog_to_sdl_message_box_color(message_box->color_scheme->background);
    sdl_color_scheme.colors[SDL_MESSAGEBOX_COLOR_TEXT] =
        dialog_to_sdl_message_box_color(message_box->color_scheme->text);
    sdl_color_scheme.colors[SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] =
        dialog_to_sdl_message_box_color(message_box->color_scheme->button_border);
    sdl_color_scheme.colors[SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] =
        dialog_to_sdl_message_box_color(message_box->color_scheme->button_background);
    sdl_color_scheme.colors[SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] =
        dialog_to_sdl_message_box_color(message_box->color_scheme->button_selected);
    sdl_color_scheme_ptr = &sdl_color_scheme;
  }

  SDL_MessageBoxData sdl_message_box_data = {
      dialog_to_sdl_message_box_flags(message_box->flags),
      owner_window,
      message_box->title != NULL ? message_box->title : "Message",
      message_box->message != NULL ? message_box->message : "",
      button_count,
      sdl_buttons,
      sdl_color_scheme_ptr,
  };

  int selected_button_id = -1;
  b32 result = SDL_ShowMessageBox(&sdl_message_box_data, &selected_button_id);
  if (!result) {
    thread_log_error("Failed to show dialog message box title=%s error=%s",
                     message_box->title != NULL ? message_box->title : "Message",
                     SDL_GetError());
  }
  if (result && out_button_id != NULL) {
    *out_button_id = (i32)selected_button_id;
  }

  if (sdl_buttons != NULL) {
    SDL_free(sdl_buttons);
  }
  profile_func_end;
  return result;
}

func b32 dialog_open_file(
    window owner,
    const dialog_file_filter* filters,
    sz filter_count,
    cstr8 default_location,
    b32 allow_many,
    dialog_file_callback* callback,
    void* user_data) {
  profile_func_begin;
  SDL_Window* owner_window = dialog_resolve_window(owner);
  dialog_callback_ctx* callback_ctx = dialog_make_callback_ctx(callback, user_data);
  if (callback != NULL && callback_ctx == NULL) {
    thread_log_error("Failed to create open file dialog callback context");
    profile_func_end;
    return false;
  }

  SDL_ShowOpenFileDialog(
      callback_ctx != NULL ? dialog_file_callback_bridge : NULL,
      callback_ctx,
      owner_window,
      (const SDL_DialogFileFilter*)filters,
      (int)filter_count,
      default_location,
      allow_many != 0);
  profile_func_end;
  return true;
}

func b32 dialog_save_file(
    window owner,
    const dialog_file_filter* filters,
    sz filter_count,
    cstr8 default_location,
    dialog_file_callback* callback,
    void* user_data) {
  profile_func_begin;
  SDL_Window* owner_window = dialog_resolve_window(owner);
  dialog_callback_ctx* callback_ctx = dialog_make_callback_ctx(callback, user_data);
  if (callback != NULL && callback_ctx == NULL) {
    thread_log_error("Failed to create save file dialog callback context");
    profile_func_end;
    return false;
  }

  SDL_ShowSaveFileDialog(
      callback_ctx != NULL ? dialog_file_callback_bridge : NULL,
      callback_ctx,
      owner_window,
      (const SDL_DialogFileFilter*)filters,
      (int)filter_count,
      default_location);
  profile_func_end;
  return true;
}

func b32 dialog_open_folder(
    window owner,
    cstr8 default_location,
    b32 allow_many,
    dialog_file_callback* callback,
    void* user_data) {
  profile_func_begin;
  SDL_Window* owner_window = dialog_resolve_window(owner);
  dialog_callback_ctx* callback_ctx = dialog_make_callback_ctx(callback, user_data);
  if (callback != NULL && callback_ctx == NULL) {
    thread_log_error("Failed to create open folder dialog callback context");
    profile_func_end;
    return false;
  }

  SDL_ShowOpenFolderDialog(
      callback_ctx != NULL ? dialog_file_callback_bridge : NULL,
      callback_ctx,
      owner_window,
      default_location,
      allow_many != 0);
  profile_func_end;
  return true;
}
