// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/random_series.h"

#include "basic/assert.h"
#include "basic/profiler.h"
#include "context/thread_ctx.h"

func u32 random_series_next_u32(random_series* series) {
  series->current = series->current * 1664525U + 1013904223U;
  return series->current;
}

func u64 random_series_next_u64(random_series* series) {
  u64 upper = (u64)random_series_next_u32(series);
  u64 lower = (u64)random_series_next_u32(series);
  return (upper << 32U) | lower;
}

func void random_series_swap_u32(u32* lhs, u32* rhs) {
  u32 temp = *lhs;
  *lhs = *rhs;
  *rhs = temp;
}

func void random_series_swap_u64(u64* lhs, u64* rhs) {
  u64 temp = *lhs;
  *lhs = *rhs;
  *rhs = temp;
}

func f32 random_series_unilateral_f32(random_series* series) {
  return (f32)random_series_next_u32(series) / ((f32)U32_MAX + 1.0f);
}

func f64 random_series_unilateral_f64(random_series* series) {
  u64 value = random_series_next_u64(series) >> 11U;
  return (f64)value / 9007199254740992.0;
}

func u32 random_series_bounded_u32(random_series* series, u32 bound) {
  if (bound == 0U) {
    return random_series_next_u32(series);
  }

  u32 threshold = (u32)(0U - bound) % bound;
  for (;;) {
    u32 value = random_series_next_u32(series);
    if (value >= threshold) {
      return value % bound;
    }
  }
}

func u64 random_series_bounded_u64(random_series* series, u64 bound) {
  if (bound == 0U) {
    return random_series_next_u64(series);
  }

  u64 threshold = (u64)(0ULL - bound) % bound;
  for (;;) {
    u64 value = random_series_next_u64(series);
    if (value >= threshold) {
      return value % bound;
    }
  }
}

func b32 random_series_validate(random_series* series, cstr8 func_name) {
  if (series == NULL) {
    thread_log_warn("%s: Series is null", func_name);
    return false;
  }

  assert(series != NULL);
  return true;
}

func void random_series_seed(random_series* series, u32 seed) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_seed")) {
    profile_func_end;
    return;
  }

  series->seed = seed;
  series->current = seed;
  profile_func_end;
}

func void random_series_reset(random_series* series) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_reset")) {
    profile_func_end;
    return;
  }

  series->current = series->seed;
  profile_func_end;
}

func b32 random_series_chance_f64(random_series* series, f64 chance) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_chance_f64")) {
    profile_func_end;
    return false;
  }

  if (chance <= 0.0) {
    profile_func_end;
    return false;
  }
  if (chance >= 1.0) {
    profile_func_end;
    return true;
  }

  b32 result = random_series_unilateral_f64(series) < chance;
  profile_func_end;
  return result;
}

func b32 random_series_chance_f32(random_series* series, f32 chance) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_chance_f32")) {
    profile_func_end;
    return false;
  }

  if (chance <= 0.0f) {
    profile_func_end;
    return false;
  }
  if (chance >= 1.0f) {
    profile_func_end;
    return true;
  }

  b32 result = random_series_unilateral_f32(series) < chance;
  profile_func_end;
  return result;
}

func b32 random_series_chance(random_series* series, u32x chance) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_chance")) {
    profile_func_end;
    return false;
  }

  if (chance == 0) {
    profile_func_end;
    return true;
  }

  b32 result = random_series_bounded_u64(series, (u64)chance) == 0ULL;
  profile_func_end;
  return result;
}

func u8 random_series_u8(random_series* series) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_u8")) {
    profile_func_end;
    return 0;
  }

  u8 value = (u8)random_series_next_u32(series);
  profile_func_end;
  return value;
}

func u16 random_series_u16(random_series* series) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_u16")) {
    profile_func_end;
    return 0;
  }

  u16 value = (u16)random_series_next_u32(series);
  profile_func_end;
  return value;
}

