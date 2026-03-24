// MIT License
// Copyright (c) 2026 Christian Luppi

#include "internal.h"
#include "test_common.hpp"

TEST(input_camera_test, id_conversion_and_invalid_lifecycle_calls) {
  camera cam_id = camera_from_native_id(123);
  EXPECT_TRUE(camera_is_valid(cam_id) != 0);
  EXPECT_EQ((up)123, camera_to_native_id(cam_id));

  EXPECT_TRUE(camera_is_valid(NULL) == 0);
  EXPECT_TRUE(camera_open(NULL) == 0);
  EXPECT_TRUE(camera_start(NULL) == 0);
  EXPECT_TRUE(camera_stop(NULL) == 0);
  EXPECT_TRUE(camera_close(NULL) == 0);

  buffer frame_buf = {};
  EXPECT_TRUE(camera_read(NULL, &frame_buf) == 0);
  EXPECT_TRUE(frame_buf.ptr == NULL);
  EXPECT_EQ(0U, frame_buf.size);
}

TEST(input_camera_test, enumeration_consistency_when_devices_exist) {
  sz count_val = camera_get_total_count();
  camera out_id = count_val == 0 ? NULL : camera_get_from_idx(0);
  if (count_val == 0) {
    return;
  }

  ASSERT_TRUE(out_id != NULL);
  EXPECT_TRUE(camera_is_valid(out_id) != 0);
  camera_pos pos_val = camera_get_pos(out_id);
  EXPECT_TRUE(
      pos_val == CAMERA_POS_UNKNOWN || pos_val == CAMERA_POS_FRONT_FACING ||
      pos_val == CAMERA_POS_BACK_FACING);
}
