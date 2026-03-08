// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(input_sensor_test, id_conversion_and_invalid_lifecycle_calls) {
  sensor sen_id = sensor_from_native_id(123);
  EXPECT_TRUE(sensor_id_is_valid(sen_id) != 0);
  EXPECT_EQ((up)123, sensor_to_native_id(sen_id));

  EXPECT_TRUE(sensor_id_is_valid(nullptr) == 0);
  EXPECT_TRUE(sensor_open(nullptr) == 0);
  EXPECT_TRUE(sensor_start(nullptr) == 0);
  EXPECT_TRUE(sensor_stop(nullptr) == 0);
  EXPECT_TRUE(sensor_close(nullptr) == 0);

  buffer out_buf = {};
  EXPECT_TRUE(sensor_read(nullptr, &out_buf) == 0);
  EXPECT_TRUE(out_buf.ptr == nullptr);
  EXPECT_EQ(0U, out_buf.size);
}

TEST(input_sensor_test, enumeration_consistency_when_devices_exist) {
  sz count_val = sensor_get_count();
  sensor out_id = nullptr;
  if (count_val == 0) {
    EXPECT_TRUE(sensor_get_id(0, &out_id) == 0);
    return;
  }

  ASSERT_TRUE(sensor_get_id(0, &out_id) != 0);
  EXPECT_TRUE(sensor_id_is_valid(out_id) != 0);
  sensor_kind kind_val = sensor_get_kind(out_id);
  EXPECT_TRUE(kind_val >= SENSOR_KIND_INVALID);
}