func u32 random_series_u32(random_series* series) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_u32")) {
    profile_func_end;
    return 0;
  }

  u32 value = random_series_next_u32(series);
  profile_func_end;
  return value;
}

func u64 random_series_u64(random_series* series) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_u64")) {
    profile_func_end;
    return 0;
  }

  u64 value = random_series_next_u64(series);
  profile_func_end;
  return value;
}

func i8 random_series_i8(random_series* series) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_i8")) {
    profile_func_end;
    return 0;
  }

  i8 value = (i8)random_series_next_u32(series);
  profile_func_end;
  return value;
}

func i16 random_series_i16(random_series* series) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_i16")) {
    profile_func_end;
    return 0;
  }

  i16 value = (i16)random_series_next_u32(series);
  profile_func_end;
  return value;
}

func i32 random_series_i32(random_series* series) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_i32")) {
    profile_func_end;
    return 0;
  }

  i32 value = (i32)random_series_next_u32(series);
  profile_func_end;
  return value;
}

func i64 random_series_i64(random_series* series) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_i64")) {
    profile_func_end;
    return 0;
  }

  i64 value = (i64)random_series_next_u64(series);
  profile_func_end;
  return value;
}

func f32 random_series_f32(random_series* series) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_f32")) {
    profile_func_end;
    return 0.0f;
  }

  f32 value = ((f32)random_series_next_u32(series) / (f32)U32_MAX) * 2.0f - 1.0f;
  value *= F32_MAX;
  profile_func_end;
  return value;
}

func f64 random_series_f64(random_series* series) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_f64")) {
    profile_func_end;
    return 0.0;
  }

  f64 value = random_series_unilateral_f64(series) * 2.0 - 1.0;
  value *= F64_MAX;
  profile_func_end;
  return value;
}

func u8 random_series_rng_u8(random_series* series, u8 min_value, u8 max_value) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_rng_u8")) {
    profile_func_end;
    return 0;
  }

  u32 min_u32 = min_value;
  u32 max_u32 = max_value;
  if (min_u32 > max_u32) {
    thread_log_warn("random_series_rng_u8: Swapping invalid range %u..%u", min_u32, max_u32);
    random_series_swap_u32(&min_u32, &max_u32);
  }

  u32 span = max_u32 - min_u32;
  u8 value = (u8)(min_u32 + random_series_bounded_u32(series, span + 1U));
  profile_func_end;
  return value;
}

func u16 random_series_rng_u16(random_series* series, u16 min_value, u16 max_value) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_rng_u16")) {
    profile_func_end;
    return 0;
  }

  u32 min_u32 = min_value;
  u32 max_u32 = max_value;
  if (min_u32 > max_u32) {
    thread_log_warn("random_series_rng_u16: Swapping invalid range %u..%u", min_u32, max_u32);
    random_series_swap_u32(&min_u32, &max_u32);
  }

  u32 span = max_u32 - min_u32;
  u16 value = (u16)(min_u32 + random_series_bounded_u32(series, span + 1U));
  profile_func_end;
  return value;
}

func u32 random_series_rng_u32(random_series* series, u32 min_value, u32 max_value) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_rng_u32")) {
    profile_func_end;
    return 0;
  }

  if (min_value > max_value) {
    thread_log_warn("random_series_rng_u32: Swapping invalid range %u..%u", min_value, max_value);
    random_series_swap_u32(&min_value, &max_value);
  }

  u32 span = max_value - min_value;
  u32 value = span == U32_MAX ? random_series_next_u32(series)
                              : min_value + random_series_bounded_u32(series, span + 1U);
  profile_func_end;
  return value;
}

func u64 random_series_rng_u64(random_series* series, u64 min_value, u64 max_value) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_rng_u64")) {
    profile_func_end;
    return 0;
  }

  if (min_value > max_value) {
    thread_log_warn("random_series_rng_u64: Swapping invalid range %llu..%llu", min_value, max_value);
    random_series_swap_u64(&min_value, &max_value);
  }

  u64 span = max_value - min_value;
  u64 value = span == U64_MAX ? random_series_next_u64(series)
                              : min_value + random_series_bounded_u64(series, span + 1ULL);
  profile_func_end;
  return value;
}

