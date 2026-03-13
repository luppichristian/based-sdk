// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(input_tablet_test, invalid_queries_are_rejected_safely) {
  tablet_pen_state out_state = {};
  EXPECT_TRUE(tablet_get_last_pen_state(&out_state) == 0);

  device bad_id = nullptr;
  u8 report_buf[64] = {0};
  sz out_size = 0;
  EXPECT_TRUE(tablet_read_hid_report(bad_id, report_buf, size_of(report_buf), &out_size, 1) == 0);
  EXPECT_EQ(0U, out_size);
}

TEST(input_tablet_test, count_and_device_consistency) {
  sz count_val = tablet_get_total_count();
  EXPECT_TRUE((tablet_is_available() != 0) == (count_val > 0));

  device out_id = nullptr;
  if (count_val == 0) {
    EXPECT_EQ(nullptr, tablet_get_device(0));
  } else {
    out_id = tablet_get_device(0);
    EXPECT_NE(nullptr, out_id);
    EXPECT_EQ(DEVICE_TYPE_TABLET, devices_get_type(out_id));
    EXPECT_TRUE(device_is_valid(out_id) != 0);
  }
}
