// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include <stddef.h>

// =========================================================================
c_begin;
// =========================================================================

// =========================================================================
// Preprocessor helpers
// =========================================================================

// Stringify and token-concatenation helpers with expansion support.
#define strfy(x)      #x
#define strfy_exp(x)  strfy(x)
#define cat(x, y)     x##y
#define cat_exp(x, y) cat(x, y)

// =========================================================================
// Expression and statement wrappers
// =========================================================================

// Expression helper that preserves expression semantics.
#define expr(x) (x)

// Statement helper for block-style macro bodies.
#define stmt(x) \
  do { x } while (0)

// =========================================================================
// Array and literal helpers
// =========================================================================

// Array-only helpers.
#define count_of(x)            (size_of(x) / size_of((x)[0]))
#define size_of_each(x)        size_of((x)[0])
#define multiline_literal(...) strfy_exp(__VA_ARGS__)

// =========================================================================
// Bit helpers
// =========================================================================

#define bit(x)              (1 << (x))
#define bit64(x)            (1ull << (x))
#define bit_is_set(bits, b) ((bits) & (b))
#define bit_set(bits, b)    ((bits) |= (b))
#define bit_unset(bits, b)  ((bits) &= ~(b))
#define bit_toggle(bits, b) ((bits) ^= (b))

// =========================================================================
// Type layout helpers
// =========================================================================

#define field_of(type, field)          ((type*)0)->field
#define size_of_field(type, field)     size_of(field_of(type, field))
#define offset_of(type, field)         offsetof(type, field)
#define container_of(ptr, type, field) ((type*)((char*)(ptr) - offset_of(type, field)))

// =========================================================================
// Range helpers
// =========================================================================

#define in_range(x, lo, hi) ((x) >= (lo) && (x) <= (hi))
#define in_bounds(arr, i)   ((i) >= 0 && (size_t)(i) < count_of(arr))

// =========================================================================
// Swap helpers
// =========================================================================

#define swap(type, a, b) stmt( \
    type _swap_tmp = (a);      \
    (a) = (b);                 \
    (b) = _swap_tmp;)

#define refswap(type, a_ptr, b_ptr) stmt( \
    type _refswap_tmp = *(a_ptr);         \
    *(a_ptr) = *(b_ptr);                  \
    *(b_ptr) = _refswap_tmp;)

// =========================================================================
// Alignment helpers
// =========================================================================

#define is_pow2(x)       ((x) != 0 && ((x) & ((x) - 1)) == 0)
#define align_up(x, n)   (((x) + (n) - 1) & ~((n) - 1))
#define align_down(x, n) ((x) & ~((n) - 1))

// =========================================================================
// Unit scale helpers
// =========================================================================

// Binary byte scale helpers.
#define kb(b) ((b) * 1024ll)
#define mb(b) (kb(b) * 1024ll)
#define gb(b) (mb(b) * 1024ll)
#define tb(b) (gb(b) * 1024ll)

// Decimal magnitude helpers.
#define th(x)   ((x) * 1000ll)
#define mil(x)  (th(x) * 1000ll)
#define bil(x)  (mil(x) * 1000ll)
#define tril(x) (bil(x) * 1000ll)

// =========================================================================
c_end;
// =========================================================================