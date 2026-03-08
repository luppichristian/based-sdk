// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(input_camera_test, id_conversion_and_invalid_lifecycle_calls) {
  camera cam_id = camera_from_native_id(123);
  EXPECT_TRUE(camera_id_is_valid(cam_id) != 0);
  EXPECT_EQ((up)123, camera_to_native_id(cam_id));

  EXPECT_TRUE(camera_id_is_valid(nullptr) == 0);
  EXPECT_TRUE(camera_open(nullptr) == 0);
  EXPECT_TRUE(camera_start(nullptr) == 0);
  EXPECT_TRUE(camera_stop(nullptr) == 0);
  EXPECT_TRUE(camera_close(nullptr) == 0);

  buffer frame_buf = {};
  EXPECT_TRUE(camera_read(nullptr, &frame_buf) == 0);
  EXPECT_TRUE(frame_buf.ptr == nullptr);
  EXPECT_EQ(0U, frame_buf.size);
}

TEST(input_camera_test, enumeration_consistency_when_devices_exist) {
  sz count_val = camera_get_count();
  camera out_id = nullptr;
  if (count_val == 0) {
    EXPECT_TRUE(camera_get_id(0, &out_id) == 0);
    return;
  }

  ASSERT_TRUE(camera_get_id(0, &out_id) != 0);
  EXPECT_TRUE(camera_id_is_valid(out_id) != 0);
  camera_position pos_val = camera_get_position(out_id);
  EXPECT_TRUE(
      pos_val == CAMERA_POSITION_UNKNOWN || pos_val == CAMERA_POSITION_FRONT_FACING ||
      pos_val == CAMERA_POSITION_BACK_FACING);
}
