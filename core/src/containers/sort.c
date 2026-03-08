// MIT License
// Copyright (c) 2026 Christian Luppi

#include "containers/sort.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "memory/scratch.h"
#include "basic/profiler.h"
#include <string.h>

typedef struct sort_range {
  sz beg_idx;
  sz end_idx;
} sort_range;

func void* sort_elem_ptr(u8* base_ptr, sz idx, sz elem_size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return base_ptr + (idx * elem_size);
}

func const void* sort_elem_ptr_const(const u8* base_ptr, sz idx, sz elem_size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return base_ptr + (idx * elem_size);
}

func void sort_swap_bytes(void* lhs_ptr, void* rhs_ptr, sz elem_size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (lhs_ptr == rhs_ptr) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  u8* lhs_bytes = (u8*)lhs_ptr;
  u8* rhs_bytes = (u8*)rhs_ptr;

  for (sz byte_idx = 0; byte_idx < elem_size; ++byte_idx) {
    u8 tmp_byte = lhs_bytes[byte_idx];
    lhs_bytes[byte_idx] = rhs_bytes[byte_idx];
    rhs_bytes[byte_idx] = tmp_byte;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}

func b32 sort_is_invalid_input(
    const void* ptr,
    sz elem_count,
    sz elem_size,
    sort_compare_fn* compare) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (elem_count < 2) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (!ptr || !compare || !elem_size) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }
  assert(ptr != NULL);
  assert(compare != NULL);
  assert(elem_size > 0);

  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
}

func sz sort_partition(
    u8* base_ptr,
    sz beg_idx,
    sz end_idx,
    sz elem_size,
    sort_compare_fn* compare,
    void* user_data) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz piv_idx = beg_idx + ((end_idx - beg_idx) / 2);
  sz lst_idx = end_idx - 1;
  sort_swap_bytes(
      sort_elem_ptr(base_ptr, piv_idx, elem_size),
      sort_elem_ptr(base_ptr, lst_idx, elem_size),
      elem_size);

  sz stw_idx = beg_idx;
  for (sz cmp_idx = beg_idx; cmp_idx < lst_idx; ++cmp_idx) {
    void* cmp_ptr = sort_elem_ptr(base_ptr, cmp_idx, elem_size);
    void* piv_ptr = sort_elem_ptr(base_ptr, lst_idx, elem_size);
    if (compare(cmp_ptr, piv_ptr, user_data) < 0) {
      sort_swap_bytes(
          sort_elem_ptr(base_ptr, stw_idx, elem_size),
          sort_elem_ptr(base_ptr, cmp_idx, elem_size),
          elem_size);
      ++stw_idx;
    }
  }

  sort_swap_bytes(
      sort_elem_ptr(base_ptr, stw_idx, elem_size),
      sort_elem_ptr(base_ptr, lst_idx, elem_size),
      elem_size);
  TracyCZoneEnd(__tracy_zone_ctx);
  return stw_idx;
}

