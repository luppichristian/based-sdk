// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(utils_timer_test, clear) {
  f32 timer = 10.0F;
  timer_clear(&timer);
  EXPECT_FLOAT_EQ(0.0F, timer);
}

TEST(utils_timer_test, bump) {
  f32 timer = 0.0F;
  timer_bump(&timer);
  EXPECT_GE(timer, 100000.0F);
}

TEST(utils_timer_test, increment) {
  f32 timer = 0.0F;
  timer_increment(&timer, 1.0F, 2.0F);
  EXPECT_FLOAT_EQ(2.0F, timer);
}

TEST(utils_timer_test, increment_zero) {
  f32 timer = 5.0F;
  timer_increment(&timer, 0.0F, 1.0F);
  EXPECT_FLOAT_EQ(5.0F, timer);
}

TEST(utils_timer_test, consume_true) {
  f32 timer = 5.0F;
  b32 result = timer_consume(&timer, 3.0F);
  EXPECT_TRUE(result != 0);
  EXPECT_FLOAT_EQ(2.0F, timer);
}

TEST(utils_timer_test, consume_false) {
  f32 timer = 2.0F;
  b32 result = timer_consume(&timer, 3.0F);
  EXPECT_FALSE(result != 0);
  EXPECT_FLOAT_EQ(2.0F, timer);
}

TEST(utils_timer_test, consume_once_true) {
  f32 timer = 5.0F;
  b32 result = timer_consume_once(&timer, 3.0F);
  EXPECT_TRUE(result != 0);
  EXPECT_FLOAT_EQ(0.0F, timer);
}

TEST(utils_timer_test, consume_once_false) {
  f32 timer = 2.0F;
  b32 result = timer_consume_once(&timer, 3.0F);
  EXPECT_FALSE(result != 0);
  EXPECT_FLOAT_EQ(2.0F, timer);
}
