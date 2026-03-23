// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/vmem.h"
#include "memory/alloc_tracker.h"
#include "basic/assert.h"
#include "basic/env_defines.h"
#include "basic/utility_defines.h"
#include "basic/profiler.h"
#include "memory/memops.h"
#include "platform_includes.h"
#include <string.h>

global_var alloc_tracker g_vmem_reserved_tracker;
global_var alloc_tracker g_vmem_committed_tracker;
global_var alloc_tracker g_vmem_full_tracker;

// =========================================================================
// Platform Implementations
// =========================================================================

#if defined(PLATFORM_WINDOWS)
func sz vmem_page_size(void) {
  profile_func_begin;
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  profile_func_end;
  return (sz)info.dwPageSize;
}

func void* vmem_reserve(sz size) {
  profile_func_begin;
  alloc_tracker_on_alloc_call(&g_vmem_reserved_tracker);
  void* ptr = VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
  if (ptr != NULL) {
    alloc_tracker_on_alloc_success(&g_vmem_reserved_tracker, (callsite) {0}, ptr, size);
    TracyCAlloc(ptr, size);
  }
  profile_func_end;
  return ptr;
}

func b32 vmem_commit(void* ptr, sz size) {
  profile_func_begin;
  alloc_tracker_on_alloc_call(&g_vmem_committed_tracker);
  b32 success = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != NULL ? true : false;
  if (success) {
    alloc_tracker_on_alloc_success(&g_vmem_committed_tracker, (callsite) {0}, ptr, size);
  }
  profile_func_end;
  return success;
}

func b32 vmem_decommit(void* ptr, sz size) {
  profile_func_begin;
  alloc_tracker_on_free_call(&g_vmem_committed_tracker);
  b32 success = VirtualFree(ptr, size, MEM_DECOMMIT) != 0 ? true : false;
  if (success) {
    alloc_tracker_on_free_success(&g_vmem_committed_tracker, (callsite) {0}, ptr, size);
  }
  profile_func_end;
  return success;
}

func b32 vmem_release(void* ptr, sz size) {
  profile_func_begin;
  alloc_tracker_on_free_call(&g_vmem_reserved_tracker);
  b32 success = VirtualFree(ptr, 0, MEM_RELEASE) != 0 ? true : false;
  if (success) {
    alloc_tracker_on_free_success(&g_vmem_reserved_tracker, (callsite) {0}, ptr, size);
    TracyCFree(ptr);
  }
  profile_func_end;
  return success;
}

func void* vmem_platform_alloc(sz size) {
  profile_func_begin;
  void* ptr = VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
  if (ptr != NULL) {
    TracyCAlloc(ptr, size);
  }
  profile_func_end;
  return ptr;
}

func b32 vmem_platform_free(void* ptr, sz size) {
  profile_func_begin;
  (void)size;
  b32 success = VirtualFree(ptr, 0, MEM_RELEASE) != 0 ? true : false;
  if (success) {
    TracyCFree(ptr);
  }
  profile_func_end;
  return success;
}

#elif defined(PLATFORM_UNIX)
func sz vmem_page_size(void) {
  profile_func_begin;
  sz res = (sz)sysconf(_SC_PAGESIZE);
  profile_func_end;
  return res;
}

func void* vmem_reserve(sz size) {
  profile_func_begin;
  alloc_tracker_on_alloc_call(&g_vmem_reserved_tracker);
  void* ptr = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (ptr == MAP_FAILED) {
    profile_func_end;
    return NULL;
  }
  alloc_tracker_on_alloc_success(&g_vmem_reserved_tracker, (callsite) {0}, ptr, size);
  TracyCAlloc(ptr, size);
  profile_func_end;
  return ptr;
}

func b32 vmem_commit(void* ptr, sz size) {
  profile_func_begin;
  alloc_tracker_on_alloc_call(&g_vmem_committed_tracker);
  b32 success = mprotect(ptr, size, PROT_READ | PROT_WRITE) == 0 ? true : false;
  if (success) {
    alloc_tracker_on_alloc_success(&g_vmem_committed_tracker, (callsite) {0}, ptr, size);
  }
  profile_func_end;
  return success;
}

func b32 vmem_decommit(void* ptr, sz size) {
  profile_func_begin;
  alloc_tracker_on_free_call(&g_vmem_committed_tracker);
  if (mprotect(ptr, size, PROT_NONE) != 0) {
    profile_func_end;
    return false;
  }
  // Hint to the OS to release the physical pages; best-effort, not checked.
  (void)madvise(ptr, size, MADV_DONTNEED);
  alloc_tracker_on_free_success(&g_vmem_committed_tracker, (callsite) {0}, ptr, size);
  profile_func_end;
  return true;
}

