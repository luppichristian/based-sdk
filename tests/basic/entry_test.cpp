// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(basic_entry_test, get_cmdline_exposes_initialized_runtime_cmdline) {
  cmdline runtime_cmdl = entry_get_cmdline();

  EXPECT_NE(runtime_cmdl.args, nullptr);
  EXPECT_GT(runtime_cmdl.count, 0U);
  EXPECT_TRUE(cmdline_is_empty(runtime_cmdl) == 0);
  EXPECT_NE(cmdline_get_program(runtime_cmdl), nullptr);
}

TEST(basic_entry_test, nested_init_keeps_original_cmdline_and_balances_quit) {
  cmdline old_cmdl = entry_get_cmdline();
  c8 arg_one[] = "unit-test";
  c8* arg_vec[] = {arg_one};
  cmdline fake_cmdl = cmdline_build(1, arg_vec);

  b32 init_ok = entry_init(fake_cmdl);
  EXPECT_TRUE(init_ok != 0);

  cmdline new_cmdl = entry_get_cmdline();
  EXPECT_EQ(new_cmdl.args, old_cmdl.args);
  EXPECT_EQ(new_cmdl.count, old_cmdl.count);

  entry_quit();
}
