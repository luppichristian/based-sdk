// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(input_capture_test, acquire_release_and_slot_epoch_behavior) {
  input_capture_release_all_keys();

  sz default_slot = input_capture_get_slot(INPUT_KEY_DEFAULT);
  EXPECT_EQ(0U, default_slot);
  EXPECT_TRUE(input_capture_acquire_key(INPUT_KEY_DEFAULT) != 0);

  input_key key_val = 42;
  ASSERT_TRUE(input_capture_acquire_key(key_val) != 0);
  sz key_slot = input_capture_get_slot(key_val);
  EXPECT_TRUE(key_slot < INPUT_CAPTURE_MAX_KEYS);

  u32 epoch_before = input_capture_get_slot_epoch(key_slot);
  EXPECT_TRUE(epoch_before != 0U);

  input_capture_release_key(key_val);
  sz released_slot = input_capture_get_slot(key_val);
  EXPECT_TRUE(released_slot < INPUT_CAPTURE_MAX_KEYS);
  u32 epoch_after = input_capture_get_slot_epoch(released_slot);
  EXPECT_TRUE(epoch_after != 0U);

  input_capture_release_key(INPUT_KEY_DEFAULT);
  EXPECT_EQ(0U, input_capture_get_slot(INPUT_KEY_DEFAULT));
}
