// MIT License
// Copyright (c) 2026 Christian Luppi

#include "basic/intrinsics.h"
#include "basic/assert.h"
#include "basic/profiler.h"
#include "../sdl3_include.h"
#include "basic/safe.h"

#if defined(COMPILER_MSVC)
#  include <intrin.h>
#  include <stdlib.h>
#endif

// =========================================================================
// Population Count
// =========================================================================

func i32 popcount_u32(u32 val) {
  profile_func_begin;
#if defined(COMPILER_MSVC)
  profile_func_end;
  return (i32)__popcnt(val);
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
  i32 res = __builtin_popcount(val);
  profile_func_end;
  return res;
#else
  i32 cnt = 0;
  safe_while (val) {
    cnt += (i32)(val & 1U);
    val >>= 1;
  }
  profile_func_end;
  return cnt;
#endif
}

func i32 popcount_u64(u64 val) {
  profile_func_begin;
#if defined(COMPILER_MSVC)
  profile_func_end;
  return (i32)__popcnt64(val);
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
  i32 res = __builtin_popcountll(val);
  profile_func_end;
  return res;
#else
  i32 cnt = 0;
  safe_while (val) {
    cnt += (i32)(val & 1ULL);
    val >>= 1;
  }
  profile_func_end;
  return cnt;
#endif
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
#if defined(COMPILER_MSVC)
  unsigned long idx;
  _BitScanForward(&idx, val);
  profile_func_end;
  return (i32)idx;
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
  i32 res = __builtin_ctz(val);
  profile_func_end;
  return res;
#else
  i32 cnt = 0;
  safe_while ((val & 1U) == 0U) {
    cnt++;
    val >>= 1;
  }
  profile_func_end;
  return cnt;
#endif
}

func i32 ctz_u64(u64 val) {
  profile_func_begin;
  if (val == 0) {
    profile_func_end;
    return 64;
  }
  assert(val != 0);
#if defined(COMPILER_MSVC)
  unsigned long idx;
  _BitScanForward64(&idx, val);
  profile_func_end;
  return (i32)idx;
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
  u32 res = __builtin_ctzll(val);
  profile_func_end;
  return res;
#else
  i32 cnt = 0;
  safe_while ((val & 1ULL) == 0ULL) {
    cnt++;
    val >>= 1;
  }
  profile_func_end;
  return cnt;
#endif
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
#if defined(COMPILER_MSVC)
  unsigned long idx;
  _BitScanReverse(&idx, val);
  profile_func_end;
  return 31 - (i32)idx;
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
  i32 res = __builtin_clz(val);
  profile_func_end;
  return res;
#else
  i32 cnt = 0;
  u32 msk = 0x80000000U;
  safe_while (msk != 0U && (val & msk) == 0U) {
    cnt++;
    msk >>= 1U;
  }
  profile_func_end;
  return cnt;
#endif
}

func i32 clz_u64(u64 val) {
  profile_func_begin;
  if (val == 0) {
    profile_func_end;
    return 64;
  }
  assert(val != 0);
#if defined(COMPILER_MSVC)
  unsigned long idx;
  _BitScanReverse64(&idx, val);
  profile_func_end;
  return 63 - (i32)idx;
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
  i32 res = __builtin_clzll(val);
  profile_func_end;
  return res;
#else
  i32 cnt = 0;
  u64 msk = 0x8000000000000000ULL;
  safe_while (msk != 0ULL && (val & msk) == 0ULL) {
    cnt++;
    msk >>= 1U;
  }
  profile_func_end;
  return cnt;
#endif
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
#if defined(COMPILER_MSVC)
  unsigned long idx;
  _BitScanReverse(&idx, val);
  profile_func_end;
  return (i32)idx;
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
  i32 res = 31 - __builtin_clz(val);
  profile_func_end;
  return res;
#else
  i32 pos = 31;
  u32 msk = 0x80000000U;
  safe_while (msk != 0U) {
    if ((val & msk) != 0U) {
      profile_func_end;
      return pos;
    }
    msk >>= 1U;
    pos--;
  }
  profile_func_end;
  return 0;
#endif
}