func void sort_quick_recursive(
    u8* base_ptr,
    sz beg_idx,
    sz end_idx,
    sz elem_size,
    sort_compare_fn* compare,
    void* user_data) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if ((end_idx - beg_idx) < 2) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  sz piv_idx = sort_partition(base_ptr, beg_idx, end_idx, elem_size, compare, user_data);

  sort_quick_recursive(base_ptr, beg_idx, piv_idx, elem_size, compare, user_data);
  sort_quick_recursive(base_ptr, piv_idx + 1, end_idx, elem_size, compare, user_data);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void sort_merge_ranges(
    u8* base_ptr,
    u8* tmp_ptr,
    sz left_idx,
    sz mid_idx,
    sz right_idx,
    sz elem_size,
    sort_compare_fn* compare,
    void* user_data) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz lhs_idx = left_idx;
  sz rhs_idx = mid_idx;
  sz out_idx = left_idx;

  while (lhs_idx < mid_idx && rhs_idx < right_idx) {
    const void* lhs_ptr = sort_elem_ptr_const(base_ptr, lhs_idx, elem_size);
    const void* rhs_ptr = sort_elem_ptr_const(base_ptr, rhs_idx, elem_size);
    if (compare(lhs_ptr, rhs_ptr, user_data) <= 0) {
      memcpy(sort_elem_ptr(tmp_ptr, out_idx, elem_size), lhs_ptr, elem_size);
      ++lhs_idx;
    } else {
      memcpy(sort_elem_ptr(tmp_ptr, out_idx, elem_size), rhs_ptr, elem_size);
      ++rhs_idx;
    }
    ++out_idx;
  }

  while (lhs_idx < mid_idx) {
    memcpy(
        sort_elem_ptr(tmp_ptr, out_idx, elem_size),
        sort_elem_ptr(base_ptr, lhs_idx, elem_size),
        elem_size);
    ++lhs_idx;
    ++out_idx;
  }

  while (rhs_idx < right_idx) {
    memcpy(
        sort_elem_ptr(tmp_ptr, out_idx, elem_size),
        sort_elem_ptr(base_ptr, rhs_idx, elem_size),
        elem_size);
    ++rhs_idx;
    ++out_idx;
  }

  memcpy(
      sort_elem_ptr(base_ptr, left_idx, elem_size),
      sort_elem_ptr(tmp_ptr, left_idx, elem_size),
      (right_idx - left_idx) * elem_size);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func sz sort_radix32_partition(u32* ptr, sz beg_idx, sz end_idx, u32 bit_mask) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if ((end_idx - beg_idx) < 2) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return beg_idx;
  }

  sz lhs_idx = beg_idx;
  sz rhs_idx = end_idx;

  while (lhs_idx < rhs_idx) {
    if ((ptr[lhs_idx] & bit_mask) == 0) {
      ++lhs_idx;
      continue;
    }

    do {
      --rhs_idx;
      if (lhs_idx >= rhs_idx) {
        break;
      }
    } while ((ptr[rhs_idx] & bit_mask) != 0);

    if (lhs_idx < rhs_idx) {
      u32 tmp_val = ptr[lhs_idx];
      ptr[lhs_idx] = ptr[rhs_idx];
      ptr[rhs_idx] = tmp_val;
      ++lhs_idx;
    }
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return lhs_idx;
}

func void sort_radix32_recursive(u32* ptr, sz beg_idx, sz end_idx, i32 bit_idx) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if ((end_idx - beg_idx) < 2 || bit_idx < 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  u32 bit_mask = ((u32)1U) << bit_idx;
  sz mid_idx = sort_radix32_partition(ptr, beg_idx, end_idx, bit_mask);
  sort_radix32_recursive(ptr, beg_idx, mid_idx, bit_idx - 1);
  sort_radix32_recursive(ptr, mid_idx, end_idx, bit_idx - 1);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func sz sort_radix64_partition(u64* ptr, sz beg_idx, sz end_idx, u64 bit_mask) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if ((end_idx - beg_idx) < 2) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return beg_idx;
  }

  sz lhs_idx = beg_idx;
  sz rhs_idx = end_idx;

  while (lhs_idx < rhs_idx) {
    if ((ptr[lhs_idx] & bit_mask) == 0) {
      ++lhs_idx;
      continue;
    }

    do {
      --rhs_idx;
      if (lhs_idx >= rhs_idx) {
        break;
      }
    } while ((ptr[rhs_idx] & bit_mask) != 0);

    if (lhs_idx < rhs_idx) {
      u64 tmp_val = ptr[lhs_idx];
      ptr[lhs_idx] = ptr[rhs_idx];
      ptr[rhs_idx] = tmp_val;
      ++lhs_idx;
    }
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return lhs_idx;
}

