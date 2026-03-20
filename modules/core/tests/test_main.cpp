// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

func b32 run(cmdline cmdl) {
  assert_set_mode(ASSERT_MODE_LOG);
  thread_log_set_level(LOG_LEVEL_FATAL);
  log_state_set_level(global_get_log_state(), LOG_LEVEL_FATAL);
  int argc = cmdl.count;
  char** argv = cmdl.args;
  testing::InitGoogleTest(&argc, argv);
  int test_result = RUN_ALL_TESTS();
  return test_result == 0;
}
