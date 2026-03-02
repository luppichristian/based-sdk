// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

// Required for offsetof and size_t.
#include <stddef.h>

/* General-purpose utility macros used throughout the project.

Stringification and token-pasting:
- 'stringify(x)'                    turns x into a string literal without expanding macros.
- 'stringify_exp(x)'                turns x into a string literal after macro expansion.
- 'concat(x, y)'                    pastes two tokens together without expanding macros.
- 'concat_exp(x, y)'                pastes two tokens together after macro expansion.
- 'expression(x)'                   evaluates x as an expression.

Array utilities:
- 'countof(x)'                      number of elements in a fixed-size array.
- 'sizeof_each(x)'                  size in bytes of a single element of a fixed-size array.
- 'multiline_literal'               converts a multi-line token sequence into a string literal.

Bit utilities:
- 'bit(x)'                          value with bit x set (1 << x).
- 'bit_is_set(bits, b)'             non-zero if bit b is set in bits.
- 'bit_set(bits, b)'                sets bit b in bits.
- 'bit_unset(bits, b)'              clears bit b in bits.
- 'bit_toggle(bits, b)'             toggles bit b in bits.

Struct / pointer utilities:
- 'field_sizeof(type, field)'       size in bytes of a field within a struct.
- 'offset_of(type, field)'          byte offset of a field within a struct.
- 'container_of(ptr, type, field)'  pointer to the enclosing struct given a pointer to one of its fields.

Range / bounds utilities:
- 'in_range(x, lo, hi)'             non-zero if lo <= x <= hi.
- 'in_bounds(arr, i)'               non-zero if index i is valid for fixed-size array arr.

Swap utility:
- 'swap(type, a, b)'                swaps the values of a and b using a temporary of the given type.

Alignment utilities (n must be a power of two):
- 'is_pow2(x)'                      non-zero if x is a non-zero power of two.
- 'align_up(x, n)'                  rounds x up to the next multiple of n.
- 'align_down(x, n)'                rounds x down to the nearest multiple of n.

Byte numeric utilities (powers of 1024):
- 'kb(b)' kilobytes — b * 1024.
- 'mb(b)' megabytes — kb(b) * 1024.
- 'gb(b)' gigabytes — mb(b) * 1024.
- 'tb(b)' terabytes — gb(b) * 1024.

Big numeric utilities (powers of 1000):
- 'th(x)'   thousands  — x * 1,000.
- 'mil(x)'  millions   — th(x) * 1,000.
- 'bil(x)'  billions   — mil(x) * 1,000.
- 'tril(x)' trillions  — bil(x) * 1,000.
*/

// =========================================================================
// Stringification and Token-Pasting
// =========================================================================

// stringify — converts x to a string literal without expanding macros.
#define stringify(x) #x

// stringify_exp — converts x to a string literal after macro expansion.
#define stringify_exp(x) stringify(x)

// concat — pastes two tokens together without expanding macros.
#define concat(x, y) x##y

// concat_exp — pastes two tokens together after macro expansion.
#define concat_exp(x, y) concat(x, y)

// expression — evaluates x as an expression, ensuring it is not treated as a statement.
#define expression(x) \
  do { (void)(x); } while (0)

// =========================================================================
// Array Utilities
// =========================================================================

// count_of — number of elements in a fixed-size array.
#define count_of(x) (sizeof(x) / sizeof((x)[0]))

// size_of_each — size in bytes of a single element of a fixed-size array.
#define size_of_each(x) sizeof((x)[0])

// multiline_literal — converts a multi-line token sequence into a string literal.
#define multiline_literal(...) stringify_exp(__VA_ARGS__)

// =========================================================================
// Bit Utilities
// =========================================================================

// bit — value with only bit x set.
#define bit(x) (1 << (x))

// bit_is_set — non-zero if bit b is set in bits.
#define bit_is_set(bits, b) ((bits) & (b))

// bit_set — sets bit b in bits.
#define bit_set(bits, b) ((bits) |= (b))

// bit_unset — clears bit b in bits.
#define bit_unset(bits, b) ((bits) &= ~(b))

// bit_toggle — toggles bit b in bits.
#define bit_toggle(bits, b) ((bits) ^= (b))

// =========================================================================
// Struct / Pointer Utilities
// =========================================================================

// field_sizeof — size in bytes of a field within a struct.
#define field_sizeof(type, field) sizeof(((type*)0)->field)

// offset_of — byte offset of a field within a struct.
#define offset_of(type, field) offsetof(type, field)

// container_of — pointer to the enclosing struct given a pointer to one of its fields.
#define container_of(ptr, type, field) ((type*)((char*)(ptr) - offset_of(type, field)))

// =========================================================================
// Range / Bounds Utilities
// =========================================================================

// in_range — non-zero if lo <= x <= hi.
#define in_range(x, lo, hi) ((x) >= (lo) && (x) <= (hi))

// in_bounds — non-zero if index i is a valid index for fixed-size array arr.
#define in_bounds(arr, i) ((i) >= 0 && (size_t)(i) < count_of(arr))

// =========================================================================
// Swap Utility
// =========================================================================

// swap — swaps the values of a and b using a temporary of the given type.
// Note: type must be provided explicitly; a and b must not have side effects.
#define swap(type, a, b)  \
  do {                    \
    type _swap_tmp = (a); \
    (a) = (b);            \
    (b) = _swap_tmp;      \
  } while (0)

// =========================================================================
// Alignment Utilities
// =========================================================================

// is_pow2 — non-zero if x is a non-zero power of two.
#define is_pow2(x) ((x) != 0 && ((x) & ((x) - 1)) == 0)

// align_up — rounds x up to the next multiple of n (n must be a power of two).
#define align_up(x, n) (((x) + (n) - 1) & ~((n) - 1))

// align_down — rounds x down to the nearest multiple of n (n must be a power of two).
#define align_down(x, n) ((x) & ~((n) - 1))

// =========================================================================
// Byte Numeric Utilities (powers of 1024)
// =========================================================================

#define kb(b) ((b) * 1024ll)
#define mb(b) (kb(b) * 1024ll)
#define gb(b) (mb(b) * 1024ll)
#define tb(b) (gb(b) * 1024ll)

// =========================================================================
// Big Numeric Utilities (powers of 1000)
// =========================================================================

#define th(x)   ((x) * 1000ll)
#define mil(x)  (th(x) * 1000ll)
#define bil(x)  (mil(x) * 1000ll)
#define tril(x) (bil(x) * 1000ll)
