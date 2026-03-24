// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(input_tablet_test, invalid_queries_are_rejected_safely) {
  EXPECT_TRUE(tablet_is_valid(NULL) == 0);
}

TEST(input_tablet_test, count_and_device_consistency) {
  sz count_val = tablet_get_total_count();
  EXPECT_TRUE((tablet_is_available() != 0) == (count_val > 0));

  tablet out_id = NULL;
  if (count_val == 0) {
    EXPECT_EQ(nullptr, tablet_get_from_idx(0));
  } else {
    out_id = tablet_get_from_idx(0);
    EXPECT_NE(nullptr, out_id);
    EXPECT_EQ(DEVICE_TYPE_TABLET, devices_get_type(tablet_to_device(out_id)));
    EXPECT_TRUE(device_is_valid(tablet_to_device(out_id)) != 0);
  }
}
