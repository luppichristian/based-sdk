// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/path.h"
#include "basic/assert.h"
#include "basic/env_defines.h"
#include "../sdl3_include.h"

#include "strings/char.h"
#include "basic/profiler.h"
#include "memory/memops.h"
#include "platform_includes.h"

#include <string.h>
#include "basic/safe.h"

func c8* path_mut_buf(path* value) {
  profile_func_begin;
  if (value == NULL) {
    profile_func_end;
    return NULL;
  }
  assert(value != NULL);
  profile_func_end;
  return value->buf;
}

func cstr8 path_buf(const path* value) {
  profile_func_begin;
  if (value == NULL) {
    profile_func_end;
    return "";
  }
  assert(value != NULL);
  profile_func_end;
  return value->buf;
}

func path path_empty_value(void) {
  path value;
  cstr8_clear(value.buf);
  return value;
}

func b32 path_is_separator(c8 chr) {
  return chr == '/' || chr == '\\';
}

func sz path_root_length_cstr(cstr8 src) {
  profile_func_begin;
  if (src[0] == '\0') {
    profile_func_end;
    return 0;
  }

  if (path_is_separator(src[0])) {
    if (path_is_separator(src[1])) {
      profile_func_end;
      return 2;
    }
    profile_func_end;
    return 1;
  }

  if (c8_is_alpha(src[0]) && src[1] != '\0' && src[1] == ':' && src[2] != '\0' &&
      path_is_separator(src[2])) {
    profile_func_end;
    return 3;
  }

  profile_func_end;
  return 0;
}

func sz path_trimmed_length_cstr(cstr8 src) {
  profile_func_begin;
  sz root_len = path_root_length_cstr(src);
  sz src_len = cstr8_len(src);

  safe_while (src_len > root_len && path_is_separator(src[src_len - 1])) {
    src_len -= 1;
  }

  profile_func_end;
  return src_len;
}

func sz path_name_start_cstr(cstr8 src) {
  profile_func_begin;
  sz root_len = path_root_length_cstr(src);
  sz src_len = path_trimmed_length_cstr(src);

  if (src_len == 0) {
    profile_func_end;
    return 0;
  }

  safe_while (src_len > root_len) {
    if (path_is_separator(src[src_len - 1])) {
      break;
    }
    src_len -= 1;
  }

  profile_func_end;
  return src_len;
}

func sz path_extension_start_cstr(cstr8 src) {
  profile_func_begin;
  sz name_idx = path_name_start_cstr(src);
  sz src_len = path_trimmed_length_cstr(src);

  safe_while (src_len > name_idx) {
    if (src[src_len - 1] == '.') {
      if (src_len - 1 == name_idx) {
        profile_func_end;
        return SZ_MAX;
      }
      profile_func_end;
      return src_len - 1;
    }

    if (path_is_separator(src[src_len - 1])) {
      break;
    }

    src_len -= 1;
  }

  profile_func_end;
  return SZ_MAX;
}

func b32 path_is_absolute_cstr(cstr8 src) {
  profile_func_begin;
  if (src[0] == '\0') {
    profile_func_end;
    return false;
  }

  if (path_is_separator(src[0])) {
    profile_func_end;
    return true;
  }

  if (c8_is_alpha(src[0]) && src[1] != '\0' && src[1] == ':' && src[2] != '\0' &&
      path_is_separator(src[2])) {
    profile_func_end;
    return true;
  }

  profile_func_end;
  return false;
}

func path path_from_cstr(cstr8 src) {
  profile_func_begin;
  path value;
  str8_from_cstr(value.buf, size_of(value.buf), src);
  profile_func_end;
  return value;
}

func path path_from_str8(str8 src) {
  profile_func_begin;
  path value = path_empty_value();
  cstr8_cpy_n(value.buf, size_of(value.buf), src.ptr, src.size);
  profile_func_end;
  return value;
}

func path path_join_cstr(const path* lhs, cstr8 rhs) {
  profile_func_begin;
  path result;
  if (lhs == NULL || rhs == NULL) {
    profile_func_end;
    return path_empty_value();
  }
  assert(lhs != NULL);
  assert(rhs != NULL);

  if (path_is_absolute_cstr(rhs) || cstr8_is_empty(path_buf(lhs))) {
    cstr8_cpy(result.buf, size_of(result.buf), rhs);
    path_norm(&result);
    profile_func_end;
    return result;
  }

  cstr8_cpy(result.buf, size_of(result.buf), path_buf(lhs));
  path_remove_trailing_slash(&result);

  safe_while (path_is_separator(*rhs)) {
    rhs += 1;
  }

  sz dst_len = cstr8_len(result.buf);
  if (*rhs != '\0' && dst_len > 0 && !path_is_separator(result.buf[dst_len - 1]) &&
      dst_len + 1 < size_of(result.buf)) {
    cstr8_append_char(result.buf, size_of(result.buf), '/');
  }

  cstr8_cat(result.buf, size_of(result.buf), rhs);
  path_norm(&result);
  profile_func_end;
  return result;
}

