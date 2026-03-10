// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "basic/intrinsics.h"

// =========================================================================
c_begin;
// =========================================================================

// Returns number of u64 words required to store n bits.
#define BITSET_WORD_COUNT(n) (((n) + 63) / 64)

// Single-bit operations.
#define BITSET_SET(arr, idx)    stmt((arr)[(idx) / 64] |= (1ULL << ((idx) % 64));)
#define BITSET_CLEAR(arr, idx)  stmt((arr)[(idx) / 64] &= ~(1ULL << ((idx) % 64));)
#define BITSET_TOGGLE(arr, idx) stmt((arr)[(idx) / 64] ^= (1ULL << ((idx) % 64));)
#define BITSET_TEST(arr, idx)   (((arr)[(idx) / 64] >> ((idx) % 64)) & 1ULL)

// Whole-set operations.
#define BITSET_CLEAR_ALL(arr, word_count) stmt(                         \
    for (sz _word_idx = 0; _word_idx < (sz)(word_count); _word_idx++) { \
      (arr)[_word_idx] = 0ULL;                                          \
    })

#define BITSET_SET_ALL(arr, word_count) stmt(                           \
    for (sz _word_idx = 0; _word_idx < (sz)(word_count); _word_idx++) { \
      (arr)[_word_idx] = ~0ULL;                                         \
    })

#define BITSET_COUNT(arr, word_count, out) stmt(                        \
    (out) = 0;                                                          \
    for (sz _word_idx = 0; _word_idx < (sz)(word_count); _word_idx++) { \
      (out) += popcount_u64((arr)[_word_idx]);                          \
    })

#define BITSET_FIRST_SET(arr, word_count, out) stmt(                    \
    (out) = -1;                                                         \
    for (sz _word_idx = 0; _word_idx < (sz)(word_count); _word_idx++) { \
      if ((arr)[_word_idx] != 0ULL) {                                   \
        (out) = (i32)(_word_idx * 64U) + ctz_u64((arr)[_word_idx]);     \
        break;                                                          \
      }                                                                 \
    })

#define BITSET_FIRST_CLEAR(arr, word_count, out) stmt(                  \
    (out) = -1;                                                         \
    for (sz _word_idx = 0; _word_idx < (sz)(word_count); _word_idx++) { \
      if (~(arr)[_word_idx] != 0ULL) {                                  \
        (out) = (i32)(_word_idx * 64U) + ctz_u64(~(arr)[_word_idx]);    \
        break;                                                          \
      }                                                                 \
    })

// TODO: Can we implement this directly in the macro?
// Returns the first set bit at or after from_idx, or -1 when not found.
func force_inline i32 bitset_find_next_set(const u64* arr, sz word_count, i32 from_idx) {
  i32 next_idx = from_idx < 0 ? 0 : from_idx;
  sz word_idx = (sz)next_idx / 64U;
  i32 bit_idx = next_idx % 64;

  if (arr == NULL || word_idx >= word_count) {
    return -1;
  }

  u64 word = arr[word_idx] & (~0ULL << (u32)bit_idx);
  if (word != 0ULL) {
    return (i32)(word_idx * 64U) + ctz_u64(word);
  }

  for (word_idx += 1; word_idx < word_count; word_idx++) {
    if (arr[word_idx] != 0ULL) {
      return (i32)(word_idx * 64U) + ctz_u64(arr[word_idx]);
    }
  }

  return -1;
}

// Typed traversal macro over all set bits.
#define BITSET_FOREACH_SET(arr, word_count, idx)                                \
  for (i32 idx = bitset_find_next_set((arr), (sz)(word_count), 0); (idx) != -1; \
       (idx) = bitset_find_next_set((arr), (sz)(word_count), (idx) + 1))

// =========================================================================
c_end;
// =========================================================================
