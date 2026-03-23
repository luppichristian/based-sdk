// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(basic_crash_test, entry_runtime_installs_crash_handler) {
  EXPECT_TRUE(crash_is_installed() != 0);
}

TEST(basic_crash_test, crash_handler_install_roundtrip_restores_previous_state) {
  b32 was_installed = crash_is_installed();

  crash_uninstall();
  EXPECT_TRUE(crash_is_installed() == 0);

  EXPECT_TRUE(crash_install() != 0);
  EXPECT_TRUE(crash_is_installed() != 0);

  crash_uninstall();
  EXPECT_TRUE(crash_is_installed() == 0);

  if (was_installed) {
    EXPECT_TRUE(crash_install() != 0);
    EXPECT_TRUE(crash_is_installed() != 0);
  }
}
