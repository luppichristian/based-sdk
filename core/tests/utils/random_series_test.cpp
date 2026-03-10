// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

#include <cmath>

namespace {

  random_series make_series(u32 seed) {
    random_series series = {};
    random_series_seed(&series, seed);
    return series;
  }

}  // namespace

TEST(utils_random_series_test, reset_restores_sequence) {
  random_series series = make_series(12345U);

  u32 first_u32 = random_series_u32(&series);
  i64 first_i64 = random_series_i64(&series);
  f32 first_f32 = random_series_f32(&series);
  f64 first_f64 = random_series_f64(&series);

  random_series_reset(&series);

  EXPECT_EQ(first_u32, random_series_u32(&series));
  EXPECT_EQ(first_i64, random_series_i64(&series));
  EXPECT_FLOAT_EQ(first_f32, random_series_f32(&series));
  EXPECT_DOUBLE_EQ(first_f64, random_series_f64(&series));
}

TEST(utils_random_series_test, seed_sets_seed_and_current) {
  random_series series = {};

  random_series_seed(&series, 77U);

  EXPECT_EQ(77U, series.seed);
  EXPECT_EQ(77U, series.current);
}

TEST(utils_random_series_test, integer_ranges_stay_in_bounds) {
  random_series series = make_series(77U);

  for (i32 idx = 0; idx < 256; ++idx) {
    u8 u8_value = random_series_rng_u8(&series, (u8)3, (u8)9);
    u16 u16_value = random_series_rng_u16(&series, (u16)100, (u16)500);
    u32 u32_value = random_series_rng_u32(&series, 5U, 17U);
    u64 u64_value = random_series_rng_u64(&series, 900ULL, 1200ULL);
    i8 i8_value = random_series_rng_i8(&series, (i8)-9, (i8)7);
    i16 i16_value = random_series_rng_i16(&series, (i16)-25, (i16)25);
    i32 i32_value = random_series_rng_i32(&series, -500, 250);
    i64 i64_value = random_series_rng_i64(&series, -9000LL, 12000LL);

    EXPECT_GE(u8_value, (u8)3);
    EXPECT_LE(u8_value, (u8)9);
    EXPECT_GE(u16_value, (u16)100);
    EXPECT_LE(u16_value, (u16)500);
    EXPECT_GE(u32_value, 5U);
    EXPECT_LE(u32_value, 17U);
    EXPECT_GE(u64_value, 900ULL);
    EXPECT_LE(u64_value, 1200ULL);
    EXPECT_GE(i8_value, (i8)-9);
    EXPECT_LE(i8_value, (i8)7);
    EXPECT_GE(i16_value, (i16)-25);
    EXPECT_LE(i16_value, (i16)25);
    EXPECT_GE(i32_value, -500);
    EXPECT_LE(i32_value, 250);
    EXPECT_GE(i64_value, -9000LL);
    EXPECT_LE(i64_value, 12000LL);
  }
}

TEST(utils_random_series_test, swapped_integer_ranges_are_supported) {
  random_series series = make_series(991U);

  for (i32 idx = 0; idx < 256; ++idx) {
    u32 u32_value = random_series_rng_u32(&series, 20U, 7U);
    i64 i64_value = random_series_rng_i64(&series, 15LL, -15LL);

    EXPECT_GE(u32_value, 7U);
    EXPECT_LE(u32_value, 20U);
    EXPECT_GE(i64_value, -15LL);
    EXPECT_LE(i64_value, 15LL);
  }
}

TEST(utils_random_series_test, chance_respects_extremes) {
  random_series series = make_series(44U);

  for (i32 idx = 0; idx < 64; ++idx) {
    EXPECT_EQ((b32)1, random_series_chance(&series, 0));
    EXPECT_EQ((b32)1, random_series_chance(&series, 1));
    EXPECT_EQ((b32)0, random_series_chance_f32(&series, 0.0f));
    EXPECT_EQ((b32)1, random_series_chance_f32(&series, 1.0f));
    EXPECT_EQ((b32)0, random_series_chance_f64(&series, 0.0));
    EXPECT_EQ((b32)1, random_series_chance_f64(&series, 1.0));
  }
}

TEST(utils_random_series_test, floating_helpers_stay_in_domain) {
  random_series series = make_series(2026U);

  for (i32 idx = 0; idx < 512; ++idx) {
    f32 unilateral32 = random_series_u_f32(&series);
    f32 bilateral32 = random_series_b_f32(&series);
    f64 unilateral64 = random_series_u_f64(&series);
    f64 bilateral64 = random_series_b_f64(&series);
    f32 ranged32 = random_series_rng_f32(&series, -4.0f, 6.0f);
    f64 ranged64 = random_series_rng_f64(&series, -8.0, 10.0);
    f32 random32 = random_series_f32(&series);
    f64 random64 = random_series_f64(&series);

    EXPECT_GE(unilateral32, 0.0f);
    EXPECT_LT(unilateral32, 1.0f);
    EXPECT_GE(bilateral32, -1.0f);
    EXPECT_LT(bilateral32, 1.0f);
    EXPECT_GE(unilateral64, 0.0);
    EXPECT_LT(unilateral64, 1.0);
    EXPECT_GE(bilateral64, -1.0);
    EXPECT_LT(bilateral64, 1.0);
    EXPECT_GE(ranged32, -4.0f);
    EXPECT_LT(ranged32, 6.0f);
    EXPECT_GE(ranged64, -8.0);
    EXPECT_LT(ranged64, 10.0);
    EXPECT_TRUE(std::isfinite(random32));
    EXPECT_TRUE(std::isfinite(random64));
  }
}