func void sort_radix64_recursive(u64* ptr, sz beg_idx, sz end_idx, i32 bit_idx) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if ((end_idx - beg_idx) < 2 || bit_idx < 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  u64 bit_mask = ((u64)1ULL) << bit_idx;
  sz mid_idx = sort_radix64_partition(ptr, beg_idx, end_idx, bit_mask);
  sort_radix64_recursive(ptr, beg_idx, mid_idx, bit_idx - 1);
  sort_radix64_recursive(ptr, mid_idx, end_idx, bit_idx - 1);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func b32 sort_check(
    const void* ptr,
    sz elem_count,
    sz elem_size,
    sort_compare_fn* compare,
    void* user_data) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (elem_count < 2) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  if (!ptr || !compare || !elem_size) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(ptr != NULL);
  assert(compare != NULL);

  const u8* base_ptr = (const u8*)ptr;
  for (sz idx = 1; idx < elem_count; ++idx) {
    const void* lhs_ptr = sort_elem_ptr_const(base_ptr, idx - 1, elem_size);
    const void* rhs_ptr = sort_elem_ptr_const(base_ptr, idx, elem_size);
    if (compare(lhs_ptr, rhs_ptr, user_data) > 0) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return 0;
    }
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func sz sort_bubble(
    void* ptr,
    sz elem_count,
    sz elem_size,
    sort_compare_fn* compare,
    void* user_data) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (elem_count < 2) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return elem_count;
  }

  if (sort_is_invalid_input(ptr, elem_count, elem_size, compare)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  u8* base_ptr = (u8*)ptr;
  for (sz out_idx = elem_count; out_idx > 1; --out_idx) {
    b32 swapped = 0;
    for (sz idx = 1; idx < out_idx; ++idx) {
      void* lhs_ptr = sort_elem_ptr(base_ptr, idx - 1, elem_size);
      void* rhs_ptr = sort_elem_ptr(base_ptr, idx, elem_size);
      if (compare(lhs_ptr, rhs_ptr, user_data) > 0) {
        sort_swap_bytes(lhs_ptr, rhs_ptr, elem_size);
        swapped = 1;
      }
    }
    if (!swapped) {
      break;
    }
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return elem_count;
}

func sz sort_quick(
    void* ptr,
    sz elem_count,
    sz elem_size,
    sort_compare_fn* compare,
    void* user_data,
    allocator allocator) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  (void)allocator;
  if (elem_count < 2) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return elem_count;
  }

  if (sort_is_invalid_input(ptr, elem_count, elem_size, compare)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  u8* base_ptr = (u8*)ptr;
  arena* temp_arena = thread_get_temp_arena();
  if (temp_arena == NULL) {
    sort_quick_recursive(base_ptr, 0, elem_count, elem_size, compare, user_data);
    TracyCZoneEnd(__tracy_zone_ctx);
    return elem_count;
  }

  if (elem_count > (((sz)-1) / size_of(sort_range))) {
    sort_quick_recursive(base_ptr, 0, elem_count, elem_size, compare, user_data);
    TracyCZoneEnd(__tracy_zone_ctx);
    return elem_count;
  }

  scratch temp_scope = scratch_begin(temp_arena);
  sort_range* stack_ptr = arena_alloc_array(temp_arena, sort_range, elem_count);
  if (!stack_ptr) {
    scratch_end(&temp_scope);
    sort_quick_recursive(base_ptr, 0, elem_count, elem_size, compare, user_data);
    TracyCZoneEnd(__tracy_zone_ctx);
    return elem_count;
  }

  sz top_idx = 0;
  stack_ptr[top_idx].beg_idx = 0;
  stack_ptr[top_idx].end_idx = elem_count;
  ++top_idx;

  while (top_idx) {
    --top_idx;
    sort_range cur_rng = stack_ptr[top_idx];
    if ((cur_rng.end_idx - cur_rng.beg_idx) < 2) {
      continue;
    }

    sz piv_idx =
        sort_partition(base_ptr, cur_rng.beg_idx, cur_rng.end_idx, elem_size, compare, user_data);

    sz left_count = piv_idx - cur_rng.beg_idx;
    sz right_count = cur_rng.end_idx - (piv_idx + 1);

    if (left_count > right_count) {
      if (left_count > 1) {
        stack_ptr[top_idx].beg_idx = cur_rng.beg_idx;
        stack_ptr[top_idx].end_idx = piv_idx;
        ++top_idx;
      }
      if (right_count > 1) {
        stack_ptr[top_idx].beg_idx = piv_idx + 1;
        stack_ptr[top_idx].end_idx = cur_rng.end_idx;
        ++top_idx;
      }
    } else {
      if (right_count > 1) {
        stack_ptr[top_idx].beg_idx = piv_idx + 1;
        stack_ptr[top_idx].end_idx = cur_rng.end_idx;
        ++top_idx;
      }
      if (left_count > 1) {
        stack_ptr[top_idx].beg_idx = cur_rng.beg_idx;
        stack_ptr[top_idx].end_idx = piv_idx;
        ++top_idx;
      }
    }
  }

  scratch_end(&temp_scope);
  TracyCZoneEnd(__tracy_zone_ctx);
  return elem_count;
}

