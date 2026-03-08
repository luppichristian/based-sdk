// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(input_tablet_test, invalid_queries_are_rejected_safely) {
  tablet_pen_state out_state = {};
  EXPECT_TRUE(tablet_get_last_pen_state(&out_state) == 0);

  device_id bad_id = {};
  u8 report_buf[64] = {0};
  sz out_size = 0;
  EXPECT_TRUE(tablet_read_hid_report(bad_id, report_buf, size_of(report_buf), &out_size, 1) == 0);
  EXPECT_EQ(0U, out_size);
}

TEST(input_tablet_test, count_and_device_id_consistency) {
  sz count_val = tablet_get_count();
  EXPECT_TRUE((tablet_is_available() != 0) == (count_val > 0));

  device_id out_id = {};
  if (count_val == 0) {
    EXPECT_TRUE(tablet_get_device_id(0, &out_id) == 0);
  } else {
    EXPECT_TRUE(tablet_get_device_id(0, &out_id) != 0);
    EXPECT_EQ(DEVICE_TYPE_TABLET, out_id.type);
    EXPECT_TRUE(device_id_is_valid(out_id) != 0);
  }
}
