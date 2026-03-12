// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/archive.h"
#include "basic/assert.h"
#include "basic/utility_defines.h"
#include "context/thread_ctx.h"
#include "filesystem/file.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "basic/profiler.h"
#include "memory/heap.h"
#include "memory/memops.h"
#include <string.h>
#include <miniz.h>
#include "basic/safe.h"

func sz archive_find_idx(const archive* arc, const path* src) {
  profile_func_begin;
  sz item_idx = 0;

  if (arc == NULL) {
    profile_func_end;
    return SZ_MAX;
  }

  safe_for (item_idx = 0; item_idx < arc->entry_count; item_idx += 1) {
    if (path_cmd_normd(&arc->entries[item_idx].item_path, src)) {
      profile_func_end;
      return item_idx;
    }
  }

  profile_func_end;
  return SZ_MAX;
}

func b32 archive_reserve(archive* arc, sz min_capacity) {
  profile_func_begin;
  archive_entry* new_entries = NULL;
  sz new_capacity = 0;
  heap* hp = NULL;
  if (arc == NULL) {
    profile_func_end;
    return false;
  }
  assert(arc->entry_count <= arc->entry_capacity);

  if (arc->entry_capacity >= min_capacity) {
    profile_func_end;
    return true;
  }

  new_capacity = arc->entry_capacity == 0 ? 8 : arc->entry_capacity;
  safe_while (new_capacity < min_capacity) {
    if (new_capacity > SZ_MAX / 2) {
      new_capacity = min_capacity;
      break;
    }
    new_capacity *= 2;
  }

  hp = thread_get_perm_heap();
  if (hp == NULL) {
    thread_log_error("Missing archive permanent heap old_size=%zu new_size=%zu",
                     (size_t)(arc->entry_capacity * size_of(archive_entry)),
                     (size_t)(new_capacity * size_of(archive_entry)));
    profile_func_end;
    return false;
  }

  new_entries = (archive_entry*)heap_realloc(hp,
                                             arc->entries,
                                             arc->entry_capacity * size_of(archive_entry),
                                             new_capacity * size_of(archive_entry),
                                             align_of(archive_entry));
  if (new_entries == NULL) {
    thread_log_error("Failed to grow archive entries min_capacity=%zu", (size_t)min_capacity);
    profile_func_end;
    return false;
  }

  arc->entries = new_entries;
  arc->entry_capacity = new_capacity;
  profile_func_end;
  return true;
}

func void archive_reset_entry(archive* arc, archive_entry* ent) {
  profile_func_begin;
  heap* hp = NULL;
  assert(arc != NULL);
  assert(ent != NULL);
  if (ent->data_ptr != NULL) {
    hp = thread_get_perm_heap();
    if (hp == NULL) {
      thread_log_error("Missing archive permanent heap for deallocation ptr=%p", ent->data_ptr);
    } else {
      heap_dealloc(hp, ent->data_ptr);
    }
  }
  ent->data_ptr = NULL;
  ent->data_size = 0;
  ent->data_capacity = 0;
  ent->is_directory = 0;
  ent->item_path = path_from_cstr("");
  profile_func_end;
}

func b32 archive_assign_entry_bytes(archive* arc, archive_entry* ent, buffer data) {
  profile_func_begin;
  void* data_ptr = NULL;
  heap* hp = NULL;
  if (arc == NULL || ent == NULL) {
    profile_func_end;
    return false;
  }
  assert(arc->entry_count <= arc->entry_capacity);

  if (ent->data_ptr != NULL) {
    hp = thread_get_perm_heap();
    if (hp == NULL) {
      thread_log_error("Missing archive permanent heap for deallocation ptr=%p", ent->data_ptr);
    } else {
      heap_dealloc(hp, ent->data_ptr);
    }
  }
  ent->data_ptr = NULL;
  ent->data_size = 0;
  ent->data_capacity = 0;

  if (data.size == 0) {
    profile_func_end;
    return true;
  }
  if (data.ptr == NULL) {
    thread_log_error("Rejected archive entry byte assignment because source data is NULL size=%zu", (size_t)data.size);
    profile_func_end;
    return false;
  }
  assert(arc != NULL);
  assert(ent != NULL);

  hp = thread_get_perm_heap();
  if (hp == NULL) {
    thread_log_error("Missing archive permanent heap size=%zu", (size_t)data.size);
    profile_func_end;
    return false;
  }

  data_ptr = heap_alloc(hp, data.size, align_of(u8));
  if (data_ptr == NULL) {
    thread_log_error("Failed to allocate archive entry bytes size=%zu", (size_t)data.size);
    profile_func_end;
    return false;
  }

  mem_cpy(data_ptr, data.ptr, data.size);
  ent->data_ptr = (u8*)data_ptr;
  ent->data_size = data.size;
  ent->data_capacity = data.size;
  profile_func_end;
  return true;
}