func i32 bsr_u64(u64 val) {
  profile_func_begin;
  if (val == 0) {
    profile_func_end;
    return 0;
  }
  assert(val != 0);
#if defined(COMPILER_MSVC)
  unsigned long idx;
  _BitScanReverse64(&idx, val);
  profile_func_end;
  return (i32)idx;
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
  i32 res = 63 - __builtin_clzll(val);
  profile_func_end;
  return res;
#else
  i32 pos = 63;
  u64 msk = 0x8000000000000000ULL;
  safe_while (msk != 0ULL) {
    if ((val & msk) != 0ULL) {
      profile_func_end;
      return pos;
    }
    msk >>= 1U;
    pos--;
  }
  profile_func_end;
  return 0;
#endif
}

// =========================================================================
// Byte Swap
// =========================================================================

func u16 bswap_u16(u16 val) {
  profile_func_begin;
#if defined(COMPILER_MSVC)
  u16 res = _byteswap_ushort(val);
  profile_func_end;
  return res;
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
  u16 res = __builtin_bswap16(val);
  profile_func_end;
  return res;
#else
  u16 res = (u16)((val >> 8U) | (val << 8U));
  profile_func_end;
  return res;
#endif
}

func u32 bswap_u32(u32 val) {
  profile_func_begin;
#if defined(COMPILER_MSVC)
  u32 res = _byteswap_ulong(val);
  profile_func_end;
  return res;
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
  u32 res = __builtin_bswap32(val);
  profile_func_end;
  return res;
#else
  u32 res = ((val & 0xFF000000U) >> 24U) | ((val & 0x00FF0000U) >> 8U) |
            ((val & 0x0000FF00U) << 8U) | ((val & 0x000000FFU) << 24U);
  profile_func_end;
  return res;
#endif
}

func u64 bswap_u64(u64 val) {
  profile_func_begin;
#if defined(COMPILER_MSVC)
  u64 res = _byteswap_uint64(val);
  profile_func_end;
  return res;
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
  u64 res = __builtin_bswap64(val);
  profile_func_end;
  return res;
#else
  u64 res = ((val & 0xFF00000000000000ULL) >> 56U) |
            ((val & 0x00FF000000000000ULL) >> 40U) |
            ((val & 0x0000FF0000000000ULL) >> 24U) |
            ((val & 0x000000FF00000000ULL) >> 8U) |
            ((val & 0x00000000FF000000ULL) << 8U) |
            ((val & 0x0000000000FF0000ULL) << 24U) |
            ((val & 0x000000000000FF00ULL) << 40U) |
            ((val & 0x00000000000000FFULL) << 56U);
  profile_func_end;
  return res;
#endif
}

// =========================================================================
// Bit Rotate
// =========================================================================

func u32 rotl_u32(u32 val, i32 cnt) {
  profile_func_begin;
#if defined(COMPILER_MSVC)
  u32 res = _rotl(val, cnt);
  profile_func_end;
  return res;
#else
  u32 ucnt = (u32)cnt & 31U;
  u32 res = (ucnt != 0U) ? ((val << ucnt) | (val >> (32U - ucnt))) : val;
  profile_func_end;
  return res;
#endif
}

func u64 rotl_u64(u64 val, i32 cnt) {
  profile_func_begin;
#if defined(COMPILER_MSVC)
  u64 res = _rotl64(val, cnt);
  profile_func_end;
  return res;
#else
  u32 ucnt = (u32)cnt & 63U;
  u64 res = (ucnt != 0U) ? ((val << ucnt) | (val >> (64U - ucnt))) : val;
  profile_func_end;
  return res;
#endif
}

func u32 rotr_u32(u32 val, i32 cnt) {
  profile_func_begin;
#if defined(COMPILER_MSVC)
  u32 res = _rotr(val, cnt);
  profile_func_end;
  return res;
#else
  u32 ucnt = (u32)cnt & 31U;
  u32 res = (ucnt != 0U) ? ((val >> ucnt) | (val << (32U - ucnt))) : val;
  profile_func_end;
  return res;
#endif
}

func u64 rotr_u64(u64 val, i32 cnt) {
  profile_func_begin;
#if defined(COMPILER_MSVC)
  u64 res = _rotr64(val, cnt);
  profile_func_end;
  return res;
#else
  u32 ucnt = (u32)cnt & 63U;
  u64 res = (ucnt != 0U) ? ((val >> ucnt) | (val << (64U - ucnt))) : val;
  profile_func_end;
  return res;
#endif
}