func i8 random_series_rng_i8(random_series* series, i8 min_value, i8 max_value) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_rng_i8")) {
    profile_func_end;
    return 0;
  }

  u8 min_mapped = (u8)min_value ^ 0x80U;
  u8 max_mapped = (u8)max_value ^ 0x80U;
  u8 value = (u8)(random_series_rng_u8(series, min_mapped, max_mapped) ^ 0x80U);
  profile_func_end;
  return (i8)value;
}

func i16 random_series_rng_i16(random_series* series, i16 min_value, i16 max_value) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_rng_i16")) {
    profile_func_end;
    return 0;
  }

  u16 min_mapped = (u16)min_value ^ 0x8000U;
  u16 max_mapped = (u16)max_value ^ 0x8000U;
  u16 value = (u16)(random_series_rng_u16(series, min_mapped, max_mapped) ^ 0x8000U);
  profile_func_end;
  return (i16)value;
}

func i32 random_series_rng_i32(random_series* series, i32 min_value, i32 max_value) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_rng_i32")) {
    profile_func_end;
    return 0;
  }

  u32 min_mapped = (u32)min_value ^ 0x80000000U;
  u32 max_mapped = (u32)max_value ^ 0x80000000U;
  u32 value = random_series_rng_u32(series, min_mapped, max_mapped) ^ 0x80000000U;
  profile_func_end;
  return (i32)value;
}

func i64 random_series_rng_i64(random_series* series, i64 min_value, i64 max_value) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_rng_i64")) {
    profile_func_end;
    return 0;
  }

  u64 min_mapped = (u64)min_value ^ 0x8000000000000000ULL;
  u64 max_mapped = (u64)max_value ^ 0x8000000000000000ULL;
  u64 value = random_series_rng_u64(series, min_mapped, max_mapped) ^ 0x8000000000000000ULL;
  profile_func_end;
  return (i64)value;
}

func f32 random_series_rng_f32(random_series* series, f32 min_value, f32 max_value) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_rng_f32")) {
    profile_func_end;
    return 0.0f;
  }

  if (min_value > max_value) {
    thread_log_warn("random_series_rng_f32: Swapping invalid range");
    f32 temp = min_value;
    min_value = max_value;
    max_value = temp;
  }

  f32 value = min_value + random_series_unilateral_f32(series) * (max_value - min_value);
  profile_func_end;
  return value;
}

func f64 random_series_rng_f64(random_series* series, f64 min_value, f64 max_value) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_rng_f64")) {
    profile_func_end;
    return 0.0;
  }

  if (min_value > max_value) {
    thread_log_warn("random_series_rng_f64: Swapping invalid range");
    f64 temp = min_value;
    min_value = max_value;
    max_value = temp;
  }

  f64 value = min_value + random_series_unilateral_f64(series) * (max_value - min_value);
  profile_func_end;
  return value;
}

func f32 random_series_u_f32(random_series* series) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_u_f32")) {
    profile_func_end;
    return 0.0f;
  }

  f32 value = random_series_unilateral_f32(series);
  profile_func_end;
  return value;
}

func f64 random_series_u_f64(random_series* series) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_u_f64")) {
    profile_func_end;
    return 0.0;
  }

  f64 value = random_series_unilateral_f64(series);
  profile_func_end;
  return value;
}

func f32 random_series_b_f32(random_series* series) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_b_f32")) {
    profile_func_end;
    return 0.0f;
  }

  f32 value = random_series_unilateral_f32(series) * 2.0f - 1.0f;
  profile_func_end;
  return value;
}

func f64 random_series_b_f64(random_series* series) {
  profile_func_begin;
  if (!random_series_validate(series, "random_series_b_f64")) {
    profile_func_end;
    return 0.0;
  }

  f64 value = random_series_unilateral_f64(series) * 2.0 - 1.0;
  profile_func_end;
  return value;
}