func b32 vmem_release(void* ptr, sz size) {
  profile_func_begin;
  alloc_tracker_on_free_call(&g_vmem_reserved_tracker);
  b32 success = munmap(ptr, size) == 0 ? true : false;
  if (success) {
    alloc_tracker_on_free_success(&g_vmem_reserved_tracker, (callsite) {0}, ptr, size);
    TracyCFree(ptr);
  }
  profile_func_end;
  return success;
}

func void* vmem_platform_alloc(sz size) {
  profile_func_begin;
  void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (ptr == MAP_FAILED) {
    profile_func_end;
    return NULL;
  }
  TracyCAlloc(ptr, size);
  profile_func_end;
  return ptr;
}

func b32 vmem_platform_free(void* ptr, sz size) {
  profile_func_begin;
  b32 success = munmap(ptr, size) == 0 ? true : false;
  if (success) {
    TracyCFree(ptr);
  }
  profile_func_end;
  return success;
}

#else

// =========================================================================
// Fallback — platforms without virtual memory primitives
// =========================================================================

func sz vmem_page_size(void) {
  invalid_code_path;
  return 0;
}

func void* vmem_reserve(sz size) {
  profile_func_begin;
  (void)size;
  invalid_code_path;
  profile_func_end;
  return NULL;
}

func b32 vmem_commit(void* ptr, sz size) {
  profile_func_begin;
  (void)ptr;
  (void)size;
  invalid_code_path;
  profile_func_end;
  return false;
}

func b32 vmem_decommit(void* ptr, sz size) {
  profile_func_begin;
  (void)ptr;
  (void)size;
  invalid_code_path;
  profile_func_end;
  return false;
}

func b32 vmem_release(void* ptr, sz size) {
  profile_func_begin;
  (void)ptr;
  (void)size;
  invalid_code_path;
  profile_func_end;
  return false;
}

func void* vmem_platform_alloc(sz size) {
  profile_func_begin;
  (void)size;
  invalid_code_path;
  profile_func_end;
  return NULL;
}

func b32 vmem_platform_free(void* ptr, sz size) {
  profile_func_begin;
  (void)ptr;
  (void)size;
  invalid_code_path;
  profile_func_end;
  return false;
}

#endif

// =========================================================================
// Allocation Wrappers
// =========================================================================

typedef union vmem_alloc_header {
  struct {
    sz mapping_size;
    sz prefix_size;
    sz user_size;
  } info;
  max_align_t force_align;
} vmem_alloc_header;

func vmem_alloc_header* vmem_get_alloc_header(void* ptr) {
  return ((vmem_alloc_header*)ptr) - 1;
}

func void* vmem_get_alloc_base(void* ptr) {
  profile_func_begin;
  vmem_alloc_header* header = vmem_get_alloc_header(ptr);
  profile_func_end;
  return (u8*)ptr - header->info.prefix_size;
}

func sz vmem_get_alloc_align(void) {
  profile_func_begin;
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_UNIX)
  profile_func_end;
  return vmem_page_size();
#else
  invalid_code_path;
  profile_func_end;
  return 0;
#endif
}

func void* vmem_alloc(sz size) {
  profile_func_begin;
  alloc_tracker_on_alloc_call(&g_vmem_full_tracker);
  if (size == 0) {
    profile_func_end;
    return NULL;
  }
  assert(size > 0);

  sz alloc_align = vmem_get_alloc_align();
  sz prefix_size = align_up(size_of(vmem_alloc_header), alloc_align);

  if (size > SZ_MAX - prefix_size) {
    profile_func_end;
    return NULL;
  }

  sz total_size = prefix_size + size;
  void* base_ptr = vmem_platform_alloc(total_size);
  if (!base_ptr) {
    profile_func_end;
    return NULL;
  }

  u8* user_ptr = (u8*)base_ptr + prefix_size;
  vmem_alloc_header* header = ((vmem_alloc_header*)user_ptr) - 1;
  header->info.mapping_size = total_size;
  header->info.prefix_size = prefix_size;
  header->info.user_size = size;
  alloc_tracker_on_alloc_success(&g_vmem_full_tracker, (callsite) {0}, user_ptr, size);
  profile_func_end;
  return user_ptr;
}

func void* vmem_calloc(sz count, sz size) {
  profile_func_begin;
  alloc_tracker_on_calloc_call(&g_vmem_full_tracker);
  if (count == 0 || size == 0) {
    profile_func_end;
    return NULL;
  }

  if (count > SZ_MAX / size) {
    profile_func_end;
    return NULL;
  }

  sz total_size = count * size;
  void* ptr = vmem_alloc(total_size);
  if (ptr) {
    mem_zero(ptr, total_size);
  }
  profile_func_end;
  return ptr;
}

