// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

int main(int argc, char** argv) {
  cmdline cmdl = cmdline_build(argc, argv);
  if (!entry_init(cmdl)) {
    return 1;
  }

  ::testing::InitGoogleTest(&argc, argv);
  int test_result = RUN_ALL_TESTS();
  return test_result;
}