func b32 archive_add_empty_entry(archive* arc, const path* src, b32 is_directory, sz* out_idx) {
  profile_func_begin;
  archive_entry* ent = NULL;
  sz item_idx = SZ_MAX;
  heap* hp = NULL;

  if (arc == NULL || src == NULL) {
    profile_func_end;
    return false;
  }
  assert(is_directory == 0 || is_directory == 1);
  item_idx = archive_find_idx(arc, src);

  if (item_idx != SZ_MAX) {
    ent = &arc->entries[item_idx];
    ent->item_path = path_norm_trimmed_cpy(src);
    ent->is_directory = is_directory;
    if (is_directory && ent->data_ptr != NULL) {
      hp = thread_get_perm_heap();
      if (hp == NULL) {
        thread_log_error("Missing archive permanent heap for deallocation ptr=%p", ent->data_ptr);
      } else {
        heap_dealloc(hp, ent->data_ptr);
      }
      ent->data_ptr = NULL;
      ent->data_size = 0;
      ent->data_capacity = 0;
    }
    if (out_idx != NULL) {
      *out_idx = item_idx;
    }
    profile_func_end;
    return true;
  }

  if (!archive_reserve(arc, arc->entry_count + 1)) {
    thread_log_error("Failed to reserve archive entry for path=%s", src->buf);
    profile_func_end;
    return false;
  }

  ent = &arc->entries[arc->entry_count];
  mem_zero(ent, size_of(*ent));
  ent->item_path = path_norm_trimmed_cpy(src);
  ent->is_directory = is_directory;
  if (out_idx != NULL) {
    *out_idx = arc->entry_count;
  }
  arc->entry_count += 1;
  profile_func_end;
  return true;
}

func b32 archive_read_disk_bytes(const path* src, allocator* alloc, void** out_ptr, sz* out_size) {
  profile_func_begin;
  buffer file_data = {0};

  if (src == NULL || alloc == NULL || out_ptr == NULL || out_size == NULL) {
    profile_func_end;
    return false;
  }
  if (alloc->alloc_fn == NULL || alloc->dealloc_fn == NULL) {
    thread_log_error("Rejected archive disk read because allocator is incomplete path=%s", src->buf);
    profile_func_end;
    return false;
  }
  assert(src->buf[0] != '\0');

  *out_ptr = NULL;
  *out_size = 0;
  if (!file_read_all(src, alloc, &file_data)) {
    thread_log_error("Failed to read archive source file path=%s", src->buf);
    profile_func_end;
    return false;
  }
  *out_ptr = file_data.ptr;
  *out_size = file_data.size;
  profile_func_end;
  return true;
}

func b32 archive_write_disk_bytes(const path* dst, const void* data_ptr, sz data_size) {
  profile_func_begin;
  buffer write_data = {0};
  if (dst == NULL || (data_size > 0 && data_ptr == NULL)) {
    thread_log_error("Rejected archive disk write path=%s size=%zu", dst != NULL ? dst->buf : "<null>", (size_t)data_size);
    profile_func_end;
    return false;
  }
  assert(dst->buf[0] != '\0');

  write_data.ptr = (void*)data_ptr;
  write_data.size = data_size;
  profile_func_end;
  return file_write_all(dst, write_data);
}

func archive _archive_create(callsite site) {
  profile_func_begin;
  archive arc;
  mem_zero(&arc, size_of(arc));
  thread_log_trace("Created archive");
  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
      .object_type = MSG_CORE_OBJECT_TYPE_ARCHIVE,
      .object_ptr = &arc,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    mem_zero(&arc, size_of(arc));
    thread_log_trace("Archive creation was suspended");
  }
  profile_func_end;
  return arc;
}

func void archive_clear(archive* arc) {
  profile_func_begin;
  sz item_idx = 0;

  if (arc == NULL) {
    profile_func_end;
    return;
  }
  assert(arc->entry_count <= arc->entry_capacity);

  safe_for (item_idx = 0; item_idx < arc->entry_count; item_idx += 1) {
    archive_reset_entry(arc, &arc->entries[item_idx]);
  }

  arc->entry_count = 0;
  profile_func_end;
}