func void* vmem_realloc(void* ptr, sz old_size, sz new_size) {
  profile_func_begin;
  alloc_tracker_on_realloc_call(&g_vmem_full_tracker);
  (void)old_size;
  if (!ptr) {
    profile_func_end;
    return vmem_alloc(new_size);
  }

  if (new_size == 0) {
    (void)vmem_free(ptr, 0);
    profile_func_end;
    return NULL;
  }
  assert(new_size > 0);

  vmem_alloc_header* old_header = vmem_get_alloc_header(ptr);
  if (new_size == old_header->info.user_size) {
    profile_func_end;
    return ptr;
  }

  void* new_ptr = vmem_alloc(new_size);
  if (!new_ptr) {
    profile_func_end;
    return NULL;
  }

  sz cpy_size = old_header->info.user_size < new_size ? old_header->info.user_size : new_size;
  mem_cpy(new_ptr, ptr, cpy_size);
  (void)vmem_free(ptr, 0);
  profile_func_end;
  return new_ptr;
}

func b32 vmem_free(void* ptr, sz size) {
  profile_func_begin;
  alloc_tracker_on_free_call(&g_vmem_full_tracker);
  (void)size;
  if (!ptr) {
    profile_func_end;
    return true;
  }

  vmem_alloc_header* header = vmem_get_alloc_header(ptr);
  sz mapping_size = header->info.mapping_size;
  sz user_size = header->info.user_size;
  void* base_ptr = vmem_get_alloc_base(ptr);
  b32 success = vmem_platform_free(base_ptr, mapping_size);
  if (success) {
    alloc_tracker_on_free_success(&g_vmem_full_tracker, (callsite) {0}, ptr, user_size);
  }
  profile_func_end;
  return success;
}

// =========================================================================
// Allocator Callbacks
// =========================================================================

func void* vmem_alloc_callback(void* user_data, callsite site, sz size) {
  profile_func_begin;
  (void)user_data;
  (void)site;
  profile_func_end;
  return vmem_alloc(size);
}

func void vmem_dealloc_callback(void* user_data, callsite site, void* ptr) {
  profile_func_begin;
  (void)user_data;
  (void)site;
  (void)vmem_free(ptr, 0);
  profile_func_end;
}

func void* vmem_realloc_callback(
    void* user_data,
    callsite site,
    void* ptr,
    sz new_size) {
  profile_func_begin;
  (void)user_data;
  (void)site;
  profile_func_end;
  return vmem_realloc(ptr, 0, new_size);
}

func allocator vmem_get_allocator(void) {
  profile_func_begin;
  allocator alloc;
  alloc.user_data = NULL;
  alloc.tracker = NULL;
  alloc.alloc_fn = vmem_alloc_callback;
  alloc.dealloc_fn = vmem_dealloc_callback;
  alloc.realloc_fn = vmem_realloc_callback;
  profile_func_end;
  return alloc;
}

func vmem_stats vmem_get_stats(void) {
  profile_func_begin;
  vmem_stats stats;
  alloc_tracker_stats reserved_stats = alloc_tracker_get_stats(&g_vmem_reserved_tracker);
  alloc_tracker_stats committed_stats = alloc_tracker_get_stats(&g_vmem_committed_tracker);
  alloc_tracker_stats full_stats = alloc_tracker_get_stats(&g_vmem_full_tracker);
  stats.page_size = vmem_page_size();

  stats.reserve_calls = reserved_stats.alloc_calls;
  stats.commit_calls = committed_stats.alloc_calls;
  stats.decommit_calls = committed_stats.free_calls;
  stats.release_calls = reserved_stats.free_calls;

  stats.reserved_bytes = (sz)reserved_stats.total_allocated_bytes;
  stats.committed_bytes = (sz)committed_stats.total_allocated_bytes;
  stats.decommitted_bytes = (sz)committed_stats.total_freed_bytes;
  stats.released_bytes = (sz)reserved_stats.total_freed_bytes;

  stats.alloc_calls = full_stats.alloc_calls;
  stats.calloc_calls = full_stats.calloc_calls;
  stats.realloc_calls = full_stats.realloc_calls;
  stats.free_calls = full_stats.free_calls;

  stats.live_allocations = full_stats.live_allocations;
  stats.live_allocated_bytes = (sz)full_stats.live_allocated_bytes;
  stats.peak_live_allocated_bytes = (sz)full_stats.peak_live_allocated_bytes;
  stats.total_allocated_bytes = (sz)full_stats.total_allocated_bytes;
  stats.total_freed_bytes = (sz)full_stats.total_freed_bytes;
  profile_func_end;
  return stats;
}