func path path_join(const path* lhs, const path* rhs) {
  profile_func_begin;
  path res = path_join_cstr(lhs, path_buf(rhs));
  profile_func_end;
  return res;
}

func sz path_append_cstr(path* dst, cstr8 src) {
  profile_func_begin;
  if (dst == NULL || src == NULL) {
    profile_func_end;
    return 0;
  }
  assert(dst != NULL);
  *dst = path_join_cstr(dst, src);
  profile_func_end;
  return cstr8_len(path_mut_buf(dst));
}

func sz path_append(path* dst, const path* src) {
  profile_func_begin;
  if (dst == NULL || src == NULL) {
    profile_func_end;
    return 0;
  }
  assert(dst != NULL);
  *dst = path_join(dst, src);
  profile_func_end;
  return cstr8_len(path_mut_buf(dst));
}

func void path_norm(path* src) {
  profile_func_begin;
  if (src == NULL) {
    profile_func_end;
    return;
  }
  assert(src != NULL);
  sz root_len = path_root_length_cstr(path_buf(src));
  sz read_idx = 0;
  sz write_idx = 0;
  b32 prev_sep = false;

  safe_while (src->buf[read_idx] != '\0') {
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
  profile_func_end;
}

func path path_norm_trimmed_cpy(const path* src) {
  profile_func_begin;
  path result = path_from_cstr(src != NULL ? src->buf : "");
  path_norm(&result);
  path_remove_trailing_slash(&result);
  profile_func_end;
  return result;
}

func b32 path_cmd_normd(const path* lhs, const path* rhs) {
  profile_func_begin;
  path lhs_norm = path_norm_trimmed_cpy(lhs);
  path rhs_norm = path_norm_trimmed_cpy(rhs);
  b32 is_equal = cstr8_cmp(lhs_norm.buf, rhs_norm.buf);
  profile_func_end;
  return is_equal;
}

func b32 path_ends_with(const path* src, cstr8 suffix) {
  profile_func_begin;
  if (src == NULL || suffix == NULL) {
    profile_func_end;
    return false;
  }
  assert(src != NULL);
  profile_func_end;
  return cstr8_ends_with(path_buf(src), suffix);
}

func b32 path_is_absolute(const path* src) {
  return path_is_absolute_cstr(path_buf(src));
}

func b32 path_is_relative(const path* src) {
  return !path_is_absolute(src) ? true : false;
}

func void path_remove_extension(path* src) {
  profile_func_begin;
  sz dot_idx = path_extension_start_cstr(path_buf(src));
  if (dot_idx != SZ_MAX) {
    cstr8_truncate(path_mut_buf(src), dot_idx);
  }
  profile_func_end;
}

func void path_remove_name(path* src) {
  profile_func_begin;
  sz cut_idx = path_name_start_cstr(path_buf(src));
  cstr8_truncate(path_mut_buf(src), cut_idx);
  path_remove_trailing_slash(src);
  profile_func_end;
}

func void path_remove_directory(path* src) {
  profile_func_begin;
  sz name_idx = path_name_start_cstr(path_buf(src));
  sz src_len = path_trimmed_length_cstr(path_buf(src));

  if (name_idx == 0) {
    src->buf[src_len] = '\0';
    profile_func_end;
    return;
  }

  mem_mv(src->buf, src->buf + name_idx, src_len - name_idx);
  src->buf[src_len - name_idx] = '\0';
  profile_func_end;
}

func void path_remove_trailing_slash(path* src) {
  profile_func_begin;
  sz root_len = path_root_length_cstr(path_buf(src));
  sz src_len = cstr8_len(path_buf(src));

  safe_while (src_len > root_len && path_is_separator(src->buf[src_len - 1])) {
    src_len -= 1;
  }

  src->buf[src_len] = '\0';
  profile_func_end;
}

func path path_get_extension(const path* src) {
  profile_func_begin;
  path result = path_empty_value();
  sz dot_idx = path_extension_start_cstr(path_buf(src));

  if (dot_idx != SZ_MAX) {
    cstr8_cpy(result.buf, size_of(result.buf), path_buf(src) + dot_idx);
  }

  profile_func_end;
  return result;
}

func path path_get_name(const path* src) {
  path result = path_empty_value();
  sz name_idx = path_name_start_cstr(path_buf(src));
  sz src_len = path_trimmed_length_cstr(path_buf(src));

  cstr8_cpy_n(result.buf, size_of(result.buf), path_buf(src) + name_idx, src_len - name_idx);
  return result;
}

func path path_get_basename(const path* src) {
  path result = path_get_name(src);
  path_remove_extension(&result);
  return result;
}

func path path_get_directory(const path* src) {
  path result = *src;
  path_remove_name(&result);
  return result;
}

func path path_get_common(const path* src_list, sz path_count) {
  profile_func_begin;
  path result = path_empty_value();
  path current;
  sz item_idx = 0;

  if (path_count == 0) {
    profile_func_end;
    return result;
  }

  result = src_list[0];
  path_norm(&result);

  safe_for (item_idx = 1; item_idx < path_count; item_idx += 1) {
    current = src_list[item_idx];
    path_norm(&current);
    cstr8_common_prefix(result.buf, current.buf, result.buf, size_of(result.buf));
  }

  path_remove_name(&result);
  profile_func_end;
  return result;
}

func path path_get_current(void) {
  profile_func_begin;
  path result = path_empty_value();

#if defined(PLATFORM_WINDOWS)
  if (_getcwd(result.buf, size_of(result.buf)) == NULL) {
    profile_func_end;
    return path_empty_value();
  }
#elif defined(PLATFORM_UNIX)
  if (getcwd(result.buf, size_of(result.buf)) == NULL) {
    profile_func_end;
    return path_empty_value();
  }
#else
  invalid_code_path;
  profile_func_end;
  return path_empty_value();
#endif

  path_norm(&result);
  path_remove_trailing_slash(&result);
  profile_func_end;
  return result;
}

func b32 path_set_current(const path* src) {
  profile_func_begin;
  if (src == NULL || src->buf[0] == '\0') {
    profile_func_end;
    return false;
  }

#if defined(PLATFORM_WINDOWS)
  profile_func_end;
  return _chdir(src->buf) == 0 ? true : false;
#elif defined(PLATFORM_UNIX)
  profile_func_end;
  return chdir(src->buf) == 0 ? true : false;
#else
  invalid_code_path;
  profile_func_end;
  return false;
#endif
}

func path path_resolve(const path* src) {
  profile_func_begin;
  path result;
  path cwd_path;

  if (src == NULL) {
    profile_func_end;
    return path_empty_value();
  }

  result = *src;
  if (path_is_absolute(&result)) {
    path_norm(&result);
    path_remove_trailing_slash(&result);
    profile_func_end;
    return result;
  }

  cwd_path = path_get_current();
  result = path_join(&cwd_path, &result);
  path_norm(&result);
  path_remove_trailing_slash(&result);
  profile_func_end;
  return result;
}

func path path_make_relative(const path* src, const path* root) {
  profile_func_begin;
  path src_abs;
  path root_abs;
  sz root_len = 0;

  if (src == NULL || root == NULL) {
    profile_func_end;
    return path_empty_value();
  }

  src_abs = path_resolve(src);
  root_abs = path_resolve(root);
  root_len = cstr8_len(root_abs.buf);

  if (root_len == 0) {
    profile_func_end;
    return src_abs;
  }

  if (!cstr8_cmp_n(src_abs.buf, root_abs.buf, root_len)) {
    profile_func_end;
    return src_abs;
  }

  if (src_abs.buf[root_len] == '\0') {
    profile_func_end;
    return path_from_cstr("");
  }

  if (src_abs.buf[root_len] != '/') {
    profile_func_end;
    return src_abs;
  }

  profile_func_end;
  return path_from_cstr(src_abs.buf + root_len + 1);
}

func b32 path_exists(const path* src) {
  profile_func_begin;
  SDL_PathInfo path_info;
  if (src == NULL || src->buf[0] == '\0') {
    profile_func_end;
    return false;
  }
  profile_func_end;
  return SDL_GetPathInfo(path_buf(src), &path_info) ? true : false;
}

func b32 path_remove(const path* src) {
  profile_func_begin;
  if (src == NULL || src->buf[0] == '\0') {
    profile_func_end;
    return false;
  }
  profile_func_end;
  return SDL_RemovePath(path_buf(src)) ? true : false;
}

func b32 path_rename(const path* old_src, const path* new_src) {
  profile_func_begin;
  if (old_src == NULL || new_src == NULL || old_src->buf[0] == '\0' || new_src->buf[0] == '\0') {
    profile_func_end;
    return false;
  }
  profile_func_end;
  return SDL_RenamePath(path_buf(old_src), path_buf(new_src)) ? true : false;
}

func timestamp path_get_last_write_time(const path* src) {
  profile_func_begin;
  SDL_PathInfo path_info;

  if (!SDL_GetPathInfo(path_buf(src), &path_info)) {
    profile_func_end;
    return timestamp_zero();
  }

  profile_func_end;
  return timestamp_from_microseconds((i64)SDL_NS_TO_US(path_info.modify_time));
}