func void _archive_destroy(archive* arc, callsite site) {
  profile_func_begin;
  heap* hp = NULL;
  if (arc == NULL) {
    profile_func_end;
    return;
  }
  thread_log_trace("Destroying archive arc=%p entries=%zu", (void*)arc, (size_t)arc->entry_count);

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
      .object_type = MSG_CORE_OBJECT_TYPE_ARCHIVE,
      .object_ptr = arc,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_warn("Archive destruction was suspended arc=%p", (void*)arc);
    profile_func_end;
    return;
  }

  archive_clear(arc);
  if (arc->entries != NULL) {
    hp = thread_get_perm_heap();
    if (hp == NULL) {
      thread_log_error("Missing archive permanent heap for deallocation ptr=%p", (void*)arc->entries);
    } else {
      heap_dealloc(hp, arc->entries);
    }
  }
  arc->entries = NULL;
  arc->entry_capacity = 0;
  profile_func_end;
}

func sz archive_count(const archive* arc) {
  if (arc == NULL) {
    return 0;
  }

  return arc->entry_count;
}

func b32 archive_exists(const archive* arc, const path* src) {
  return archive_find_idx(arc, src) != SZ_MAX ? true : false;
}

func b32 archive_write_all(archive* arc, const path* src, buffer data) {
  profile_func_begin;
  sz item_idx = 0;
  archive_entry* ent = NULL;

  if (arc == NULL || src == NULL || (data.size > 0 && data.ptr == NULL)) {
    profile_func_end;
    return false;
  }
  assert(arc->entry_count <= arc->entry_capacity);

  if (!archive_add_empty_entry(arc, src, 0, &item_idx)) {
    thread_log_error("Failed to create archive entry path=%s", src->buf);
    profile_func_end;
    return false;
  }

  ent = &arc->entries[item_idx];
  ent->is_directory = 0;
  profile_func_end;
  return archive_assign_entry_bytes(arc, ent, data);
}

func b32 archive_remove(archive* arc, const path* src) {
  profile_func_begin;
  sz item_idx = archive_find_idx(arc, src);

  if (arc == NULL || item_idx == SZ_MAX) {
    profile_func_end;
    return false;
  }

  archive_reset_entry(arc, &arc->entries[item_idx]);
  if (item_idx + 1 < arc->entry_count) {
    mem_mv(
        &arc->entries[item_idx],
        &arc->entries[item_idx + 1],
        (arc->entry_count - item_idx - 1) * size_of(archive_entry));
  }
  arc->entry_count -= 1;
  profile_func_end;
  return true;
}

func b32 archive_read_all(const archive* arc, const path* src, allocator* alloc, buffer* out_data) {
  profile_func_begin;
  const archive_entry* ent = NULL;
  void* data_ptr = NULL;
  sz item_idx = 0;

  if (arc == NULL || alloc == NULL || out_data == NULL) {
    thread_log_error("Rejected archive read all arc=%p src=%p alloc=%p out_data=%p",
                     (void*)arc,
                     (void*)src,
                     (void*)alloc,
                     (void*)out_data);
    profile_func_end;
    return false;
  }
  if (alloc->alloc_fn == NULL || alloc->dealloc_fn == NULL) {
    profile_func_end;
    return false;
  }
  assert(arc->entry_count <= arc->entry_capacity);

  out_data->ptr = NULL;
  out_data->size = 0;

  item_idx = archive_find_idx(arc, src);
  if (item_idx == SZ_MAX) {
    thread_log_warn("Archive entry not found path=%s", src != NULL ? src->buf : "<null>");
    profile_func_end;
    return false;
  }

  ent = &arc->entries[item_idx];
  if (ent->is_directory) {
    thread_log_warn("Cannot read archive directory entry path=%s", src->buf);
    profile_func_end;
    return false;
  }

  if (ent->data_size > 0) {
    data_ptr = allocator_alloc(*alloc, ent->data_size);
    if (data_ptr == NULL) {
      thread_log_error("Failed to allocate archive read buffer path=%s size=%zu",
                       src->buf,
                       (size_t)ent->data_size);
      profile_func_end;
      return false;
    }
    mem_cpy(data_ptr, ent->data_ptr, ent->data_size);
  }

  out_data->ptr = data_ptr;
  out_data->size = ent->data_size;
  profile_func_end;
  return true;
}

