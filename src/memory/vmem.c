// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/vmem.h"
#include "basic/assert.h"
#include "basic/env_defines.h"
#include "basic/utility_defines.h"
#include "basic/profiler.h"
#include <string.h>

typedef struct vmem_stats_state {
  u64 reserve_calls;
  u64 commit_calls;
  u64 decommit_calls;
  u64 release_calls;

  sz reserved_bytes;
  sz committed_bytes;
  sz decommitted_bytes;
  sz released_bytes;

  u64 alloc_calls;
  u64 calloc_calls;
  u64 realloc_calls;
  u64 free_calls;

  u64 live_allocations;
  sz live_allocated_bytes;
  sz peak_live_allocated_bytes;
  sz total_allocated_bytes;
  sz total_freed_bytes;
} vmem_stats_state;

global_var vmem_stats_state g_vmem_stats;

func void vmem_update_peak_allocated_bytes(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (g_vmem_stats.live_allocated_bytes > g_vmem_stats.peak_live_allocated_bytes) {
    g_vmem_stats.peak_live_allocated_bytes = g_vmem_stats.live_allocated_bytes;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}

// =========================================================================
// Platform Implementations
// =========================================================================

#if defined(PLATFORM_WINDOWS)

#  include <windows.h>

func sz vmem_page_size(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (sz)info.dwPageSize;
}

func void* vmem_reserve(sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  g_vmem_stats.reserve_calls += 1;
  void* ptr = VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
  if (ptr != NULL) {
    g_vmem_stats.reserved_bytes += size;
    TracyCAlloc(ptr, size);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return ptr;
}

func b32 vmem_commit(void* ptr, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  g_vmem_stats.commit_calls += 1;
  b32 success = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != NULL ? 1 : 0;
  if (success) {
    g_vmem_stats.committed_bytes += size;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return success;
}

func b32 vmem_decommit(void* ptr, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  g_vmem_stats.decommit_calls += 1;
  b32 success = VirtualFree(ptr, size, MEM_DECOMMIT) != 0 ? 1 : 0;
  if (success) {
    g_vmem_stats.decommitted_bytes += size;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return success;
}

func b32 vmem_release(void* ptr, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  g_vmem_stats.release_calls += 1;
  b32 success = VirtualFree(ptr, 0, MEM_RELEASE) != 0 ? 1 : 0;
  if (success) {
    g_vmem_stats.released_bytes += size;
    TracyCFree(ptr);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return success;
}

func void* vmem_platform_alloc(sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  void* ptr = VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
  if (ptr != NULL) {
    TracyCAlloc(ptr, size);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return ptr;
}

func b32 vmem_platform_free(void* ptr, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  (void)size;
  b32 success = VirtualFree(ptr, 0, MEM_RELEASE) != 0 ? 1 : 0;
  if (success) {
    TracyCFree(ptr);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return success;
}

#elif defined(PLATFORM_UNIX)

#  include <sys/mman.h>
#  include <unistd.h>

func sz vmem_page_size(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (sz)sysconf(_SC_PAGESIZE);
}

func void* vmem_reserve(sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  g_vmem_stats.reserve_calls += 1;
  void* ptr = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (ptr == MAP_FAILED) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  g_vmem_stats.reserved_bytes += size;
  TracyCAlloc(ptr, size);
  TracyCZoneEnd(__tracy_zone_ctx);
  return ptr;
}

func b32 vmem_commit(void* ptr, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  g_vmem_stats.commit_calls += 1;
  b32 success = mprotect(ptr, size, PROT_READ | PROT_WRITE) == 0 ? 1 : 0;
  if (success) {
    g_vmem_stats.committed_bytes += size;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return success;
}

func b32 vmem_decommit(void* ptr, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  g_vmem_stats.decommit_calls += 1;
  if (mprotect(ptr, size, PROT_NONE) != 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  // Hint to the OS to release the physical pages; best-effort, not checked.
  (void)madvise(ptr, size, MADV_DONTNEED);
  g_vmem_stats.decommitted_bytes += size;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 vmem_release(void* ptr, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  g_vmem_stats.release_calls += 1;
  b32 success = munmap(ptr, size) == 0 ? 1 : 0;
  if (success) {
    g_vmem_stats.released_bytes += size;
    TracyCFree(ptr);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return success;
}

func void* vmem_platform_alloc(sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (ptr == MAP_FAILED) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  TracyCAlloc(ptr, size);
  TracyCZoneEnd(__tracy_zone_ctx);
  return ptr;
}

func b32 vmem_platform_free(void* ptr, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  b32 success = munmap(ptr, size) == 0 ? 1 : 0;
  if (success) {
    TracyCFree(ptr);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return success;
}

#else

// =========================================================================
// Fallback — platforms without virtual memory primitives
// =========================================================================
// reserve + commit collapse into a single malloc; decommit is a no-op.

#  include <stdlib.h>

func sz vmem_page_size(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 4096;
}

func void* vmem_reserve(sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  g_vmem_stats.reserve_calls += 1;
  void* ptr = malloc(size);
  if (ptr != NULL) {
    g_vmem_stats.reserved_bytes += size;
    TracyCAlloc(ptr, size);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return ptr;
}

func b32 vmem_commit(void* ptr, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  g_vmem_stats.commit_calls += 1;
  (void)ptr;
  g_vmem_stats.committed_bytes += size;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 vmem_decommit(void* ptr, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  g_vmem_stats.decommit_calls += 1;
  (void)ptr;
  g_vmem_stats.decommitted_bytes += size;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 vmem_release(void* ptr, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  g_vmem_stats.release_calls += 1;
  g_vmem_stats.released_bytes += size;
  if (ptr != NULL) {
    TracyCFree(ptr);
  }
  free(ptr);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func void* vmem_platform_alloc(sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  void* ptr = malloc(size);
  if (ptr != NULL) {
    TracyCAlloc(ptr, size);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return ptr;
}

func b32 vmem_platform_free(void* ptr, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  (void)size;
  if (ptr != NULL) {
    TracyCFree(ptr);
  }
  free(ptr);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
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
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return ((vmem_alloc_header*)ptr) - 1;
}

func void* vmem_get_alloc_base(void* ptr) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  vmem_alloc_header* header = vmem_get_alloc_header(ptr);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (u8*)ptr - header->info.prefix_size;
}

func sz vmem_get_alloc_align(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_UNIX)
  TracyCZoneEnd(__tracy_zone_ctx);
  return vmem_page_size();
#else
  TracyCZoneEnd(__tracy_zone_ctx);
  return align_of(max_align_t);
#endif
}

func void* vmem_alloc(sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  g_vmem_stats.alloc_calls += 1;
  if (size == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  assert(size > 0);

  sz alloc_align = vmem_get_alloc_align();
  sz prefix_size = align_up(size_of(vmem_alloc_header), alloc_align);

  if (size > SZ_MAX - prefix_size) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }

  sz total_size = prefix_size + size;
  void* base_ptr = vmem_platform_alloc(total_size);
  if (!base_ptr) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }

  u8* user_ptr = (u8*)base_ptr + prefix_size;
  vmem_alloc_header* header = ((vmem_alloc_header*)user_ptr) - 1;
  header->info.mapping_size = total_size;
  header->info.prefix_size = prefix_size;
  header->info.user_size = size;
  g_vmem_stats.live_allocations += 1;
  g_vmem_stats.live_allocated_bytes += size;
  g_vmem_stats.total_allocated_bytes += size;
  vmem_update_peak_allocated_bytes();
  TracyCZoneEnd(__tracy_zone_ctx);
  return user_ptr;
}

func void* vmem_calloc(sz count, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  g_vmem_stats.calloc_calls += 1;
  if (count == 0 || size == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }

  if (count > SZ_MAX / size) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }

  sz total_size = count * size;
  void* ptr = vmem_alloc(total_size);
  if (ptr) {
    memset(ptr, 0, total_size);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return ptr;
}

func void* vmem_realloc(void* ptr, sz old_size, sz new_size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  g_vmem_stats.realloc_calls += 1;
  (void)old_size;
  if (!ptr) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return vmem_alloc(new_size);
  }

  if (new_size == 0) {
    (void)vmem_free(ptr, 0);
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  assert(new_size > 0);

  vmem_alloc_header* old_header = vmem_get_alloc_header(ptr);
  if (new_size == old_header->info.user_size) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return ptr;
  }

  void* new_ptr = vmem_alloc(new_size);
  if (!new_ptr) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }

  sz copy_size = old_header->info.user_size < new_size ? old_header->info.user_size : new_size;
  memcpy(new_ptr, ptr, copy_size);
  (void)vmem_free(ptr, 0);
  TracyCZoneEnd(__tracy_zone_ctx);
  return new_ptr;
}

func b32 vmem_free(void* ptr, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  g_vmem_stats.free_calls += 1;
  (void)size;
  if (!ptr) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  vmem_alloc_header* header = vmem_get_alloc_header(ptr);
  sz mapping_size = header->info.mapping_size;
  sz user_size = header->info.user_size;
  void* base_ptr = vmem_get_alloc_base(ptr);
  b32 success = vmem_platform_free(base_ptr, mapping_size);
  if (success) {
    if (g_vmem_stats.live_allocations > 0) {
      g_vmem_stats.live_allocations -= 1;
    }

    if (g_vmem_stats.live_allocated_bytes >= user_size) {
      g_vmem_stats.live_allocated_bytes -= user_size;
    } else {
      g_vmem_stats.live_allocated_bytes = 0;
    }
    g_vmem_stats.total_freed_bytes += user_size;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return success;
}

// =========================================================================
// Allocator Callbacks
// =========================================================================

func void* vmem_alloc_callback(void* user_data, callsite site, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  (void)user_data;
  (void)site;
  TracyCZoneEnd(__tracy_zone_ctx);
  return vmem_alloc(size);
}

func void vmem_dealloc_callback(void* user_data, callsite site, void* ptr) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  (void)user_data;
  (void)site;
  (void)vmem_free(ptr, 0);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void* vmem_realloc_callback(
    void* user_data,
    callsite site,
    void* ptr,
    sz old_size,
    sz new_size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  (void)user_data;
  (void)site;
  TracyCZoneEnd(__tracy_zone_ctx);
  return vmem_realloc(ptr, old_size, new_size);
}

func allocator vmem_get_allocator(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator alloc;
  alloc.user_data = NULL;
  alloc.alloc_fn = vmem_alloc_callback;
  alloc.dealloc_fn = vmem_dealloc_callback;
  alloc.realloc_fn = vmem_realloc_callback;
  TracyCZoneEnd(__tracy_zone_ctx);
  return alloc;
}

func vmem_stats vmem_get_stats(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  vmem_stats stats;
  stats.page_size = vmem_page_size();

  stats.reserve_calls = g_vmem_stats.reserve_calls;
  stats.commit_calls = g_vmem_stats.commit_calls;
  stats.decommit_calls = g_vmem_stats.decommit_calls;
  stats.release_calls = g_vmem_stats.release_calls;

  stats.reserved_bytes = g_vmem_stats.reserved_bytes;
  stats.committed_bytes = g_vmem_stats.committed_bytes;
  stats.decommitted_bytes = g_vmem_stats.decommitted_bytes;
  stats.released_bytes = g_vmem_stats.released_bytes;

  stats.alloc_calls = g_vmem_stats.alloc_calls;
  stats.calloc_calls = g_vmem_stats.calloc_calls;
  stats.realloc_calls = g_vmem_stats.realloc_calls;
  stats.free_calls = g_vmem_stats.free_calls;

  stats.live_allocations = g_vmem_stats.live_allocations;
  stats.live_allocated_bytes = g_vmem_stats.live_allocated_bytes;
  stats.peak_live_allocated_bytes = g_vmem_stats.peak_live_allocated_bytes;
  stats.total_allocated_bytes = g_vmem_stats.total_allocated_bytes;
  stats.total_freed_bytes = g_vmem_stats.total_freed_bytes;
  TracyCZoneEnd(__tracy_zone_ctx);
  return stats;
}

