// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "env_defines.h"
#include "keyword_defines.h"
#include "primitive_types.h"

// =========================================================================
// Population Count
// =========================================================================

// popcount_u32 — number of set bits in a 32-bit word.
func i32 popcount_u32(u32 val);

// popcount_u64 — number of set bits in a 64-bit word.
func i32 popcount_u64(u64 val);

// =========================================================================
// Count Trailing Zeros
// =========================================================================

// ctz_u32 — index of the lowest set bit in a 32-bit word (undefined for val == 0).
func i32 ctz_u32(u32 val);

// ctz_u64 — index of the lowest set bit in a 64-bit word (undefined for val == 0).
func i32 ctz_u64(u64 val);

// =========================================================================
// Count Leading Zeros
// =========================================================================

// clz_u32 — number of leading zero bits in a 32-bit word (undefined for val == 0).
func i32 clz_u32(u32 val);

// clz_u64 — number of leading zero bits in a 64-bit word (undefined for val == 0).
func i32 clz_u64(u64 val);

// =========================================================================
// Bit Scan Reverse
// =========================================================================

// bsr_u32 — index of the highest set bit in a 32-bit word (undefined for val == 0).
func i32 bsr_u32(u32 val);

// bsr_u64 — index of the highest set bit in a 64-bit word (undefined for val == 0).
func i32 bsr_u64(u64 val);

// =========================================================================
// Byte Swap
// =========================================================================

// bswap_u16 — reverse the byte order of a 16-bit value.
func u16 bswap_u16(u16 val);

// bswap_u32 — reverse the byte order of a 32-bit value.
func u32 bswap_u32(u32 val);

// bswap_u64 — reverse the byte order of a 64-bit value.
func u64 bswap_u64(u64 val);

// =========================================================================
// Bit Rotate
// =========================================================================

// rotl_u32 — rotate a 32-bit value left by cnt bits.
func u32 rotl_u32(u32 val, i32 cnt);

// rotl_u64 — rotate a 64-bit value left by cnt bits.
func u64 rotl_u64(u64 val, i32 cnt);

// rotr_u32 — rotate a 32-bit value right by cnt bits.
func u32 rotr_u32(u32 val, i32 cnt);

// rotr_u64 — rotate a 64-bit value right by cnt bits.
func u64 rotr_u64(u64 val, i32 cnt);