func b32 archive_add_file(archive* arc, const path* archive_path, const path* disk_path) {
  profile_func_begin;
  heap* data_heap = NULL;
  allocator data_alloc = {0};
  void* data_ptr = NULL;
  sz data_size = 0;
  b32 result = false;
  buffer data = {0};

  if (arc == NULL || archive_path == NULL || disk_path == NULL) {
    profile_func_end;
    return false;
  }

  data_heap = thread_get_temp_heap();
  if (data_heap == NULL) {
    thread_log_error("Missing archive temporary heap while adding file path=%s", disk_path->buf);
    profile_func_end;
    return false;
  }

  data_alloc = heap_get_allocator(data_heap);
  if (!archive_read_disk_bytes(disk_path, &data_alloc, &data_ptr, &data_size)) {
    profile_func_end;
    return false;
  }
  assert(data_size == 0 || data_ptr != NULL);

  data.ptr = data_ptr;
  data.size = data_size;
  result = archive_write_all(arc, archive_path, data);
  heap_dealloc(data_heap, data_ptr);
  profile_func_end;
  return result;
}

func b32 archive_iterate(
    const archive* arc,
    archive_iterate_callback* callback,
    void* user_data) {
  profile_func_begin;
  archive_entry_info info;
  sz item_idx = 0;

  if (arc == NULL || callback == NULL) {
    profile_func_end;
    return false;
  }

  safe_for (item_idx = 0; item_idx < arc->entry_count; item_idx += 1) {
    info.item_path = arc->entries[item_idx].item_path;
    info.data_size = arc->entries[item_idx].data_size;
    info.is_directory = arc->entries[item_idx].is_directory;
    if (!callback(&info, user_data)) {
      profile_func_end;
      return false;
    }
  }

  profile_func_end;
  return true;
}

func b32 archive_get_entry_info(
    const archive* arc,
    const path* src,
    archive_entry_info* out_info) {
  profile_func_begin;
  if (arc == NULL || src == NULL || out_info == NULL) {
    profile_func_end;
    return false;
  }
  sz item_idx = archive_find_idx(arc, src);
  if (item_idx == SZ_MAX) {
    profile_func_end;
    return false;
  }
  archive_entry const* ent = &arc->entries[item_idx];
  out_info->item_path = ent->item_path;
  out_info->data_size = ent->data_size;
  out_info->is_directory = ent->is_directory;
  profile_func_end;
  return true;
}

func b32 archive_get_entry_data(
    const archive* arc,
    const path* src,
    buffer* out_data) {
  profile_func_begin;
  if (arc == NULL || src == NULL || out_data == NULL) {
    profile_func_end;
    return false;
  }
  out_data->ptr = NULL;
  out_data->size = 0;
  sz item_idx = archive_find_idx(arc, src);
  if (item_idx == SZ_MAX) {
    profile_func_end;
    return false;
  }
  archive_entry const* ent = &arc->entries[item_idx];
  if (ent->is_directory) {
    profile_func_end;
    return false;
  }
  out_data->ptr = ent->data_ptr;
  out_data->size = ent->data_size;
  profile_func_end;
  return true;
}

