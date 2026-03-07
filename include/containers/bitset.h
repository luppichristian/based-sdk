// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "basic/intrinsics.h"

// A bitset operates on plain u64[] arrays with no wrapping struct.
// Capacity is implicit in the array size; use BITSET_WORD_COUNT(n) to size
// the array for n bits.  All bit indices are zero-based.
//
// Example:
//   u64 free_ids[BITSET_WORD_COUNT(256)];   // 256-bit set
//   BITSET_SET_ALL(free_ids, BITSET_WORD_COUNT(256));
//   BITSET_FOREACH_SET(free_ids, BITSET_WORD_COUNT(256), idx) { use(idx); }

// =========================================================================
// Sizing
// =========================================================================

// BITSET_WORD_COUNT — number of u64 words required to hold n bits.
#define BITSET_WORD_COUNT(n) (((n) + 63) / 64)

// =========================================================================
// Single-Bit Operations
// =========================================================================

// BITSET_SET — set the bit at index idx.
#define BITSET_SET(arr, idx) \
  expr(((arr)[(idx) / 64] |= (1ULL << ((idx) % 64))))

// BITSET_CLEAR — clear the bit at index idx.
#define BITSET_CLEAR(arr, idx) \
  expr(((arr)[(idx) / 64] &= ~(1ULL << ((idx) % 64))))

// BITSET_TOGGLE — toggle the bit at index idx.
#define BITSET_TOGGLE(arr, idx) \
  expr(((arr)[(idx) / 64] ^= (1ULL << ((idx) % 64))))

// BITSET_TEST — evaluates to non-zero if the bit at index idx is set.
#define BITSET_TEST(arr, idx) \
  (((arr)[(idx) / 64] >> ((idx) % 64)) & 1ULL)

// =========================================================================
// Bulk Operations
// =========================================================================

// BITSET_CLEAR_ALL — clear every bit across all word_count words.
#define BITSET_CLEAR_ALL(arr, word_count) expr({       \
  for (sz _wrd = 0; _wrd < (sz)(word_count); _wrd++) { \
    (arr)[_wrd] = 0ULL;                                \
  }                                                    \
})

// BITSET_SET_ALL — set every bit across all word_count words.
#define BITSET_SET_ALL(arr, word_count) expr({         \
  for (sz _wrd = 0; _wrd < (sz)(word_count); _wrd++) { \
    (arr)[_wrd] = ~0ULL;                               \
  }                                                    \
})

// =========================================================================
// Aggregate Queries
// =========================================================================

// BITSET_COUNT — store the total number of set bits across all words into out.
// out must be an i32 lvalue.
#define BITSET_COUNT(arr, word_count, out) expr({      \
  (out) = 0;                                           \
  for (sz _wrd = 0; _wrd < (sz)(word_count); _wrd++) { \
    (out) += popcount_u64((arr)[_wrd]);                \
  }                                                    \
})

// BITSET_FIRST_SET — store the index of the lowest set bit into out, or -1 if none.
// out must be an i32 lvalue.
#define BITSET_FIRST_SET(arr, word_count, out) expr({   \
  (out) = -1;                                           \
  for (sz _wrd = 0; _wrd < (sz)(word_count); _wrd++) {  \
    if ((arr)[_wrd] != 0ULL) {                          \
      (out) = (i32)(_wrd * 64U) + ctz_u64((arr)[_wrd]); \
      break;                                            \
    }                                                   \
  }                                                     \
})

// BITSET_FIRST_CLEAR — store the index of the lowest clear bit into out, or -1 if none.
// out must be an i32 lvalue.
#define BITSET_FIRST_CLEAR(arr, word_count, out) expr({  \
  (out) = -1;                                            \
  for (sz _wrd = 0; _wrd < (sz)(word_count); _wrd++) {   \
    if (~(arr)[_wrd] != 0ULL) {                          \
      (out) = (i32)(_wrd * 64U) + ctz_u64(~(arr)[_wrd]); \
      break;                                             \
    }                                                    \
  }                                                      \
})

// =========================================================================
// Iteration
// =========================================================================

// BITSET_FOREACH_SET iterates over every set bit, exposing its index as 'idx' (i32).
// Uses efficient word-level skipping via ctz — only words with set bits are scanned.
//
// Example:
//   BITSET_FOREACH_SET(arr, BITSET_WORD_COUNT(128), idx) {
//     printf("bit %d is set\n", idx);
//   }
#define BITSET_FOREACH_SET(arr, word_count, idx)                         \
  for (i32 idx = ({                                                      \
         i32 _fst = -1;                                                  \
         for (sz _fwi = 0; _fwi < (sz)(word_count); _fwi++) {            \
           if ((arr)[_fwi] != 0ULL) {                                    \
             _fst = (i32)(_fwi * 64U) + ctz_u64((arr)[_fwi]);            \
             break;                                                      \
           }                                                             \
         }                                                               \
         _fst;                                                           \
       });                                                               \
       (idx) != -1;                                                      \
       (idx) = ({                                                        \
         sz _nwi = (sz)((idx) + 1) / 64U;                                \
         i32 _nbi = ((idx) + 1) % 64;                                    \
         i32 _nxt = -1;                                                  \
         if (_nwi < (sz)(word_count)) {                                  \
           u64 _nword = (arr)[_nwi] >> (u32)_nbi;                        \
           if (_nword != 0ULL) {                                         \
             _nxt = (i32)(_nwi * 64U) + _nbi + ctz_u64(_nword);          \
           } else {                                                      \
             for (sz _nwj = _nwi + 1; _nwj < (sz)(word_count); _nwj++) { \
               if ((arr)[_nwj] != 0ULL) {                                \
                 _nxt = (i32)(_nwj * 64U) + ctz_u64((arr)[_nwj]);        \
                 break;                                                  \
               }                                                         \
             }                                                           \
           }                                                             \
         }                                                               \
         _nxt;                                                           \
       }))
