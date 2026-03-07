// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/pathinfo.h"

#include "basic/assert.h"
#include "basic/env_defines.h"
#include "filesystem/directory.h"
#include "filesystem/file.h"
#include "basic/profiler.h"

#if defined(PLATFORM_WINDOWS)

#  include <windows.h>

func timestamp filesystem_timestamp_from_filetime(FILETIME value) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  ULARGE_INTEGER raw_value;
  u64 total_microseconds = 0;

  raw_value.LowPart = value.dwLowDateTime;
  raw_value.HighPart = value.dwHighDateTime;
  if (raw_value.QuadPart == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return timestamp_zero();
  }

  total_microseconds = raw_value.QuadPart / 10;
  if (total_microseconds < 11644473600000000ULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return timestamp_zero();
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return timestamp_from_microseconds((i64)(total_microseconds - 11644473600000000ULL));
}

func pathinfo_type filesystem_kind_from_attributes(DWORD attributes) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if ((attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return PATHINFO_TYPE_SYMLINK;
  }

  if ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return PATHINFO_TYPE_DIRECTORY;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return PATHINFO_TYPE_FILE;
}

func void filesystem_info_set_windows_flags(pathinfo* info, DWORD attributes) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  info->is_read_only = (attributes & FILE_ATTRIBUTE_READONLY) != 0 ? 1 : 0;
  info->hidden = (attributes & FILE_ATTRIBUTE_HIDDEN) != 0 ? 1 : 0;
  TracyCZoneEnd(__tracy_zone_ctx);
}

#elif defined(PLATFORM_UNIX) || defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)

#  include <sys/stat.h>
#  include <unistd.h>

func cstr8 filesystem_name_ptr(cstr8 src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  cstr8 last_sep = src;
  sz item_idx = 0;

  if (src == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return "";
  }

  for (item_idx = 0; src[item_idx] != '\0'; item_idx += 1) {
    if (src[item_idx] == '/' || src[item_idx] == '\\') {
      last_sep = src + item_idx + 1;
    }
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return last_sep;
}

func pathinfo_type filesystem_kind_from_mode(mode_t mode_value) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (S_ISLNK(mode_value)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return PATHINFO_TYPE_SYMLINK;
  }

  if (S_ISDIR(mode_value)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return PATHINFO_TYPE_DIRECTORY;
  }

  if (S_ISREG(mode_value)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return PATHINFO_TYPE_FILE;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return PATHINFO_TYPE_OTHER;
}

func timestamp filesystem_timestamp_from_timespec(struct timespec value) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  i64 microseconds = (i64)value.tv_sec * 1000000;
  microseconds += (i64)(value.tv_nsec / 1000);
  TracyCZoneEnd(__tracy_zone_ctx);
  return timestamp_from_microseconds(microseconds);
}

#else

func cstr8 filesystem_name_ptr(cstr8 src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  cstr8 last_sep = src;
  sz item_idx = 0;

  if (src == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return "";
  }

  for (item_idx = 0; src[item_idx] != '\0'; item_idx += 1) {
    if (src[item_idx] == '/' || src[item_idx] == '\\') {
      last_sep = src + item_idx + 1;
    }
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return last_sep;
}

#endif

func pathinfo filesystem_info_empty(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  pathinfo info;

  info.kind = PATHINFO_TYPE_NONE;
  info.size = 0;
  info.create_time = timestamp_zero();
  info.access_time = timestamp_zero();
  info.write_time = timestamp_zero();
  info.exists = 0;
  info.is_read_only = 0;
  info.hidden = 0;
  TracyCZoneEnd(__tracy_zone_ctx);
  return info;
}

func b32 pathinfo_get(const path* src, pathinfo* out_info) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  pathinfo info = filesystem_info_empty();
  cstr8 src_str = src != NULL ? src->buf : "";

  if (out_info == NULL || src == NULL || src->buf[0] == '\0') {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(src->buf[0] != '\0');

#if defined(PLATFORM_WINDOWS)
  WIN32_FILE_ATTRIBUTE_DATA attr_data;

  if (!GetFileAttributesExA(src_str, GetFileExInfoStandard, &attr_data)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  info.kind = filesystem_kind_from_attributes(attr_data.dwFileAttributes);
  info.exists = 1;
  info.size = (sz)(((u64)attr_data.nFileSizeHigh << 32) | attr_data.nFileSizeLow);
  info.create_time = filesystem_timestamp_from_filetime(attr_data.ftCreationTime);
  info.access_time = filesystem_timestamp_from_filetime(attr_data.ftLastAccessTime);
  info.write_time = filesystem_timestamp_from_filetime(attr_data.ftLastWriteTime);
  filesystem_info_set_windows_flags(&info, attr_data.dwFileAttributes);
#elif defined(PLATFORM_UNIX) || defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
  struct stat stat_info;
  cstr8 name_ptr = NULL;

  if (lstat(src_str, &stat_info) != 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  info.kind = filesystem_kind_from_mode(stat_info.st_mode);
  info.exists = 1;
  info.size = (sz)stat_info.st_size;
#  if defined(PLATFORM_MACOS) || defined(PLATFORM_IOS)
  info.access_time = filesystem_timestamp_from_timespec(stat_info.st_atimespec);
  info.write_time = filesystem_timestamp_from_timespec(stat_info.st_mtimespec);
  info.create_time = filesystem_timestamp_from_timespec(stat_info.st_ctimespec);
#  else
  info.access_time = filesystem_timestamp_from_timespec(stat_info.st_atim);
  info.write_time = filesystem_timestamp_from_timespec(stat_info.st_mtim);
  info.create_time = filesystem_timestamp_from_timespec(stat_info.st_ctim);
#  endif
  info.is_read_only = (stat_info.st_mode & (S_IWUSR | S_IWGRP | S_IWOTH)) == 0 ? 1 : 0;

  name_ptr = filesystem_name_ptr(src_str);
  info.hidden = (name_ptr[0] == '.' && name_ptr[1] != '\0') ? 1 : 0;
#else
  info.exists = path_exists(src);
  if (!info.exists) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (dir_exists(src)) {
    info.kind = PATHINFO_TYPE_DIRECTORY;
  } else if (file_exists(src)) {
    info.kind = PATHINFO_TYPE_FILE;
    (void)file_get_size(src, &info.size);
  } else {
    info.kind = PATHINFO_TYPE_OTHER;
  }

  info.write_time = path_get_last_write_time(src);
  info.create_time = info.write_time;
  info.access_time = info.write_time;
  info.is_read_only = 0;
  info.hidden = (filesystem_name_ptr(src_str)[0] == '.') ? 1 : 0;
#endif

  *out_info = info;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}
