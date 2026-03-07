// MIT License
// Copyright (c) 2026 Christian Luppi

#include "basic/intrinsics.h"
#include "basic/assert.h"
#include "basic/profiler.h"

#if defined(COMPILER_MSVC)
#  include <intrin.h>
#  include <stdlib.h>
#endif

// =========================================================================
// Population Count
// =========================================================================

func i32 popcount_u32(u32 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
#if defined(COMPILER_MSVC)
  TracyCZoneEnd(__tracy_zone_ctx);
  return (i32)__popcnt(val);
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
  TracyCZoneEnd(__tracy_zone_ctx);
  return __builtin_popcount(val);
#else
  i32 cnt = 0;
  while (val) {
    cnt += (i32)(val & 1U);
    val >>= 1;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return cnt;
#endif
}

func i32 popcount_u64(u64 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
#if defined(COMPILER_MSVC)
  TracyCZoneEnd(__tracy_zone_ctx);
  return (i32)__popcnt64(val);
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
  TracyCZoneEnd(__tracy_zone_ctx);
  return __builtin_popcountll(val);
#else
  i32 cnt = 0;
  while (val) {
    cnt += (i32)(val & 1ULL);
    val >>= 1;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return cnt;
#endif
}

// =========================================================================
// Count Trailing Zeros
// =========================================================================

func i32 ctz_u32(u32 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (val == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 32;
  }
  assert(val != 0);
#if defined(COMPILER_MSVC)
  unsigned long idx;
  _BitScanForward(&idx, val);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (i32)idx;
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
  TracyCZoneEnd(__tracy_zone_ctx);
  return __builtin_ctz(val);
#else
  i32 cnt = 0;
  while ((val & 1U) == 0U) {
    cnt++;
    val >>= 1;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return cnt;
#endif
}

func i32 ctz_u64(u64 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (val == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 64;
  }
  assert(val != 0);
#if defined(COMPILER_MSVC)
  unsigned long idx;
  _BitScanForward64(&idx, val);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (i32)idx;
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
  TracyCZoneEnd(__tracy_zone_ctx);
  return __builtin_ctzll(val);
#else
  i32 cnt = 0;
  while ((val & 1ULL) == 0ULL) {
    cnt++;
    val >>= 1;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return cnt;
#endif
}

// =========================================================================
// Count Leading Zeros
// =========================================================================

func i32 clz_u32(u32 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (val == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 32;
  }
  assert(val != 0);
#if defined(COMPILER_MSVC)
  unsigned long idx;
  _BitScanReverse(&idx, val);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 31 - (i32)idx;
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
  TracyCZoneEnd(__tracy_zone_ctx);
  return __builtin_clz(val);
#else
  i32 cnt = 0;
  u32 msk = 0x80000000U;
  while (msk != 0U && (val & msk) == 0U) {
    cnt++;
    msk >>= 1U;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return cnt;
#endif
}

func i32 clz_u64(u64 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (val == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 64;
  }
  assert(val != 0);
#if defined(COMPILER_MSVC)
  unsigned long idx;
  _BitScanReverse64(&idx, val);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 63 - (i32)idx;
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
  TracyCZoneEnd(__tracy_zone_ctx);
  return __builtin_clzll(val);
#else
  i32 cnt = 0;
  u64 msk = 0x8000000000000000ULL;
  while (msk != 0ULL && (val & msk) == 0ULL) {
    cnt++;
    msk >>= 1U;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return cnt;
#endif
}

// =========================================================================
// Bit Scan Reverse
// =========================================================================

func i32 bsr_u32(u32 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (val == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(val != 0);
#if defined(COMPILER_MSVC)
  unsigned long idx;
  _BitScanReverse(&idx, val);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (i32)idx;
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
  TracyCZoneEnd(__tracy_zone_ctx);
  return 31 - __builtin_clz(val);
#else
  i32 pos = 31;
  u32 msk = 0x80000000U;
  while (msk != 0U) {
    if ((val & msk) != 0U) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return pos;
    }
    msk >>= 1U;
    pos--;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
#endif
}

func i32 bsr_u64(u64 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (val == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(val != 0);
#if defined(COMPILER_MSVC)
  unsigned long idx;
  _BitScanReverse64(&idx, val);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (i32)idx;
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
  TracyCZoneEnd(__tracy_zone_ctx);
  return 63 - __builtin_clzll(val);
#else
  i32 pos = 63;
  u64 msk = 0x8000000000000000ULL;
  while (msk != 0ULL) {
    if ((val & msk) != 0ULL) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return pos;
    }
    msk >>= 1U;
    pos--;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
#endif
}

// =========================================================================
// Byte Swap
// =========================================================================

func u16 bswap_u16(u16 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
#if defined(COMPILER_MSVC)
  TracyCZoneEnd(__tracy_zone_ctx);
  return _byteswap_ushort(val);
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
  TracyCZoneEnd(__tracy_zone_ctx);
  return __builtin_bswap16(val);
#else
  TracyCZoneEnd(__tracy_zone_ctx);
  return (u16)((val >> 8U) | (val << 8U));
#endif
}

func u32 bswap_u32(u32 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
#if defined(COMPILER_MSVC)
  TracyCZoneEnd(__tracy_zone_ctx);
  return _byteswap_ulong(val);
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
  TracyCZoneEnd(__tracy_zone_ctx);
  return __builtin_bswap32(val);
#else
  TracyCZoneEnd(__tracy_zone_ctx);
  return ((val & 0xFF000000U) >> 24U) | ((val & 0x00FF0000U) >> 8U) |
         ((val & 0x0000FF00U) << 8U) | ((val & 0x000000FFU) << 24U);
#endif
}

func u64 bswap_u64(u64 val) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
#if defined(COMPILER_MSVC)
  TracyCZoneEnd(__tracy_zone_ctx);
  return _byteswap_uint64(val);
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || \
    defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
  TracyCZoneEnd(__tracy_zone_ctx);
  return __builtin_bswap64(val);
#else
  TracyCZoneEnd(__tracy_zone_ctx);
  return ((val & 0xFF00000000000000ULL) >> 56U) |
         ((val & 0x00FF000000000000ULL) >> 40U) |
         ((val & 0x0000FF0000000000ULL) >> 24U) |
         ((val & 0x000000FF00000000ULL) >> 8U) |
         ((val & 0x00000000FF000000ULL) << 8U) |
         ((val & 0x0000000000FF0000ULL) << 24U) |
         ((val & 0x000000000000FF00ULL) << 40U) |
         ((val & 0x00000000000000FFULL) << 56U);
#endif
}

// =========================================================================
// Bit Rotate
// =========================================================================

func u32 rotl_u32(u32 val, i32 cnt) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
#if defined(COMPILER_MSVC)
  TracyCZoneEnd(__tracy_zone_ctx);
  return _rotl(val, cnt);
#else
  u32 ucnt = (u32)cnt & 31U;
  TracyCZoneEnd(__tracy_zone_ctx);
  return (ucnt != 0U) ? ((val << ucnt) | (val >> (32U - ucnt))) : val;
#endif
}

func u64 rotl_u64(u64 val, i32 cnt) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
#if defined(COMPILER_MSVC)
  TracyCZoneEnd(__tracy_zone_ctx);
  return _rotl64(val, cnt);
#else
  u32 ucnt = (u32)cnt & 63U;
  TracyCZoneEnd(__tracy_zone_ctx);
  return (ucnt != 0U) ? ((val << ucnt) | (val >> (64U - ucnt))) : val;
#endif
}

func u32 rotr_u32(u32 val, i32 cnt) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
#if defined(COMPILER_MSVC)
  TracyCZoneEnd(__tracy_zone_ctx);
  return _rotr(val, cnt);
#else
  u32 ucnt = (u32)cnt & 31U;
  TracyCZoneEnd(__tracy_zone_ctx);
  return (ucnt != 0U) ? ((val >> ucnt) | (val << (32U - ucnt))) : val;
#endif
}

func u64 rotr_u64(u64 val, i32 cnt) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
#if defined(COMPILER_MSVC)
  TracyCZoneEnd(__tracy_zone_ctx);
  return _rotr64(val, cnt);
#else
  u32 ucnt = (u32)cnt & 63U;
  TracyCZoneEnd(__tracy_zone_ctx);
  return (ucnt != 0U) ? ((val >> ucnt) | (val << (64U - ucnt))) : val;
#endif
}