func b32 archive_load_file(archive* arc, const path* src) {
  profile_func_begin;
  heap* zip_heap = NULL;
  allocator zip_alloc = {0};
  mz_zip_archive zip_archive;
  void* zip_ptr = NULL;
  sz zip_size = 0;
  mz_uint file_count = 0;
  mz_uint file_idx = 0;

  if (arc == NULL) {
    profile_func_end;
    return false;
  }
  if (src == NULL) {
    profile_func_end;
    return false;
  }
  assert(arc->entry_count <= arc->entry_capacity);
  thread_log_trace("Loading archive file arc=%p src=%s", (void*)arc, src->buf);
  zip_heap = thread_get_temp_heap();
  if (zip_heap == NULL) {
    thread_log_error("Missing archive temporary heap while loading path=%s", src->buf);
    profile_func_end;
    return false;
  }

  zip_alloc = heap_get_allocator(zip_heap);

  archive_clear(arc);

  if (!archive_read_disk_bytes(src, &zip_alloc, &zip_ptr, &zip_size)) {
    thread_log_error("Failed to read archive file from disk path=%s", src->buf);
    profile_func_end;
    return false;
  }
  assert(zip_size == 0 || zip_ptr != NULL);

  mem_zero(&zip_archive, size_of(zip_archive));
  if (!mz_zip_reader_init_mem(&zip_archive, zip_ptr, (size_t)zip_size, 0)) {
    thread_log_error("Failed to initialize zip reader path=%s", src->buf);
    heap_dealloc(zip_heap, zip_ptr);
    profile_func_end;
    return false;
  }

  file_count = mz_zip_reader_get_num_files(&zip_archive);
  safe_for (file_idx = 0; file_idx < file_count; file_idx += 1) {
    mz_zip_archive_file_stat file_stat;
    path item_path;
    sz item_idx = 0;

    if (!mz_zip_reader_file_stat(&zip_archive, file_idx, &file_stat)) {
      thread_log_error("Failed to query zip entry stat path=%s index=%u", src->buf, (u32)file_idx);
      mz_zip_reader_end(&zip_archive);
      heap_dealloc(zip_heap, zip_ptr);
      archive_clear(arc);
      profile_func_end;
      return false;
    }

    item_path = path_from_cstr(file_stat.m_filename);
    path_norm(&item_path);
    if (mz_zip_reader_is_file_a_directory(&zip_archive, file_idx)) {
      path_remove_trailing_slash(&item_path);
      if (!archive_add_empty_entry(arc, &item_path, 1, &item_idx)) {
        thread_log_error("Failed to add archive directory entry path=%s", item_path.buf);
        mz_zip_reader_end(&zip_archive);
        heap_dealloc(zip_heap, zip_ptr);
        archive_clear(arc);
        profile_func_end;
        return false;
      }
      continue;
    }

    if (!archive_add_empty_entry(arc, &item_path, 0, &item_idx)) {
      thread_log_error("Failed to add archive file entry path=%s", item_path.buf);
      mz_zip_reader_end(&zip_archive);
      heap_dealloc(zip_heap, zip_ptr);
      archive_clear(arc);
      profile_func_end;
      return false;
    }

    if (file_stat.m_uncomp_size > 0) {
      size_t item_size = 0;
      void* item_ptr = mz_zip_reader_extract_to_heap(&zip_archive, file_idx, &item_size, 0);
      buffer data = {0};

      if (item_ptr == NULL) {
        thread_log_error("Failed to extract zip entry path=%s", item_path.buf);
        mz_zip_reader_end(&zip_archive);
        heap_dealloc(zip_heap, zip_ptr);
        archive_clear(arc);
        profile_func_end;
        return false;
      }

      data.ptr = item_ptr;
      data.size = (sz)item_size;
      if (!archive_assign_entry_bytes(arc, &arc->entries[item_idx], data)) {
        thread_log_error("Failed to store extracted zip entry path=%s", item_path.buf);
        mz_free(item_ptr);
        mz_zip_reader_end(&zip_archive);
        heap_dealloc(zip_heap, zip_ptr);
        archive_clear(arc);
        profile_func_end;
        return false;
      }

      mz_free(item_ptr);
    }
  }

  mz_zip_reader_end(&zip_archive);
  heap_dealloc(zip_heap, zip_ptr);
  profile_func_end;
  return true;
}

func b32 archive_save_file(const archive* arc, const path* dst) {
  profile_func_begin;
  mz_zip_archive zip_archive;
  void* zip_ptr = NULL;
  size_t zip_size = 0;
  sz item_idx = 0;

  if (arc == NULL || dst == NULL) {
    profile_func_end;
    return false;
  }
  assert(arc->entry_count <= arc->entry_capacity);
  thread_log_trace("Saving archive file arc=%p dst=%s entries=%zu", (void*)arc, dst->buf, (size_t)arc->entry_count);

  mem_zero(&zip_archive, size_of(zip_archive));
  if (!mz_zip_writer_init_heap(&zip_archive, 0, 0)) {
    thread_log_error("Failed to initialize zip writer dst=%s", dst->buf);
    profile_func_end;
    return false;
  }

  safe_for (item_idx = 0; item_idx < arc->entry_count; item_idx += 1) {
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
      thread_log_error("Failed to add archive entry to zip path=%s", item_path.buf);
      mz_zip_writer_end(&zip_archive);
      profile_func_end;
      return false;
    }
  }

  if (!mz_zip_writer_finalize_heap_archive(&zip_archive, &zip_ptr, &zip_size)) {
    thread_log_error("Failed to finalize archive file dst=%s", dst->buf);
    mz_zip_writer_end(&zip_archive);
    profile_func_end;
    return false;
  }

  mz_zip_writer_end(&zip_archive);

  if (!archive_write_disk_bytes(dst, zip_ptr, (sz)zip_size)) {
    thread_log_error("Failed to write archive file dst=%s", dst->buf);
    mz_free(zip_ptr);
    profile_func_end;
    return false;
  }

  mz_free(zip_ptr);
  profile_func_end;
  return true;
}
