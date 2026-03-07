// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/directory.h"

#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "filesystem/file.h"

#include "../sdl3_include.h"
#include "basic/profiler.h"

#include <string.h>

typedef struct dir_iterate_state {
  path root_path;
  b32 recursive;
  dir_iterate_callback* callback;
  void* user_data;
  b32 stop_requested;
  b32 success;
} dir_iterate_state;

typedef struct dir_remove_state {
  b32 success;
} dir_remove_state;

typedef struct dir_copy_state {
  path dst_root;
  b32 overwrite_existing;
  b32 success;
} dir_copy_state;

func path dir_path_from_string(cstr8 src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return path_from_cstr(src != NULL ? src : "");
}

func path dir_path_from_owned_string(c8* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  path result = dir_path_from_string(src);
  if (src != NULL) {
    SDL_free(src);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func cstr8 dir_path_cstr(const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (src == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return "";
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return src->buf;
}

func SDL_Folder dir_system_path_to_sdl(dir_system_path location) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  switch (location) {
    case DIR_SYSTEM_PATH_HOME:
      TracyCZoneEnd(__tracy_zone_ctx);
      return SDL_FOLDER_HOME;
    case DIR_SYSTEM_PATH_DESKTOP:
      TracyCZoneEnd(__tracy_zone_ctx);
      return SDL_FOLDER_DESKTOP;
    case DIR_SYSTEM_PATH_DOCUMENTS:
      TracyCZoneEnd(__tracy_zone_ctx);
      return SDL_FOLDER_DOCUMENTS;
    case DIR_SYSTEM_PATH_DOWNLOADS:
      TracyCZoneEnd(__tracy_zone_ctx);
      return SDL_FOLDER_DOWNLOADS;
    case DIR_SYSTEM_PATH_MUSIC:
      TracyCZoneEnd(__tracy_zone_ctx);
      return SDL_FOLDER_MUSIC;
    case DIR_SYSTEM_PATH_PICTURES:
      TracyCZoneEnd(__tracy_zone_ctx);
      return SDL_FOLDER_PICTURES;
    case DIR_SYSTEM_PATH_PUBLICSHARE:
      TracyCZoneEnd(__tracy_zone_ctx);
      return SDL_FOLDER_PUBLICSHARE;
    case DIR_SYSTEM_PATH_SAVEDGAMES:
      TracyCZoneEnd(__tracy_zone_ctx);
      return SDL_FOLDER_SAVEDGAMES;
    case DIR_SYSTEM_PATH_SCREENSHOTS:
      TracyCZoneEnd(__tracy_zone_ctx);
      return SDL_FOLDER_SCREENSHOTS;
    case DIR_SYSTEM_PATH_TEMPLATES:
      TracyCZoneEnd(__tracy_zone_ctx);
      return SDL_FOLDER_TEMPLATES;
    case DIR_SYSTEM_PATH_VIDEOS:
      TracyCZoneEnd(__tracy_zone_ctx);
      return SDL_FOLDER_VIDEOS;
    case DIR_SYSTEM_PATH_COUNT:
      break;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_FOLDER_COUNT;
}

func sz dir_root_length(cstr8 src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (src == NULL || src[0] == '\0') {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (src[0] == '/' || src[0] == '\\') {
    if (src[1] == '/' || src[1] == '\\') {
      TracyCZoneEnd(__tracy_zone_ctx);
      return 2;
    }
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  if (((src[0] >= 'A' && src[0] <= 'Z') || (src[0] >= 'a' && src[0] <= 'z')) &&
      src[1] == ':' && (src[2] == '/' || src[2] == '\\')) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 3;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
}

func path dir_relative_path(const path* root_path, const path* full_path) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  path relative_path = *full_path;
  path root_copy = *root_path;
  sz root_len = 0;

  path_normalize(&relative_path);
  path_normalize(&root_copy);
  path_remove_trailing_slash(&root_copy);
  root_len = cstr8_len(root_copy.buf);

  if (root_len == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return relative_path;
  }

  if (cstr8_cmp_n(relative_path.buf, root_copy.buf, root_len) == 0) {
    if (relative_path.buf[root_len] == '/') {
      memmove(
          relative_path.buf,
          relative_path.buf + root_len + 1,
          cstr8_len(relative_path.buf + root_len + 1) + 1);
      TracyCZoneEnd(__tracy_zone_ctx);
      return relative_path;
    }

    if (relative_path.buf[root_len] == '\0') {
      cstr8_clear(relative_path.buf);
      TracyCZoneEnd(__tracy_zone_ctx);
      return relative_path;
    }
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return relative_path;
}

func b32 dir_emit_entry(
    dir_iterate_state* state,
    const path* full_path,
    b32 is_directory) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  dir_entry entry;

  if (state->callback == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  entry.full_path = *full_path;
  entry.relative_path = dir_relative_path(&state->root_path, full_path);
  entry.is_directory = is_directory;
  TracyCZoneEnd(__tracy_zone_ctx);
  return state->callback(&entry, state->user_data);
}

func SDL_EnumerationResult SDLCALL dir_enumerate_callback(
    void* user_data,
    const char* dirname,
    const char* fname) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  dir_iterate_state* state = (dir_iterate_state*)user_data;
  SDL_PathInfo path_info;
  path dir_path;
  path name_path;
  path full_path;

  if (state == NULL || fname == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return SDL_ENUM_FAILURE;
  }

  if (state->stop_requested) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return SDL_ENUM_SUCCESS;
  }

  dir_path = path_from_cstr(dirname != NULL ? dirname : "");
  name_path = path_from_cstr(fname);
  full_path = path_join(&dir_path, &name_path);

  if (!SDL_GetPathInfo(full_path.buf, &path_info)) {
    state->success = 0;
    TracyCZoneEnd(__tracy_zone_ctx);
    return SDL_ENUM_FAILURE;
  }

  if (!dir_emit_entry(state, &full_path, path_info.type == SDL_PATHTYPE_DIRECTORY ? 1 : 0)) {
    state->stop_requested = 1;
    TracyCZoneEnd(__tracy_zone_ctx);
    return SDL_ENUM_SUCCESS;
  }

  if (state->recursive && path_info.type == SDL_PATHTYPE_DIRECTORY) {
    if (!SDL_EnumerateDirectory(full_path.buf, dir_enumerate_callback, state)) {
      if (state->stop_requested) {
        TracyCZoneEnd(__tracy_zone_ctx);
        return SDL_ENUM_SUCCESS;
      }

      state->success = 0;
      TracyCZoneEnd(__tracy_zone_ctx);
      return SDL_ENUM_FAILURE;
    }

    if (state->stop_requested) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return SDL_ENUM_SUCCESS;
    }
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_ENUM_CONTINUE;
}

func SDL_EnumerationResult SDLCALL dir_remove_callback(
    void* user_data,
    const char* dirname,
    const char* fname) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  dir_remove_state* state = (dir_remove_state*)user_data;
  SDL_PathInfo path_info;
  path dir_path;
  path name_path;
  path full_path;

  if (state == NULL || fname == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return SDL_ENUM_FAILURE;
  }

  dir_path = path_from_cstr(dirname != NULL ? dirname : "");
  name_path = path_from_cstr(fname);
  full_path = path_join(&dir_path, &name_path);

  if (!SDL_GetPathInfo(full_path.buf, &path_info)) {
    state->success = 0;
    TracyCZoneEnd(__tracy_zone_ctx);
    return SDL_ENUM_FAILURE;
  }

  if (path_info.type == SDL_PATHTYPE_DIRECTORY) {
    if (!SDL_EnumerateDirectory(full_path.buf, dir_remove_callback, state)) {
      state->success = 0;
      TracyCZoneEnd(__tracy_zone_ctx);
      return SDL_ENUM_FAILURE;
    }
  }

  if (!SDL_RemovePath(full_path.buf)) {
    state->success = 0;
    TracyCZoneEnd(__tracy_zone_ctx);
    return SDL_ENUM_FAILURE;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_ENUM_CONTINUE;
}

func b32 dir_copy_entry(const dir_entry* entry, void* user_data) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  dir_copy_state* state = (dir_copy_state*)user_data;
  path dst_path;

  if (state == NULL || entry == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  dst_path = path_join(&state->dst_root, &entry->relative_path);
  if (entry->is_directory) {
    state->success = dir_copy_recursive(
        &entry->full_path,
        &dst_path,
        state->overwrite_existing);
  } else {
    state->success = file_copy(&entry->full_path, &dst_path, state->overwrite_existing);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return state->success;
}

func b32 dir_path_is_same_or_child(const path* root_path, const path* child_path) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  path root_abs = path_resolve(root_path);
  path child_abs = path_resolve(child_path);
  sz root_len = cstr8_len(root_abs.buf);

  if (root_len == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (cstr8_cmp_n(root_abs.buf, child_abs.buf, root_len) != 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return child_abs.buf[root_len] == '\0' || child_abs.buf[root_len] == '/' ? 1 : 0;
}

func path dir_get_base(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return dir_path_from_string(SDL_GetBasePath());
}

func path dir_get_pref(cstr8 org_name, cstr8 app_name) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  cstr8 org_value = org_name;
  cstr8 app_value = app_name;

  if (org_value != NULL && org_value[0] == '\0') {
    org_value = NULL;
  }

  if (app_value == NULL || app_value[0] == '\0') {
    TracyCZoneEnd(__tracy_zone_ctx);
    return path_from_cstr("");
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return dir_path_from_owned_string(SDL_GetPrefPath(org_value, app_value));
}

func path dir_get_system(dir_system_path location) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Folder folder = dir_system_path_to_sdl(location);

  if (folder == SDL_FOLDER_COUNT) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return path_from_cstr("");
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return dir_path_from_string(SDL_GetUserFolder(folder));
}

func b32 dir_create(const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (src == NULL || src->buf[0] == '\0') {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(src->buf[0] != '\0');
  if (SDL_CreateDirectory(dir_path_cstr(src))) {
    thread_log_trace("dir_create: path=%s", src->buf);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return dir_exists(src);
}

func b32 dir_remove(const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!dir_exists(src)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(src != NULL);

  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_RemovePath(dir_path_cstr(src)) ? 1 : 0;
}

func b32 dir_rename(const path* old_src, const path* new_src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!dir_exists(old_src)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  if (new_src == NULL || new_src->buf[0] == '\0') {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(new_src->buf[0] != '\0');

  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_RenamePath(dir_path_cstr(old_src), dir_path_cstr(new_src)) ? 1 : 0;
}

func b32 dir_copy_recursive(const path* src, const path* dst, b32 overwrite_existing) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  dir_copy_state state;
  path src_abs;
  path dst_abs;

  if (!dir_exists(src) || dst == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(src != NULL);
  assert(dst != NULL);

  src_abs = path_resolve(src);
  dst_abs = path_resolve(dst);
  if (dir_path_is_same_or_child(&src_abs, &dst_abs)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return cstr8_cmp(src_abs.buf, dst_abs.buf) == 0 ? 1 : 0;
  }

  if (path_exists(dst)) {
    if (!dir_exists(dst)) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return 0;
    }
  } else if (!dir_create_recursive(dst)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  memset(&state, 0, size_of(state));
  state.dst_root = *dst;
  state.overwrite_existing = overwrite_existing;
  state.success = 1;

  if (!dir_iterate(src, dir_copy_entry, &state)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return state.success;
}

func b32 dir_exists(const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_PathInfo path_info;

  if (!SDL_GetPathInfo(dir_path_cstr(src), &path_info)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return path_info.type == SDL_PATHTYPE_DIRECTORY ? 1 : 0;
}

func b32 dir_create_recursive(const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  path cur_path = path_from_cstr(dir_path_cstr(src));
  sz root_len = 0;
  sz item_idx = 0;

  path_normalize(&cur_path);
  path_remove_trailing_slash(&cur_path);

  if (cstr8_is_empty(cur_path.buf)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(cur_path.buf[0] != '\0');

  root_len = dir_root_length(cur_path.buf);
  for (item_idx = root_len; cur_path.buf[item_idx] != '\0'; item_idx += 1) {
    if (cur_path.buf[item_idx] == '/') {
      c8 saved_chr = cur_path.buf[item_idx];
      cur_path.buf[item_idx] = '\0';
      if (!cstr8_is_empty(cur_path.buf) && !dir_exists(&cur_path) && !dir_create(&cur_path)) {
        cur_path.buf[item_idx] = saved_chr;
        TracyCZoneEnd(__tracy_zone_ctx);
        return 0;
      }
      cur_path.buf[item_idx] = saved_chr;
    }
  }

  if (!dir_exists(&cur_path) && !dir_create(&cur_path)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 dir_remove_recursive(const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  dir_remove_state state;

  if (!dir_exists(src)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(src != NULL);

  memset(&state, 0, size_of(state));
  state.success = 1;

  if (!SDL_EnumerateDirectory(dir_path_cstr(src), dir_remove_callback, &state)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (!state.success) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_RemovePath(dir_path_cstr(src)) ? 1 : 0;
}

func b32 dir_iterate(const path* src, dir_iterate_callback* callback, void* user_data) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  dir_iterate_state state;

  if (!dir_exists(src) || callback == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(callback != NULL);

  memset(&state, 0, size_of(state));
  state.root_path = *src;
  state.recursive = 0;
  state.callback = callback;
  state.user_data = user_data;
  state.success = 1;

  if (!SDL_EnumerateDirectory(dir_path_cstr(src), dir_enumerate_callback, &state)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return state.stop_requested ? 1 : 0;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return state.success;
}

func b32 dir_iterate_recursive(
    const path* src,
    dir_iterate_callback* callback,
    void* user_data) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  dir_iterate_state state;

  if (!dir_exists(src) || callback == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(callback != NULL);

  memset(&state, 0, size_of(state));
  state.root_path = *src;
  state.recursive = 1;
  state.callback = callback;
  state.user_data = user_data;
  state.success = 1;

  if (!SDL_EnumerateDirectory(dir_path_cstr(src), dir_enumerate_callback, &state)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return state.stop_requested ? 1 : 0;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return state.success;
}
