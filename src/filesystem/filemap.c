// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/filemap.h"

#include "basic/assert.h"
#include "basic/env_defines.h"
#include "context/global_ctx.h"
#include "context/thread_ctx.h"
#include "filesystem/file.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "basic/profiler.h"

#include <stdio.h>
#include <string.h>

#if defined(PLATFORM_WINDOWS)

#  include <windows.h>

#elif defined(PLATFORM_UNIX)

#  include <fcntl.h>
#  include <sys/mman.h>
#  include <sys/stat.h>
#  include <unistd.h>

#endif

global_var thread_local filemap_error filemap_last_error = FILEMAP_ERROR_NONE;

func void filemap_set_error(filemap_error error_code) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  filemap_last_error = error_code;
  TracyCZoneEnd(__tracy_zone_ctx);
}

func filemap filemap_empty(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  filemap map;
  memset(&map, 0, size_of(map));
  map.source_path = path_from_cstr("");
  TracyCZoneEnd(__tracy_zone_ctx);
  return map;
}

func allocator filemap_allocator_resolve(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator alloc = thread_get_allocator();
  if (alloc.alloc_fn != NULL && alloc.dealloc_fn != NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return alloc;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return global_get_allocator();
}

func b32 filemap_is_open(const filemap* map) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (map == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (map->native_file != NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return map->uses_fallback_copy && map->source_path.buf[0] != '\0' ? 1 : 0;
}

func b32 filemap_is_writable(const filemap* map) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  b32 result = map != NULL && map->writable != 0;
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func void filemap_mark_dirty(filemap* map) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (map != NULL && map->writable) {
    map->dirty = 1;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}

func filemap_error filemap_get_last_error(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  filemap_error result = filemap_last_error;
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func b32 filemap_flush(filemap* map) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!filemap_is_open(map)) {
    filemap_set_error(FILEMAP_ERROR_INVALID_ARGUMENT);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (!map->writable) {
    filemap_set_error(FILEMAP_ERROR_NONE);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  if (!map->dirty) {
    filemap_set_error(FILEMAP_ERROR_NONE);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }
  assert(map->source_path.buf[0] != '\0');

  if (map->uses_fallback_copy) {
    buffer data = buffer_from(map->data_ptr, map->data_size);
    if (!file_write_all(&map->source_path, data)) {
      filemap_set_error(FILEMAP_ERROR_IO_FAILED);
      TracyCZoneEnd(__tracy_zone_ctx);
      return 0;
    }
    map->dirty = 0;
    filemap_set_error(FILEMAP_ERROR_NONE);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

#if defined(PLATFORM_WINDOWS)
  if (map->data_ptr != NULL && map->data_size > 0 && !FlushViewOfFile(map->data_ptr, (SIZE_T)map->data_size)) {
    filemap_set_error(FILEMAP_ERROR_IO_FAILED);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (map->native_file != NULL && !FlushFileBuffers((HANDLE)map->native_file)) {
    filemap_set_error(FILEMAP_ERROR_IO_FAILED);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  map->dirty = 0;
  filemap_set_error(FILEMAP_ERROR_NONE);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
#elif defined(PLATFORM_UNIX)
  if (map->data_ptr != NULL && map->data_size > 0) {
    if (msync(map->data_ptr, map->data_size, MS_SYNC) != 0) {
      filemap_set_error(FILEMAP_ERROR_IO_FAILED);
      TracyCZoneEnd(__tracy_zone_ctx);
      return 0;
    }
  }
  map->dirty = 0;
  filemap_set_error(FILEMAP_ERROR_NONE);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
#else
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
#endif
}

func void filemap_close(filemap* map) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator alloc = filemap_allocator_resolve();
  if (map == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(map->data_size == 0 || map->source_path.buf[0] != '\0');

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_FILEMAP,
                                                     .object_ptr = map,
                                                 });
  (void)msg_post(&lifecycle_msg);

  if (map->uses_fallback_copy) {
    if (map->writable) {
      (void)filemap_flush(map);
    }

    if (map->data_ptr != NULL) {
      allocator_dealloc(&alloc, map->data_ptr, map->data_size);
    }
    thread_log_trace("filemap_close: fallback path=%s", map->source_path.buf);
    *map = filemap_empty();
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

#if defined(PLATFORM_WINDOWS)
  if (map->data_ptr != NULL) {
    UnmapViewOfFile(map->data_ptr);
  }

  if (map->native_mapping != NULL) {
    CloseHandle((HANDLE)map->native_mapping);
  }

  if (map->native_file != NULL) {
    CloseHandle((HANDLE)map->native_file);
  }
#elif defined(PLATFORM_UNIX)
  if (map->data_ptr != NULL && map->data_size > 0) {
    munmap(map->data_ptr, map->data_size);
  }

  if (map->native_file != NULL) {
    close((i32)(up)map->native_file - 1);
  }
#endif

  *map = filemap_empty();
  thread_log_trace("filemap_close: mapped");
  TracyCZoneEnd(__tracy_zone_ctx);
}

func filemap filemap_open(const path* src, filemap_access access) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator alloc = filemap_allocator_resolve();
  filemap map = filemap_empty();

  if (src == NULL || !file_exists(src)) {
    filemap_set_error(FILEMAP_ERROR_OPEN_FAILED);
    TracyCZoneEnd(__tracy_zone_ctx);
    return map;
  }
  assert(access == FILEMAP_ACCESS_READ || access == FILEMAP_ACCESS_READ_WRITE || access == FILEMAP_ACCESS_COPY_ON_WRITE);

  map.source_path = *src;
  map.writable = access != FILEMAP_ACCESS_READ ? 1 : 0;
  map.dirty = access == FILEMAP_ACCESS_COPY_ON_WRITE ? 1 : 0;
  filemap_set_error(FILEMAP_ERROR_NONE);

#if defined(PLATFORM_WINDOWS)
  HANDLE file_handle = INVALID_HANDLE_VALUE;
  HANDLE mapping_handle = NULL;
  DWORD desired_access = GENERIC_READ;
  DWORD protect_flags = PAGE_READONLY;
  DWORD view_access = FILE_MAP_READ;
  LARGE_INTEGER file_size;

  if (access == FILEMAP_ACCESS_READ_WRITE) {
    desired_access = GENERIC_READ | GENERIC_WRITE;
    protect_flags = PAGE_READWRITE;
    view_access = FILE_MAP_READ | FILE_MAP_WRITE;
  } else if (access == FILEMAP_ACCESS_COPY_ON_WRITE) {
    protect_flags = PAGE_WRITECOPY;
    view_access = FILE_MAP_COPY;
  }

  file_handle = CreateFileA(
      src->buf,
      desired_access,
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      NULL,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      NULL);
  if (file_handle == INVALID_HANDLE_VALUE) {
    filemap_set_error(FILEMAP_ERROR_OPEN_FAILED);
    TracyCZoneEnd(__tracy_zone_ctx);
    return filemap_empty();
  }

  if (!GetFileSizeEx(file_handle, &file_size)) {
    filemap_set_error(FILEMAP_ERROR_IO_FAILED);
    CloseHandle(file_handle);
    TracyCZoneEnd(__tracy_zone_ctx);
    return filemap_empty();
  }

  map.native_file = file_handle;
  map.data_size = (sz)file_size.QuadPart;
  if (map.data_size == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return map;
  }

  mapping_handle = CreateFileMappingA(
      file_handle,
      NULL,
      protect_flags,
      (DWORD)(file_size.QuadPart >> 32),
      (DWORD)(file_size.QuadPart & 0xffffffff),
      NULL);
  if (mapping_handle == NULL) {
    filemap_set_error(FILEMAP_ERROR_MAP_FAILED);
    filemap_close(&map);
    TracyCZoneEnd(__tracy_zone_ctx);
    return filemap_empty();
  }

  map.native_mapping = mapping_handle;
  map.data_ptr = MapViewOfFile(mapping_handle, view_access, 0, 0, (SIZE_T)map.data_size);
  if (map.data_ptr == NULL) {
    filemap_set_error(FILEMAP_ERROR_MAP_FAILED);
    filemap_close(&map);
    TracyCZoneEnd(__tracy_zone_ctx);
    return filemap_empty();
  }

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_FILEMAP,
                                                     .object_ptr = &map,
                                                 });
  (void)msg_post(&lifecycle_msg);
  thread_log_trace("filemap_open: windows path=%s size=%zu writable=%u", src->buf, (size_t)map.data_size, (u32)map.writable);
  TracyCZoneEnd(__tracy_zone_ctx);
  return map;
#elif defined(PLATFORM_UNIX)
  i32 open_flags = O_RDONLY;
  i32 prot_flags = PROT_READ;
  i32 map_flags = MAP_PRIVATE;
  i32 file_desc = -1;
  struct stat stat_info;

  if (access == FILEMAP_ACCESS_READ_WRITE) {
    open_flags = O_RDWR;
    prot_flags = PROT_READ | PROT_WRITE;
    map_flags = MAP_SHARED;
  } else if (access == FILEMAP_ACCESS_COPY_ON_WRITE) {
    prot_flags = PROT_READ | PROT_WRITE;
    map_flags = MAP_PRIVATE;
  }

  file_desc = open(src->buf, open_flags);
  if (file_desc < 0) {
    filemap_set_error(FILEMAP_ERROR_OPEN_FAILED);
    TracyCZoneEnd(__tracy_zone_ctx);
    return filemap_empty();
  }

  if (fstat(file_desc, &stat_info) != 0) {
    filemap_set_error(FILEMAP_ERROR_IO_FAILED);
    close(file_desc);
    TracyCZoneEnd(__tracy_zone_ctx);
    return filemap_empty();
  }

  map.native_file = (void*)(up)(file_desc + 1);
  map.data_size = (sz)stat_info.st_size;
  if (map.data_size == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return map;
  }

  map.data_ptr = mmap(NULL, map.data_size, prot_flags, map_flags, file_desc, 0);
  if (map.data_ptr == MAP_FAILED) {
    filemap_set_error(FILEMAP_ERROR_MAP_FAILED);
    filemap_close(&map);
    TracyCZoneEnd(__tracy_zone_ctx);
    return filemap_empty();
  }

  map.native_mapping = (void*)(up)map_flags;
  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_FILEMAP,
                                                     .object_ptr = &map,
                                                 });
  (void)msg_post(&lifecycle_msg);
  thread_log_trace("filemap_open: unix path=%s size=%zu writable=%u", src->buf, (size_t)map.data_size, (u32)map.writable);
  TracyCZoneEnd(__tracy_zone_ctx);
  return map;
#else
  buffer file_data = {0};

  if (!file_read_all(src, &alloc, &file_data)) {
    filemap_set_error(FILEMAP_ERROR_IO_FAILED);
    TracyCZoneEnd(__tracy_zone_ctx);
    return filemap_empty();
  }
  map.data_ptr = file_data.ptr;
  map.data_size = file_data.size;
  map.uses_fallback_copy = 1;
  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_FILEMAP,
                                                     .object_ptr = &map,
                                                 });
  (void)msg_post(&lifecycle_msg);
  thread_log_trace("filemap_open: fallback path=%s size=%zu writable=%u", src->buf, (size_t)map.data_size, (u32)map.writable);
  TracyCZoneEnd(__tracy_zone_ctx);
  return map;
#endif
}
