// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(utils_timestamp_test, zero) {
  timestamp value = timestamp_zero();
  EXPECT_TRUE(timestamp_is_zero(value) != 0);
  EXPECT_EQ(0LL, timestamp_as_microseconds(value));
}

TEST(utils_timestamp_test, now) {
  timestamp value = timestamp_now();
  EXPECT_FALSE(timestamp_is_zero(value) != 0);
}

TEST(utils_timestamp_test, from_microseconds) {
  timestamp value = timestamp_from_microseconds(1000000LL);
  EXPECT_EQ(1000000LL, timestamp_as_microseconds(value));
}

TEST(utils_timestamp_test, from_milliseconds) {
  timestamp value = timestamp_from_milliseconds(1000LL);
  EXPECT_EQ(1000000LL, timestamp_as_microseconds(value));
}

TEST(utils_timestamp_test, from_seconds) {
  timestamp value = timestamp_from_seconds(1.0);
  EXPECT_EQ(1000000LL, timestamp_as_microseconds(value));
}

TEST(utils_timestamp_test, from_minutes) {
  timestamp value = timestamp_from_minutes(1.0);
  EXPECT_EQ(60000000LL, timestamp_as_microseconds(value));
}

TEST(utils_timestamp_test, from_hours) {
  timestamp value = timestamp_from_hours(1.0);
  EXPECT_EQ(3600000000LL, timestamp_as_microseconds(value));
}

TEST(utils_timestamp_test, as_milliseconds) {
  timestamp value = timestamp_from_microseconds(1500000LL);
  EXPECT_DOUBLE_EQ(1500.0, timestamp_as_milliseconds(value));
}

TEST(utils_timestamp_test, as_seconds) {
  timestamp value = timestamp_from_microseconds(1500000LL);
  EXPECT_DOUBLE_EQ(1.5, timestamp_as_seconds(value));
}

TEST(utils_timestamp_test, as_minutes) {
  timestamp value = timestamp_from_seconds(60.0);
  EXPECT_DOUBLE_EQ(1.0, timestamp_as_minutes(value));
}

TEST(utils_timestamp_test, as_hours) {
  timestamp value = timestamp_from_seconds(3600.0);
  EXPECT_DOUBLE_EQ(1.0, timestamp_as_hours(value));
}

TEST(utils_timestamp_test, add) {
  timestamp lhs = timestamp_from_milliseconds(500LL);
  timestamp rhs = timestamp_from_milliseconds(700LL);
  timestamp result = timestamp_add(lhs, rhs);
  EXPECT_EQ(1200000LL, timestamp_as_microseconds(result));
}

TEST(utils_timestamp_test, sub) {
  timestamp lhs = timestamp_from_milliseconds(700LL);
  timestamp rhs = timestamp_from_milliseconds(200LL);
  timestamp result = timestamp_sub(lhs, rhs);
  EXPECT_EQ(500000LL, timestamp_as_microseconds(result));
}

TEST(utils_timestamp_test, scale) {
  timestamp value = timestamp_from_milliseconds(1000LL);
  timestamp result = timestamp_scale(value, 2.0);
  EXPECT_EQ(2000000LL, timestamp_as_microseconds(result));
}

TEST(utils_timestamp_test, abs) {
  timestamp value = timestamp_from_milliseconds(-500LL);
  timestamp result = timestamp_abs(value);
  EXPECT_EQ(500000LL, timestamp_as_microseconds(result));
}

TEST(utils_timestamp_test, abs_already_positive) {
  timestamp value = timestamp_from_milliseconds(500LL);
  timestamp result = timestamp_abs(value);
  EXPECT_EQ(500000LL, timestamp_as_microseconds(result));
}

TEST(utils_timestamp_test, clamp) {
  timestamp value = timestamp_from_milliseconds(50LL);
  timestamp min_val = timestamp_from_milliseconds(10LL);
  timestamp max_val = timestamp_from_milliseconds(100LL);
  timestamp result = timestamp_clamp(value, min_val, max_val);
  EXPECT_EQ(50000LL, timestamp_as_microseconds(result));
}

TEST(utils_timestamp_test, clamp_below_min) {
  timestamp value = timestamp_from_milliseconds(5LL);
  timestamp min_val = timestamp_from_milliseconds(10LL);
  timestamp max_val = timestamp_from_milliseconds(100LL);
  timestamp result = timestamp_clamp(value, min_val, max_val);
  EXPECT_EQ(10000LL, timestamp_as_microseconds(result));
}

TEST(utils_timestamp_test, clamp_above_max) {
  timestamp value = timestamp_from_milliseconds(200LL);
  timestamp min_val = timestamp_from_milliseconds(10LL);
  timestamp max_val = timestamp_from_milliseconds(100LL);
  timestamp result = timestamp_clamp(value, min_val, max_val);
  EXPECT_EQ(100000LL, timestamp_as_microseconds(result));
}

TEST(utils_timestamp_test, cmp_equal) {
  timestamp lhs = timestamp_from_milliseconds(100LL);
  timestamp rhs = timestamp_from_milliseconds(100LL);
  EXPECT_EQ(0, timestamp_cmp(lhs, rhs));
}

TEST(utils_timestamp_test, cmp_less) {
  timestamp lhs = timestamp_from_milliseconds(50LL);
  timestamp rhs = timestamp_from_milliseconds(100LL);
  EXPECT_LT(timestamp_cmp(lhs, rhs), 0);
}

TEST(utils_timestamp_test, cmp_greater) {
  timestamp lhs = timestamp_from_milliseconds(100LL);
  timestamp rhs = timestamp_from_milliseconds(50LL);
  EXPECT_GT(timestamp_cmp(lhs, rhs), 0);
}
