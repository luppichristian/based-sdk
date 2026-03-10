// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// =========================================================================
c_begin;
// =========================================================================

typedef struct random_series {
  u32 current;
  u32 seed;
} random_series;

// Set seed and current value within the series
func void random_series_seed(random_series* series, u32 seed);

// Reset internal current value to seed value
func void random_series_reset(random_series* series);

// Returns true if a predefined chance is met.
// The chance is described as a 0.0 to 1.0 value, 0.0 never happens, 1.0 always happens.
func b32 random_series_chance_f64(random_series* series, f64 chance);
func b32 random_series_chance_f32(random_series* series, f32 chance);

// Returns true if a predefined chance is met.
// The chance is described as 1 in chance.
// For example if chance is 100 the chance will 1 / 100
// If 0 is specified, true will always be returned.
func b32 random_series_chance(random_series* series, u32x chance);

// Getters for randomly generated primitive types.
func u8 random_series_u8(random_series* series);
func u16 random_series_u16(random_series* series);
func u32 random_series_u32(random_series* series);
func u64 random_series_u64(random_series* series);
func i8 random_series_i8(random_series* series);
func i16 random_series_i16(random_series* series);
func i32 random_series_i32(random_series* series);
func i64 random_series_i64(random_series* series);
func f32 random_series_f32(random_series* series);
func f64 random_series_f64(random_series* series);

// Getters for randomly generated primitive types within ranges.
func u8 random_series_rng_u8(random_series* series, u8 min_value, u8 max_value);
func u16 random_series_rng_u16(random_series* series, u16 min_value, u16 max_value);
func u32 random_series_rng_u32(random_series* series, u32 min_value, u32 max_value);
func u64 random_series_rng_u64(random_series* series, u64 min_value, u64 max_value);
func i8 random_series_rng_i8(random_series* series, i8 min_value, i8 max_value);
func i16 random_series_rng_i16(random_series* series, i16 min_value, i16 max_value);
func i32 random_series_rng_i32(random_series* series, i32 min_value, i32 max_value);
func i64 random_series_rng_i64(random_series* series, i64 min_value, i64 max_value);
func f32 random_series_rng_f32(random_series* series, f32 min_value, f32 max_value);
func f64 random_series_rng_f64(random_series* series, f64 min_value, f64 max_value);

// Floating point unilateral getters.
// Bilaterals are basically values between 0.0 to 1.0
func f32 random_series_u_f32(random_series* series);
func f64 random_series_u_f64(random_series* series);

// Floating point bilateral getters.
// Bilaterals are basically values between -1.0 to 1.0
func f32 random_series_b_f32(random_series* series);
func f64 random_series_b_f64(random_series* series);

// =========================================================================
c_end;
// =========================================================================
