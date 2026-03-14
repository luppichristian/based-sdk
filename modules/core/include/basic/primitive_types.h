// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "keyword_defines.h"

#include <float.h>
#include <stddef.h>
#include <stdint.h>

// =========================================================================
c_begin;
// =========================================================================

/*
Primitive types:
- `i8`: 8-bit signed integer
- `u8`: 8-bit unsigned integer
- `i16`: 16-bit signed integer
- `u16`: 16-bit unsigned integer
- `i32`: 32-bit signed integer
- `u32`: 32-bit unsigned integer
- `i64`: 64-bit signed integer
- `u64`: 64-bit unsigned integer
- `f32`: 32-bit floating-point number
- `f64`: 64-bit floating-point number
- `b8`: 8-bit boolean
- `b16`: 16-bit boolean
- `b32`: 32-bit boolean
- `b64`: 64-bit boolean
- `c8`: 8-bit character
- `c16`: 16-bit character
- `c32`: 32-bit character
- `i8x`: 8-bit fast signed integer (at least 8 bits)
- `u8x`: 8-bit fast unsigned integer (at least 8 bits)
- `i16x`: 16-bit fast signed integer (at least 16 bits)
- `u16x`: 16-bit fast unsigned integer (at least 16 bits)
- `i32x`: 32-bit fast signed integer (at least 32 bits)
- `u32x`: 32-bit fast unsigned integer (at least 32 bits)
- `i64x`: 64-bit fast signed integer (at least 64 bits)
- `u64x`: 64-bit fast unsigned integer (at least 64 bits)
- `b8x`: 8-bit fast boolean (at least 8 bits)
- `b16x`: 16-bit fast boolean (at least 16 bits)
- `b32x`: 32-bit fast boolean (at least 32 bits)
- `b64x`: 64-bit fast boolean (at least 64 bits)
- `sz`: platform-width unsigned size type (`size_t`)
- `up`: platform-width unsigned pointer integer (`uintptr_t`)
- `sp`: platform-width signed pointer integer (`intptr_t`)
- `dp`: platform-width signed pointer difference (`ptrdiff_t`)

For each type the following constants are also defined:
- `TYPE_MIN`: Minimum value of the type (e.g., `I8_MIN`, `U8_MIN`, etc.)
- `TYPE_MAX`: Maximum value of the type (e.g., `I8_MAX`, `U8_MAX`, etc.)
*/

// =========================================================================
// Exact-width integer types
// =========================================================================

typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;

static const i8 I8_MIN = -128;
static const i8 I8_MAX = 127;
static const u8 U8_MIN = 0;
static const u8 U8_MAX = 255;
static const i16 I16_MIN = -32768;
static const i16 I16_MAX = 32767;
static const u16 U16_MIN = 0;
static const u16 U16_MAX = 65535;
static const i32 I32_MIN = -2147483647 - 1;
static const i32 I32_MAX = 2147483647;
static const u32 U32_MIN = 0;
static const u32 U32_MAX = 4294967295U;
static const i64 I64_MIN = -9223372036854775807LL - 1;
static const i64 I64_MAX = 9223372036854775807LL;
static const u64 U64_MIN = 0;
static const u64 U64_MAX = 18446744073709551615ULL;

// =========================================================================
// Floating-point types
// =========================================================================

typedef float f32;
typedef double f64;

static const f32 F32_MIN = -FLT_MAX;
static const f32 F32_MAX = FLT_MAX;
static const f64 F64_MIN = -DBL_MAX;
static const f64 F64_MAX = DBL_MAX;

// =========================================================================
// Boolean types
// =========================================================================

typedef uint8_t b8;
typedef uint16_t b16;
typedef uint32_t b32;
typedef uint64_t b64;

static const b8 B8_MIN = 0;
static const b8 B8_MAX = 1;
static const b16 B16_MIN = 0;
static const b16 B16_MAX = 1;
static const b32 B32_MIN = 0;
static const b32 B32_MAX = 1;
static const b64 B64_MIN = 0;
static const b64 B64_MAX = 1;

// =========================================================================
// Character types
// =========================================================================

typedef char c8;
typedef uint16_t c16;
typedef uint32_t c32;

static const c8 C8_MIN = 0;
static const c8 C8_MAX = (c8)255;
static const c16 C16_MIN = 0;
static const c16 C16_MAX = 65535;
static const c32 C32_MIN = 0;
static const c32 C32_MAX = 4294967295U;

