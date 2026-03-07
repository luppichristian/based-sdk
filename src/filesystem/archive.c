// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/archive.h"
#include "basic/assert.h"
#include "basic/utility_defines.h"
#include "context/global_ctx.h"
#include "context/thread_ctx.h"
#include "filesystem/file.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "basic/profiler.h"

#include <string.h>

#if defined(__has_include)
#  if __has_include(<miniz.h>)
#    include <miniz.h>
#    define BASED_ARCHIVE_HAS_MINIZ 1
#  endif
#endif

#ifndef BASED_ARCHIVE_HAS_MINIZ
#  define BASED_ARCHIVE_HAS_MINIZ 0
#endif

func allocator archive_allocator_resolve(allocator* opt_alloc) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (opt_alloc != NULL && opt_alloc->alloc_fn != NULL && opt_alloc->dealloc_fn != NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return *opt_alloc;
  }

  allocator thread_alloc = thread_get_allocator();
  if (thread_alloc.alloc_fn != NULL && thread_alloc.dealloc_fn != NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return thread_alloc;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return global_get_allocator();
}

func void* archive_alloc_bytes(allocator* opt_alloc, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator resolved_alloc = archive_allocator_resolve(opt_alloc);
  if (size == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }

  assert(resolved_alloc.alloc_fn != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return allocator_alloc(&resolved_alloc, size);
}

func void* archive_realloc_bytes(allocator* opt_alloc, void* ptr, sz old_size, sz new_size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator resolved_alloc = archive_allocator_resolve(opt_alloc);
  if (new_size == 0) {
    if (ptr != NULL) {
      allocator_dealloc(&resolved_alloc, ptr, old_size);
    }
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }

  assert(resolved_alloc.alloc_fn != NULL);
  assert(resolved_alloc.dealloc_fn != NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return allocator_realloc(&resolved_alloc, ptr, old_size, new_size);
}

func void archive_dealloc_bytes(allocator* opt_alloc, void* ptr, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator resolved_alloc = archive_allocator_resolve(opt_alloc);
  if (ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  assert(resolved_alloc.dealloc_fn != NULL);
  allocator_dealloc(&resolved_alloc, ptr, size);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func path archive_normalize_entry_path(const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  path item_path = path_from_cstr(src != NULL ? src->buf : "");
  path_normalize(&item_path);
  path_remove_trailing_slash(&item_path);
  TracyCZoneEnd(__tracy_zone_ctx);
  return item_path;
}

func b32 archive_path_equals(const path* lhs, const path* rhs) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  path lhs_norm = archive_normalize_entry_path(lhs);
  path rhs_norm = archive_normalize_entry_path(rhs);
  TracyCZoneEnd(__tracy_zone_ctx);
  return cstr8_cmp(lhs_norm.buf, rhs_norm.buf) == 0 ? 1 : 0;
}

func sz archive_find_idx(const archive* arc, const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz item_idx = 0;

  if (arc == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return SZ_MAX;
  }

  for (item_idx = 0; item_idx < arc->entry_count; item_idx += 1) {
    if (archive_path_equals(&arc->entries[item_idx].item_path, src)) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return item_idx;
    }
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return SZ_MAX;
}

func b32 archive_reserve(archive* arc, sz min_capacity) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  archive_entry* new_entries = NULL;
  sz new_capacity = 0;
  if (arc == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(arc->entry_count <= arc->entry_capacity);

  if (arc->entry_capacity >= min_capacity) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  new_capacity = arc->entry_capacity == 0 ? 8 : arc->entry_capacity;
  while (new_capacity < min_capacity) {
    if (new_capacity > SZ_MAX / 2) {
      new_capacity = min_capacity;
      break;
    }
    new_capacity *= 2;
  }

  new_entries = (archive_entry*)archive_realloc_bytes(
      arc->opt_alloc,
      arc->entries,
      arc->entry_capacity * size_of(archive_entry),
      new_capacity * size_of(archive_entry));
  if (new_entries == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  arc->entries = new_entries;
  arc->entry_capacity = new_capacity;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func void archive_reset_entry(archive* arc, archive_entry* ent) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  assert(arc != NULL);
  assert(ent != NULL);
  archive_dealloc_bytes(arc->opt_alloc, ent->data_ptr, ent->data_capacity);
  ent->data_ptr = NULL;
  ent->data_size = 0;
  ent->data_capacity = 0;
  ent->is_directory = 0;
  ent->item_path = path_from_cstr("");
  TracyCZoneEnd(__tracy_zone_ctx);
}

func b32 archive_assign_entry_bytes(archive* arc, archive_entry* ent, buffer data) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  void* data_ptr = NULL;
  if (arc == NULL || ent == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(arc->entry_count <= arc->entry_capacity);

  archive_dealloc_bytes(arc->opt_alloc, ent->data_ptr, ent->data_capacity);
  ent->data_ptr = NULL;
  ent->data_size = 0;
  ent->data_capacity = 0;

  if (data.size == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }
  if (data.ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(arc != NULL);
  assert(ent != NULL);

  data_ptr = archive_alloc_bytes(arc->opt_alloc, data.size);
  if (data_ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  memcpy(data_ptr, data.ptr, data.size);
  ent->data_ptr = (u8*)data_ptr;
  ent->data_size = data.size;
  ent->data_capacity = data.size;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 archive_add_empty_entry(archive* arc, const path* src, b32 is_directory, sz* out_idx) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  archive_entry* ent = NULL;
  sz item_idx = SZ_MAX;

  if (arc == NULL || src == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(is_directory == 0 || is_directory == 1);
  item_idx = archive_find_idx(arc, src);

  if (item_idx != SZ_MAX) {
    ent = &arc->entries[item_idx];
    ent->item_path = archive_normalize_entry_path(src);
    ent->is_directory = is_directory;
    if (is_directory) {
      archive_dealloc_bytes(arc->opt_alloc, ent->data_ptr, ent->data_capacity);
      ent->data_ptr = NULL;
      ent->data_size = 0;
      ent->data_capacity = 0;
    }
    if (out_idx != NULL) {
      *out_idx = item_idx;
    }
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  if (!archive_reserve(arc, arc->entry_count + 1)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  ent = &arc->entries[arc->entry_count];
  memset(ent, 0, size_of(*ent));
  ent->item_path = archive_normalize_entry_path(src);
  ent->is_directory = is_directory;
  if (out_idx != NULL) {
    *out_idx = arc->entry_count;
  }
  arc->entry_count += 1;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 archive_read_disk_bytes(const path* src, allocator* alloc, void** out_ptr, sz* out_size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  buffer file_data = {0};

  if (src == NULL || alloc == NULL || out_ptr == NULL || out_size == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  if (alloc->alloc_fn == NULL || alloc->dealloc_fn == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(src->buf[0] != '\0');

  *out_ptr = NULL;
  *out_size = 0;
  if (!file_read_all(src, alloc, &file_data)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  *out_ptr = file_data.ptr;
  *out_size = file_data.size;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 archive_write_disk_bytes(const path* dst, const void* data_ptr, sz data_size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  buffer write_data = {0};
  if (dst == NULL || (data_size > 0 && data_ptr == NULL)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(dst->buf[0] != '\0');

  write_data.ptr = (void*)data_ptr;
  write_data.size = data_size;
  TracyCZoneEnd(__tracy_zone_ctx);
  return file_write_all(dst, write_data);
}

func archive archive_create(allocator* opt_alloc) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  archive arc;
  memset(&arc, 0, size_of(arc));
  arc.opt_alloc = opt_alloc;
  thread_log_trace("archive_create: opt_alloc=%p", (void*)opt_alloc);
  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_ARCHIVE,
                                                     .object_ptr = &arc,
                                                 });
  if (!msg_post(&lifecycle_msg)) {
    memset(&arc, 0, size_of(arc));
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return arc;
}

func void archive_clear(archive* arc) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz item_idx = 0;

  if (arc == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(arc->entry_count <= arc->entry_capacity);

  for (item_idx = 0; item_idx < arc->entry_count; item_idx += 1) {
    archive_reset_entry(arc, &arc->entries[item_idx]);
  }

  arc->entry_count = 0;
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void archive_destroy(archive* arc) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (arc == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  thread_log_trace("archive_destroy: arc=%p entries=%zu", (void*)arc, (size_t)arc->entry_count);

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_ARCHIVE,
                                                     .object_ptr = arc,
                                                 });
  if (!msg_post(&lifecycle_msg)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  archive_clear(arc);
  archive_dealloc_bytes(
      arc->opt_alloc,
      arc->entries,
      arc->entry_capacity * size_of(archive_entry));
  arc->entries = NULL;
  arc->entry_capacity = 0;
  TracyCZoneEnd(__tracy_zone_ctx);
}

func sz archive_count(const archive* arc) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (arc == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return arc->entry_count;
}

func b32 archive_exists(const archive* arc, const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return archive_find_idx(arc, src) != SZ_MAX ? 1 : 0;
}

func b32 archive_write_all(archive* arc, const path* src, buffer data) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz item_idx = 0;
  archive_entry* ent = NULL;

  if (arc == NULL || src == NULL || (data.size > 0 && data.ptr == NULL)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(arc->entry_count <= arc->entry_capacity);

  if (!archive_add_empty_entry(arc, src, 0, &item_idx)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  ent = &arc->entries[item_idx];
  ent->is_directory = 0;
  TracyCZoneEnd(__tracy_zone_ctx);
  return archive_assign_entry_bytes(arc, ent, data);
}

func b32 archive_remove(archive* arc, const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz item_idx = archive_find_idx(arc, src);

  if (arc == NULL || item_idx == SZ_MAX) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  archive_reset_entry(arc, &arc->entries[item_idx]);
  if (item_idx + 1 < arc->entry_count) {
    memmove(
        &arc->entries[item_idx],
        &arc->entries[item_idx + 1],
        (arc->entry_count - item_idx - 1) * size_of(archive_entry));
  }
  arc->entry_count -= 1;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 archive_read_all(const archive* arc, const path* src, allocator* alloc, buffer* out_data) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  const archive_entry* ent = NULL;
  void* data_ptr = NULL;
  sz item_idx = 0;

  if (arc == NULL || alloc == NULL || out_data == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  if (alloc->alloc_fn == NULL || alloc->dealloc_fn == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(arc->entry_count <= arc->entry_capacity);

  out_data->ptr = NULL;
  out_data->size = 0;

  item_idx = archive_find_idx(arc, src);
  if (item_idx == SZ_MAX) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  ent = &arc->entries[item_idx];
  if (ent->is_directory) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (ent->data_size > 0) {
    data_ptr = allocator_alloc(alloc, ent->data_size);
    if (data_ptr == NULL) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return 0;
    }
    memcpy(data_ptr, ent->data_ptr, ent->data_size);
  }

  out_data->ptr = data_ptr;
  out_data->size = ent->data_size;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 archive_add_file(archive* arc, const path* archive_path, const path* disk_path) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator data_alloc = {0};
  void* data_ptr = NULL;
  sz data_size = 0;
  b32 result = 0;
  buffer data = {0};

  if (arc == NULL || archive_path == NULL || disk_path == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  data_alloc = archive_allocator_resolve(arc->opt_alloc);
  if (!archive_read_disk_bytes(disk_path, &data_alloc, &data_ptr, &data_size)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(data_size == 0 || data_ptr != NULL);

  data.ptr = data_ptr;
  data.size = data_size;
  result = archive_write_all(arc, archive_path, data);
  archive_dealloc_bytes(arc->opt_alloc, data_ptr, data_size);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func b32 archive_iterate(const archive* arc, archive_iterate_callback* callback, void* user_data) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  archive_entry_info info;
  sz item_idx = 0;

  if (arc == NULL || callback == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  for (item_idx = 0; item_idx < arc->entry_count; item_idx += 1) {
    info.item_path = arc->entries[item_idx].item_path;
    info.data_size = arc->entries[item_idx].data_size;
    info.is_directory = arc->entries[item_idx].is_directory;
    if (!callback(&info, user_data)) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return 0;
    }
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 archive_load_file(archive* arc, const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
#if BASED_ARCHIVE_HAS_MINIZ
  allocator zip_alloc = {0};
  mz_zip_archive zip_archive;
  void* zip_ptr = NULL;
  sz zip_size = 0;
  mz_uint file_count = 0;
  mz_uint file_idx = 0;

  if (arc == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  if (src == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(arc->entry_count <= arc->entry_capacity);
  thread_log_trace("archive_load_file: arc=%p src=%s", (void*)arc, src->buf);
  zip_alloc = archive_allocator_resolve(arc->opt_alloc);

  archive_clear(arc);

  if (!archive_read_disk_bytes(src, &zip_alloc, &zip_ptr, &zip_size)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(zip_size == 0 || zip_ptr != NULL);

  memset(&zip_archive, 0, size_of(zip_archive));
  if (!mz_zip_reader_init_mem(&zip_archive, zip_ptr, (size_t)zip_size, 0)) {
    archive_dealloc_bytes(arc->opt_alloc, zip_ptr, zip_size);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  file_count = mz_zip_reader_get_num_files(&zip_archive);
  for (file_idx = 0; file_idx < file_count; file_idx += 1) {
    mz_zip_archive_file_stat file_stat;
    path item_path;
    sz item_idx = 0;

    if (!mz_zip_reader_file_stat(&zip_archive, file_idx, &file_stat)) {
      mz_zip_reader_end(&zip_archive);
      archive_dealloc_bytes(arc->opt_alloc, zip_ptr, zip_size);
      archive_clear(arc);
      TracyCZoneEnd(__tracy_zone_ctx);
      return 0;
    }

    item_path = path_from_cstr(file_stat.m_filename);
    path_normalize(&item_path);
    if (mz_zip_reader_is_file_a_directory(&zip_archive, file_idx)) {
      path_remove_trailing_slash(&item_path);
      if (!archive_add_empty_entry(arc, &item_path, 1, &item_idx)) {
        mz_zip_reader_end(&zip_archive);
        archive_dealloc_bytes(arc->opt_alloc, zip_ptr, zip_size);
        archive_clear(arc);
        TracyCZoneEnd(__tracy_zone_ctx);
        return 0;
      }
      continue;
    }

    if (!archive_add_empty_entry(arc, &item_path, 0, &item_idx)) {
      mz_zip_reader_end(&zip_archive);
      archive_dealloc_bytes(arc->opt_alloc, zip_ptr, zip_size);
      archive_clear(arc);
      TracyCZoneEnd(__tracy_zone_ctx);
      return 0;
    }

    if (file_stat.m_uncomp_size > 0) {
      size_t item_size = 0;
      void* item_ptr = mz_zip_reader_extract_to_heap(&zip_archive, file_idx, &item_size, 0);
      buffer data = {0};

      if (item_ptr == NULL) {
        mz_zip_reader_end(&zip_archive);
        archive_dealloc_bytes(arc->opt_alloc, zip_ptr, zip_size);
        archive_clear(arc);
        TracyCZoneEnd(__tracy_zone_ctx);
        return 0;
      }

      data.ptr = item_ptr;
      data.size = (sz)item_size;
      if (!archive_assign_entry_bytes(arc, &arc->entries[item_idx], data)) {
        mz_free(item_ptr);
        mz_zip_reader_end(&zip_archive);
        archive_dealloc_bytes(arc->opt_alloc, zip_ptr, zip_size);
        archive_clear(arc);
        TracyCZoneEnd(__tracy_zone_ctx);
        return 0;
      }

      mz_free(item_ptr);
    }
  }

  mz_zip_reader_end(&zip_archive);
  archive_dealloc_bytes(arc->opt_alloc, zip_ptr, zip_size);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
#else
  (void)arc;
  (void)src;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
#endif
}

func b32 archive_save_file(const archive* arc, const path* dst) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
#if BASED_ARCHIVE_HAS_MINIZ
  mz_zip_archive zip_archive;
  void* zip_ptr = NULL;
  size_t zip_size = 0;
  sz item_idx = 0;

  if (arc == NULL || dst == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(arc->entry_count <= arc->entry_capacity);
  thread_log_trace("archive_save_file: arc=%p dst=%s entries=%zu", (void*)arc, dst->buf, (size_t)arc->entry_count);

  memset(&zip_archive, 0, size_of(zip_archive));
  if (!mz_zip_writer_init_heap(&zip_archive, 0, 0)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  for (item_idx = 0; item_idx < arc->entry_count; item_idx += 1) {
    const archive_entry* ent = &arc->entries[item_idx];
    path item_path = ent->item_path;

    if (ent->is_directory) {
      if (!cstr8_is_empty(item_path.buf) && item_path.buf[cstr8_len(item_path.buf) - 1] != '/') {
        cstr8_append_char(item_path.buf, count_of(item_path.buf), '/');
      }
    }

    if (!mz_zip_writer_add_mem(
            &zip_archive,
            item_path.buf,
            ent->data_ptr,
            (size_t)ent->data_size,
            MZ_DEFAULT_COMPRESSION)) {
      mz_zip_writer_end(&zip_archive);
      TracyCZoneEnd(__tracy_zone_ctx);
      return 0;
    }
  }

  if (!mz_zip_writer_finalize_heap_archive(&zip_archive, &zip_ptr, &zip_size)) {
    mz_zip_writer_end(&zip_archive);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  mz_zip_writer_end(&zip_archive);

  if (!archive_write_disk_bytes(dst, zip_ptr, (sz)zip_size)) {
    mz_free(zip_ptr);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  mz_free(zip_ptr);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
#else
  (void)arc;
  (void)dst;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
#endif
}
