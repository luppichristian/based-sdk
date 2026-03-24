// MIT License
// Copyright (c) 2026 Christian Luppi

#include "internal.h"
#include "test_common.hpp"

TEST(input_sensor_test, id_conversion_and_invalid_lifecycle_calls) {
  sensor sen_id = sensor_from_native_id(123);
  EXPECT_TRUE(sensor_is_valid(sen_id) != 0);
  EXPECT_EQ((up)123, sensor_to_native_id(sen_id));

  EXPECT_TRUE(sensor_is_valid(NULL) == 0);
  EXPECT_TRUE(sensor_open(NULL) == 0);
  EXPECT_TRUE(sensor_start(NULL) == 0);
  EXPECT_TRUE(sensor_stop(NULL) == 0);
  EXPECT_TRUE(sensor_close(NULL) == 0);

  buffer out_buf = {};
  EXPECT_TRUE(sensor_read(NULL, &out_buf) == 0);
  EXPECT_TRUE(out_buf.ptr == NULL);
  EXPECT_EQ(0U, out_buf.size);
}

TEST(input_sensor_test, enumeration_consistency_when_devices_exist) {
  sz count_val = sensor_get_total_count();
  sensor out_id = count_val == 0 ? NULL : sensor_get_from_idx(0);
  if (count_val == 0) {
    return;
  }

  ASSERT_TRUE(out_id != NULL);
  EXPECT_TRUE(sensor_is_valid(out_id) != 0);
  sensor_kind kind_val = sensor_get_kind(out_id);
  EXPECT_TRUE(kind_val >= SENSOR_KIND_INVALID);
}
