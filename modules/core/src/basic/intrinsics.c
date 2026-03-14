// MIT License
// Copyright (c) 2026 Christian Luppi

#include "basic/intrinsics.h"
#include "basic/assert.h"
#include "basic/profiler.h"
#include "../sdl3_include.h"
#include "basic/safe.h"

#if defined(PLATFORM_WINDOWS)
#  include <intrin.h>
#  include <stdlib.h>
#endif

// =========================================================================
// Population Count
// =========================================================================

func i32 popcount_u32(u32 val) {
  profile_func_begin;
  i32 res = __builtin_popcount(val);
  profile_func_end;
  return res;
}

func i32 popcount_u64(u64 val) {
  profile_func_begin;
  i32 res = __builtin_popcountll(val);
  profile_func_end;
  return res;
}

// =========================================================================
// Count Trailing Zeros
// =========================================================================

func i32 ctz_u32(u32 val) {
  profile_func_begin;
  if (val == 0) {
    profile_func_end;
    return 32;
  }
  assert(val != 0);
  i32 res = __builtin_ctz(val);
  profile_func_end;
  return res;
}

func i32 ctz_u64(u64 val) {
  profile_func_begin;
  if (val == 0) {
    profile_func_end;
    return 64;
  }
  assert(val != 0);
  u32 res = __builtin_ctzll(val);
  profile_func_end;
  return res;
}

// =========================================================================
// Count Leading Zeros
// =========================================================================

func i32 clz_u32(u32 val) {
  profile_func_begin;
  if (val == 0) {
    profile_func_end;
    return 32;
  }
  assert(val != 0);
  i32 res = __builtin_clz(val);
  profile_func_end;
  return res;
}

func i32 clz_u64(u64 val) {
  profile_func_begin;
  if (val == 0) {
    profile_func_end;
    return 64;
  }
  assert(val != 0);
  i32 res = __builtin_clzll(val);
  profile_func_end;
  return res;
}

// =========================================================================
// Bit Scan Reverse
// =========================================================================

func i32 bsr_u32(u32 val) {
  profile_func_begin;
  if (val == 0) {
    profile_func_end;
    return 0;
  }
  assert(val != 0);
  i32 res = 31 - __builtin_clz(val);
  profile_func_end;
  return res;
}

func i32 bsr_u64(u64 val) {
  profile_func_begin;
  if (val == 0) {
    profile_func_end;
    return 0;
  }
  assert(val != 0);
  i32 res = 63 - __builtin_clzll(val);
  profile_func_end;
  return res;
}

// =========================================================================
// Byte Swap
// =========================================================================

func u16 bswap_u16(u16 val) {
  profile_func_begin;
  u16 res = __builtin_bswap16(val);
  profile_func_end;
  return res;
}

func u32 bswap_u32(u32 val) {
  profile_func_begin;
  u32 res = __builtin_bswap32(val);
  profile_func_end;
  return res;
}

func u64 bswap_u64(u64 val) {
  profile_func_begin;
  u64 res = __builtin_bswap64(val);
  profile_func_end;
  return res;
}

// =========================================================================
// Bit Rotate
// =========================================================================

func u32 rotl_u32(u32 val, i32 cnt) {
  profile_func_begin;
  u32 ucnt = (u32)cnt & 31U;
  u32 res = (ucnt != 0U) ? ((val << ucnt) | (val >> (32U - ucnt))) : val;
  profile_func_end;
  return res;
}

func u64 rotl_u64(u64 val, i32 cnt) {
  profile_func_begin;
  u32 ucnt = (u32)cnt & 63U;
  u64 res = (ucnt != 0U) ? ((val << ucnt) | (val >> (64U - ucnt))) : val;
  profile_func_end;
  return res;
}

func u32 rotr_u32(u32 val, i32 cnt) {
  profile_func_begin;
  u32 ucnt = (u32)cnt & 31U;
  u32 res = (ucnt != 0U) ? ((val >> ucnt) | (val << (32U - ucnt))) : val;
  profile_func_end;
  return res;
}

func u64 rotr_u64(u64 val, i32 cnt) {
  profile_func_begin;
  u32 ucnt = (u32)cnt & 63U;
  u64 res = (ucnt != 0U) ? ((val >> ucnt) | (val << (64U - ucnt))) : val;
  profile_func_end;
  return res;
}