func sz sort_merge(
    void* ptr,
    sz elem_count,
    sz elem_size,
    sort_compare_fn* compare,
    void* user_data,
    allocator allocator) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  (void)allocator;
  if (elem_count < 2) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return elem_count;
  }

  if (sort_is_invalid_input(ptr, elem_count, elem_size, compare)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  arena* temp_arena = thread_get_temp_arena();
  if (temp_arena == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return sort_quick(ptr, elem_count, elem_size, compare, user_data, allocator);
  }

  if (elem_count > (((sz)-1) / elem_size)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return sort_quick(ptr, elem_count, elem_size, compare, user_data, allocator);
  }

  sz temp_size = elem_count * elem_size;
  scratch temp_scope = scratch_begin(temp_arena);
  u8* tmp_ptr = (u8*)arena_alloc(temp_arena, temp_size, align_of(u8));
  if (!tmp_ptr) {
    scratch_end(&temp_scope);
    TracyCZoneEnd(__tracy_zone_ctx);
    return sort_quick(ptr, elem_count, elem_size, compare, user_data, allocator);
  }

  u8* base_ptr = (u8*)ptr;
  for (sz run_size = 1; run_size < elem_count;) {
    sz left_idx = 0;
    while (left_idx < elem_count) {
      sz mid_idx = elem_count;
      if ((elem_count - left_idx) > run_size) {
        mid_idx = left_idx + run_size;
      }

      sz right_idx = elem_count;
      if ((elem_count - mid_idx) > run_size) {
        right_idx = mid_idx + run_size;
      }

      if (mid_idx < right_idx) {
        sort_merge_ranges(
            base_ptr,
            tmp_ptr,
            left_idx,
            mid_idx,
            right_idx,
            elem_size,
            compare,
            user_data);
      }

      sz rem_count = elem_count - left_idx;
      if (rem_count <= run_size) {
        break;
      }
      if ((rem_count - run_size) <= run_size) {
        break;
      }
      left_idx += run_size;
      left_idx += run_size;
    }

    if (run_size > (elem_count / 2)) {
      break;
    }
    run_size *= 2;
  }

  scratch_end(&temp_scope);
  TracyCZoneEnd(__tracy_zone_ctx);
  return elem_count;
}

func sz sort_selection(
    void* ptr,
    sz elem_count,
    sz elem_size,
    sort_compare_fn* compare,
    void* user_data) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (elem_count < 2) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return elem_count;
  }

  if (sort_is_invalid_input(ptr, elem_count, elem_size, compare)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  u8* base_ptr = (u8*)ptr;
  for (sz out_idx = 0; out_idx < elem_count; ++out_idx) {
    sz min_idx = out_idx;
    for (sz idx = out_idx + 1; idx < elem_count; ++idx) {
      void* cur_ptr = sort_elem_ptr(base_ptr, idx, elem_size);
      void* min_ptr = sort_elem_ptr(base_ptr, min_idx, elem_size);
      if (compare(cur_ptr, min_ptr, user_data) < 0) {
        min_idx = idx;
      }
    }

    if (min_idx != out_idx) {
      sort_swap_bytes(
          sort_elem_ptr(base_ptr, out_idx, elem_size),
          sort_elem_ptr(base_ptr, min_idx, elem_size),
          elem_size);
    }
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return elem_count;
}

func sz sort_insertion(
    void* ptr,
    sz elem_count,
    sz elem_size,
    sort_compare_fn* compare,
    void* user_data) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (elem_count < 2) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return elem_count;
  }

  if (sort_is_invalid_input(ptr, elem_count, elem_size, compare)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  u8* base_ptr = (u8*)ptr;
  for (sz out_idx = 1; out_idx < elem_count; ++out_idx) {
    sz cur_idx = out_idx;
    while (cur_idx > 0) {
      void* lhs_ptr = sort_elem_ptr(base_ptr, cur_idx - 1, elem_size);
      void* rhs_ptr = sort_elem_ptr(base_ptr, cur_idx, elem_size);
      if (compare(lhs_ptr, rhs_ptr, user_data) <= 0) {
        break;
      }

      sort_swap_bytes(lhs_ptr, rhs_ptr, elem_size);
      --cur_idx;
    }
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return elem_count;
}

func sz sort_radix32(u32* ptr, sz elem_count) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (elem_count < 2) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return elem_count;
  }

  if (!ptr) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  sort_radix32_recursive(ptr, 0, elem_count, 31);
  TracyCZoneEnd(__tracy_zone_ctx);
  return elem_count;
}

func sz sort_radix64(u64* ptr, sz elem_count) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (elem_count < 2) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return elem_count;
  }

  if (!ptr) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  sort_radix64_recursive(ptr, 0, elem_count, 63);
  TracyCZoneEnd(__tracy_zone_ctx);
  return elem_count;
}
