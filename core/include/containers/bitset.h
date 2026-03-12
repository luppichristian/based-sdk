// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "basic/intrinsics.h"
#include "basic/safe.h"

// =========================================================================
c_begin;
// =========================================================================

/*
BITSET_* treats a `u64` array as a packed bitset. The caller owns storage.

Example:

  enum { PERM_COUNT = 96 };
  u64 permissions[BITSET_WORD_COUNT(PERM_COUNT)] = {0};

  BITSET_SET(permissions, 5);  // enable bit 5
  BITSET_FOREACH_SET(permissions, BITSET_WORD_COUNT(PERM_COUNT), idx) {
    // idx is each enabled bit index
  }
*/

// Returns number of u64 words required to store n bits.
#define BITSET_WORD_COUNT(n) (((n) + 63) / 64)

// Single-bit operations.
#define BITSET_SET(arr, idx)    stmt((arr)[(idx) / 64] |= (1ULL << ((idx) % 64));)
#define BITSET_CLEAR(arr, idx)  stmt((arr)[(idx) / 64] &= ~(1ULL << ((idx) % 64));)
#define BITSET_TOGGLE(arr, idx) stmt((arr)[(idx) / 64] ^= (1ULL << ((idx) % 64));)
#define BITSET_TEST(arr, idx)   (((arr)[(idx) / 64] >> ((idx) % 64)) & 1ULL)

// Whole-set operations.
#define BITSET_CLEAR_ALL(arr, word_count) stmt(                         \
    safe_for (sz _word_idx = 0; _word_idx < (sz)(word_count); _word_idx++) { \
      (arr)[_word_idx] = 0ULL;                                          \
    })

#define BITSET_SET_ALL(arr, word_count) stmt(                           \
    safe_for (sz _word_idx = 0; _word_idx < (sz)(word_count); _word_idx++) { \
      (arr)[_word_idx] = ~0ULL;                                         \
    })

#define BITSET_COUNT(arr, word_count, out) stmt(                        \
    (out) = 0;                                                          \
    safe_for (sz _word_idx = 0; _word_idx < (sz)(word_count); _word_idx++) { \
      (out) += popcount_u64((arr)[_word_idx]);                          \
    })

#define BITSET_FIRST_SET(arr, word_count, out) stmt(                    \
    (out) = -1;                                                         \
    safe_for (sz _word_idx = 0; _word_idx < (sz)(word_count); _word_idx++) { \
      if ((arr)[_word_idx] != 0ULL) {                                   \
        (out) = (i32)(_word_idx * 64U) + ctz_u64((arr)[_word_idx]);     \
        break;                                                          \
      }                                                                 \
    })

#define BITSET_FIRST_CLEAR(arr, word_count, out) stmt(                  \
    (out) = -1;                                                         \
    safe_for (sz _word_idx = 0; _word_idx < (sz)(word_count); _word_idx++) { \
      if (~(arr)[_word_idx] != 0ULL) {                                  \
        (out) = (i32)(_word_idx * 64U) + ctz_u64(~(arr)[_word_idx]);    \
        break;                                                          \
      }                                                                 \
    })

#define BITSET_FIND_NEXT_SET(arr, word_count, from_idx, out) stmt(                             \
    const u64* _bitset_arr = (arr);                                                            \
    i32 _bitset_next_idx = (from_idx) < 0 ? 0 : (from_idx);                                    \
    sz _bitset_word_idx = (sz)_bitset_next_idx / 64U;                                          \
    i32 _bitset_bit_idx = _bitset_next_idx % 64;                                               \
    (out) = -1;                                                                                \
    if (_bitset_arr != nullptr && _bitset_word_idx < (sz)(word_count)) {                       \
      u64 _bitset_word = _bitset_arr[_bitset_word_idx] & (~0ULL << (u32)_bitset_bit_idx);      \
      if (_bitset_word != 0ULL) {                                                              \
        (out) = (i32)(_bitset_word_idx * 64U) + ctz_u64(_bitset_word);                         \
      } else {                                                                                 \
        safe_for (_bitset_word_idx += 1; _bitset_word_idx < (sz)(word_count); _bitset_word_idx++) { \
          if (_bitset_arr[_bitset_word_idx] != 0ULL) {                                         \
            (out) = (i32)(_bitset_word_idx * 64U) + ctz_u64(_bitset_arr[_bitset_word_idx]);    \
            break;                                                                             \
          }                                                                                    \
        }                                                                                      \
      }                                                                                        \
    })

// Typed traversal macro over all set bits.
#define BITSET_FOREACH_SET(arr, word_count, idx) \
  safe_for (i32 idx = ({ i32 _bitset_idx = -1; BITSET_FIND_NEXT_SET((arr), (word_count), 0, _bitset_idx); _bitset_idx; });                          \
       (idx) != -1;                              \
       (idx) = ({ i32 _bitset_idx = -1; BITSET_FIND_NEXT_SET((arr), (word_count), (idx) + 1, _bitset_idx); _bitset_idx; }))

// =========================================================================
c_end;
// =========================================================================
