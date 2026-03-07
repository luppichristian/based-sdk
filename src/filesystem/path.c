// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/path.h"
#include "basic/assert.h"
#include "basic/env_defines.h"
#include "../sdl3_include.h"

#include "strings/char.h"
#include "basic/profiler.h"

#include <string.h>

#if defined(PLATFORM_WINDOWS)
#  include <direct.h>
#elif defined(PLATFORM_UNIX) || defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
#  include <unistd.h>
#endif

func c8* path_mut_buf(path* value) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (value == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  assert(value != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return value->buf;
}

func cstr8 path_buf(const path* value) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (value == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return "";
  }
  assert(value != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return value->buf;
}

func path path_empty_value(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  path value;
  cstr8_clear(value.buf);
  TracyCZoneEnd(__tracy_zone_ctx);
  return value;
}

func b32 path_is_separator(c8 chr) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return chr == '/' || chr == '\\';
}

func sz path_root_length_cstr(cstr8 src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (src[0] == '\0') {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (path_is_separator(src[0])) {
    if (path_is_separator(src[1])) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return 2;
    }
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  if (c8_is_alpha(src[0]) && src[1] != '\0' && src[1] == ':' && src[2] != '\0' &&
      path_is_separator(src[2])) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 3;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
}

func sz path_trimmed_length_cstr(cstr8 src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz root_len = path_root_length_cstr(src);
  sz src_len = cstr8_len(src);

  while (src_len > root_len && path_is_separator(src[src_len - 1])) {
    src_len -= 1;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return src_len;
}

func sz path_name_start_cstr(cstr8 src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz root_len = path_root_length_cstr(src);
  sz src_len = path_trimmed_length_cstr(src);

  if (src_len == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  while (src_len > root_len) {
    if (path_is_separator(src[src_len - 1])) {
      break;
    }
    src_len -= 1;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return src_len;
}

func sz path_extension_start_cstr(cstr8 src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz name_idx = path_name_start_cstr(src);
  sz src_len = path_trimmed_length_cstr(src);

  while (src_len > name_idx) {
    if (src[src_len - 1] == '.') {
      if (src_len - 1 == name_idx) {
        TracyCZoneEnd(__tracy_zone_ctx);
        return SZ_MAX;
      }
      TracyCZoneEnd(__tracy_zone_ctx);
      return src_len - 1;
    }

    if (path_is_separator(src[src_len - 1])) {
      break;
    }

    src_len -= 1;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return SZ_MAX;
}

func b32 path_is_absolute_cstr(cstr8 src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (src[0] == '\0') {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (path_is_separator(src[0])) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  if (c8_is_alpha(src[0]) && src[1] != '\0' && src[1] == ':' && src[2] != '\0' &&
      path_is_separator(src[2])) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
}

func path path_from_cstr(cstr8 src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  path value;
  str8_from_cstr(value.buf, size_of(value.buf), src);
  TracyCZoneEnd(__tracy_zone_ctx);
  return value;
}

func path path_from_str8(str8 src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  path value = path_empty_value();
  cstr8_copy_n(value.buf, size_of(value.buf), src.ptr, src.size);
  TracyCZoneEnd(__tracy_zone_ctx);
  return value;
}

func path path_join_cstr(const path* lhs, cstr8 rhs) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  path result;
  if (lhs == NULL || rhs == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return path_empty_value();
  }
  assert(lhs != NULL);
  assert(rhs != NULL);

  if (path_is_absolute_cstr(rhs) || cstr8_is_empty(path_buf(lhs))) {
    cstr8_copy(result.buf, size_of(result.buf), rhs);
    path_normalize(&result);
    TracyCZoneEnd(__tracy_zone_ctx);
    return result;
  }

  cstr8_copy(result.buf, size_of(result.buf), path_buf(lhs));
  path_remove_trailing_slash(&result);

  while (path_is_separator(*rhs)) {
    rhs += 1;
  }

  sz dst_len = cstr8_len(result.buf);
  if (*rhs != '\0' && dst_len > 0 && !path_is_separator(result.buf[dst_len - 1]) &&
      dst_len + 1 < size_of(result.buf)) {
    cstr8_append_char(result.buf, size_of(result.buf), '/');
  }

  cstr8_cat(result.buf, size_of(result.buf), rhs);
  path_normalize(&result);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func path path_join(const path* lhs, const path* rhs) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return path_join_cstr(lhs, path_buf(rhs));
}

func sz path_append_cstr(path* dst, cstr8 src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (dst == NULL || src == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(dst != NULL);
  *dst = path_join_cstr(dst, src);
  TracyCZoneEnd(__tracy_zone_ctx);
  return cstr8_len(path_mut_buf(dst));
}

func sz path_append(path* dst, const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (dst == NULL || src == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(dst != NULL);
  *dst = path_join(dst, src);
  TracyCZoneEnd(__tracy_zone_ctx);
  return cstr8_len(path_mut_buf(dst));
}

func void path_normalize(path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (src == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(src != NULL);
  sz root_len = path_root_length_cstr(path_buf(src));
  sz read_idx = 0;
  sz write_idx = 0;
  b32 prev_sep = 0;

  while (src->buf[read_idx] != '\0') {
    c8 chr = src->buf[read_idx];
    if (chr == '\\') {
      chr = '/';
    }

    if (chr == '/') {
      if (prev_sep && !(write_idx < root_len && root_len == 2)) {
        read_idx += 1;
        continue;
      }
      prev_sep = 1;
    } else {
      prev_sep = 0;
    }

    src->buf[write_idx] = chr;
    write_idx += 1;
    read_idx += 1;
  }

  src->buf[write_idx] = '\0';
  TracyCZoneEnd(__tracy_zone_ctx);
}

func b32 path_ends_with(const path* src, cstr8 suffix) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (src == NULL || suffix == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(src != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return cstr8_ends_with(path_buf(src), suffix);
}

func b32 path_is_absolute(const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return path_is_absolute_cstr(path_buf(src));
}

func b32 path_is_relative(const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return !path_is_absolute(src) ? 1 : 0;
}

func void path_remove_extension(path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz dot_idx = path_extension_start_cstr(path_buf(src));
  if (dot_idx != SZ_MAX) {
    cstr8_truncate(path_mut_buf(src), dot_idx);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void path_remove_name(path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz cut_idx = path_name_start_cstr(path_buf(src));
  cstr8_truncate(path_mut_buf(src), cut_idx);
  path_remove_trailing_slash(src);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void path_remove_directory(path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz name_idx = path_name_start_cstr(path_buf(src));
  sz src_len = path_trimmed_length_cstr(path_buf(src));

  if (name_idx == 0) {
    src->buf[src_len] = '\0';
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  memmove(src->buf, src->buf + name_idx, src_len - name_idx);
  src->buf[src_len - name_idx] = '\0';
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void path_remove_trailing_slash(path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz root_len = path_root_length_cstr(path_buf(src));
  sz src_len = cstr8_len(path_buf(src));

  while (src_len > root_len && path_is_separator(src->buf[src_len - 1])) {
    src_len -= 1;
  }

  src->buf[src_len] = '\0';
  TracyCZoneEnd(__tracy_zone_ctx);
}

func path path_get_extension(const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  path result = path_empty_value();
  sz dot_idx = path_extension_start_cstr(path_buf(src));

  if (dot_idx != SZ_MAX) {
    cstr8_copy(result.buf, size_of(result.buf), path_buf(src) + dot_idx);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func path path_get_name(const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  path result = path_empty_value();
  sz name_idx = path_name_start_cstr(path_buf(src));
  sz src_len = path_trimmed_length_cstr(path_buf(src));

  cstr8_copy_n(result.buf, size_of(result.buf), path_buf(src) + name_idx, src_len - name_idx);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func path path_get_basename(const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  path result = path_get_name(src);
  path_remove_extension(&result);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func path path_get_directory(const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  path result = *src;
  path_remove_name(&result);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func path path_get_common(const path* src_list, sz path_count) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  path result = path_empty_value();
  path current;
  sz item_idx = 0;

  if (path_count == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return result;
  }

  result = src_list[0];
  path_normalize(&result);

  for (item_idx = 1; item_idx < path_count; item_idx += 1) {
    current = src_list[item_idx];
    path_normalize(&current);
    cstr8_common_prefix(result.buf, current.buf, result.buf, size_of(result.buf));
  }

  path_remove_name(&result);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func path path_get_current(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  path result = path_empty_value();

#if defined(PLATFORM_WINDOWS)
  if (_getcwd(result.buf, size_of(result.buf)) == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return path_empty_value();
  }
#elif defined(PLATFORM_UNIX) || defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
  if (getcwd(result.buf, size_of(result.buf)) == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return path_empty_value();
  }
#else
  TracyCZoneEnd(__tracy_zone_ctx);
  return path_empty_value();
#endif

  path_normalize(&result);
  path_remove_trailing_slash(&result);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func b32 path_set_current(const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (src == NULL || src->buf[0] == '\0') {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

#if defined(PLATFORM_WINDOWS)
  TracyCZoneEnd(__tracy_zone_ctx);
  return _chdir(src->buf) == 0 ? 1 : 0;
#elif defined(PLATFORM_UNIX) || defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
  TracyCZoneEnd(__tracy_zone_ctx);
  return chdir(src->buf) == 0 ? 1 : 0;
#else
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
#endif
}

func path path_resolve(const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  path result;
  path cwd_path;

  if (src == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return path_empty_value();
  }

  result = *src;
  if (path_is_absolute(&result)) {
    path_normalize(&result);
    path_remove_trailing_slash(&result);
    TracyCZoneEnd(__tracy_zone_ctx);
    return result;
  }

  cwd_path = path_get_current();
  result = path_join(&cwd_path, &result);
  path_normalize(&result);
  path_remove_trailing_slash(&result);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func path path_make_relative(const path* src, const path* root) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  path src_abs;
  path root_abs;
  sz root_len = 0;

  if (src == NULL || root == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return path_empty_value();
  }

  src_abs = path_resolve(src);
  root_abs = path_resolve(root);
  root_len = cstr8_len(root_abs.buf);

  if (root_len == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return src_abs;
  }

  if (cstr8_cmp_n(src_abs.buf, root_abs.buf, root_len) != 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return src_abs;
  }

  if (src_abs.buf[root_len] == '\0') {
    TracyCZoneEnd(__tracy_zone_ctx);
    return path_from_cstr("");
  }

  if (src_abs.buf[root_len] != '/') {
    TracyCZoneEnd(__tracy_zone_ctx);
    return src_abs;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return path_from_cstr(src_abs.buf + root_len + 1);
}

func b32 path_exists(const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_PathInfo path_info;
  if (src == NULL || src->buf[0] == '\0') {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_GetPathInfo(path_buf(src), &path_info) ? 1 : 0;
}

func b32 path_remove(const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (src == NULL || src->buf[0] == '\0') {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_RemovePath(path_buf(src)) ? 1 : 0;
}

func b32 path_rename(const path* old_src, const path* new_src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (old_src == NULL || new_src == NULL || old_src->buf[0] == '\0' || new_src->buf[0] == '\0') {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_RenamePath(path_buf(old_src), path_buf(new_src)) ? 1 : 0;
}

func timestamp path_get_last_write_time(const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_PathInfo path_info;

  if (!SDL_GetPathInfo(path_buf(src), &path_info)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return timestamp_zero();
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return timestamp_from_microseconds((i64)SDL_NS_TO_US(path_info.modify_time));
}