// =========================================================================
// C String definitions
// =========================================================================

typedef const c8* cstr8;    // Null terminated UTF-8 C String
typedef const c16* cstr16;  // Null terminated UTF-16 C String
typedef const c32* cstr32;  // Null terminated UTF-32 C String

// =========================================================================
// Fast integer types (at least N bits, platform's fastest representation)
// =========================================================================

typedef int_fast8_t i8x;
typedef uint_fast8_t u8x;
typedef int_fast16_t i16x;
typedef uint_fast16_t u16x;
typedef int_fast32_t i32x;
typedef uint_fast32_t u32x;
typedef int_fast64_t i64x;
typedef uint_fast64_t u64x;
typedef uint_fast8_t b8x;
typedef uint_fast16_t b16x;
typedef uint_fast32_t b32x;
typedef uint_fast64_t b64x;

static const i8x I8X_MIN = INT_FAST8_MIN;
static const i8x I8X_MAX = INT_FAST8_MAX;
static const u8x U8X_MIN = 0;
static const u8x U8X_MAX = UINT_FAST8_MAX;
static const i16x I16X_MIN = INT_FAST16_MIN;
static const i16x I16X_MAX = INT_FAST16_MAX;
static const u16x U16X_MIN = 0;
static const u16x U16X_MAX = UINT_FAST16_MAX;
static const i32x I32X_MIN = INT_FAST32_MIN;
static const i32x I32X_MAX = INT_FAST32_MAX;
static const u32x U32X_MIN = 0;
static const u32x U32X_MAX = UINT_FAST32_MAX;
static const i64x I64X_MIN = INT_FAST64_MIN;
static const i64x I64X_MAX = INT_FAST64_MAX;
static const u64x U64X_MIN = 0;
static const u64x U64X_MAX = UINT_FAST64_MAX;
static const b8x B8X_MIN = 0;
static const b8x B8X_MAX = 1;
static const b16x B16X_MIN = 0;
static const b16x B16X_MAX = 1;
static const b32x B32X_MIN = 0;
static const b32x B32X_MAX = 1;
static const b64x B64X_MIN = 0;
static const b64x B64X_MAX = 1;

// =========================================================================
// Pointer and size types
// =========================================================================

typedef size_t sz;
typedef uintptr_t up;
typedef intptr_t sp;
typedef ptrdiff_t dp;

static const sz SZ_MIN = 0;
static const sz SZ_MAX = SIZE_MAX;
static const up UP_MIN = 0;
static const up UP_MAX = UINTPTR_MAX;
static const sp SP_MIN = INTPTR_MIN;
static const sp SP_MAX = INTPTR_MAX;
static const dp DP_MIN = PTRDIFF_MIN;
static const dp DP_MAX = PTRDIFF_MAX;

// =========================================================================
// Compile-time size guarantees
// =========================================================================

static_assert(size_of(i8) == 1);
static_assert(size_of(u8) == 1);
static_assert(size_of(i16) == 2);
static_assert(size_of(u16) == 2);
static_assert(size_of(i32) == 4);
static_assert(size_of(u32) == 4);
static_assert(size_of(i64) == 8);
static_assert(size_of(u64) == 8);
static_assert(size_of(f32) == 4);
static_assert(size_of(f64) == 8);
static_assert(size_of(b8) == 1);
static_assert(size_of(b16) == 2);
static_assert(size_of(b32) == 4);
static_assert(size_of(b64) == 8);
static_assert(size_of(c8) == 1);
static_assert(size_of(c16) == 2);
static_assert(size_of(c32) == 4);
static_assert(size_of(i8x) >= 1);
static_assert(size_of(u8x) >= 1);
static_assert(size_of(i16x) >= 2);
static_assert(size_of(u16x) >= 2);
static_assert(size_of(i32x) >= 4);
static_assert(size_of(u32x) >= 4);
static_assert(size_of(i64x) >= 8);
static_assert(size_of(u64x) >= 8);
static_assert(size_of(b8x) >= 1);
static_assert(size_of(b16x) >= 2);
static_assert(size_of(b32x) >= 4);
static_assert(size_of(b64x) >= 8);

// =========================================================================
c_end;
// =========================================================================