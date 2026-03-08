// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(filesystem_pathwatch_test, invalid_arguments_are_rejected) {
  path watch_path = path_from_cstr(".");
  path out_path = path_from_cstr("");
  EXPECT_TRUE(pathwatch_start(nullptr) == 0);
  EXPECT_TRUE(pathwatch_stop(nullptr) == 0);
  EXPECT_TRUE(pathwatch_pause(nullptr) == 0);
  EXPECT_TRUE(pathwatch_resume(nullptr) == 0);
  EXPECT_TRUE(pathwatch_add(nullptr, &watch_path, 1) == 0);
  EXPECT_TRUE(pathwatch_remove(nullptr, 1) == 0);
  EXPECT_TRUE(pathwatch_remove_path(nullptr, &watch_path) == 0);
  EXPECT_TRUE(pathwatch_get_path(0, &out_path) == 0);
  EXPECT_TRUE(pathwatch_follow_symlinks(nullptr, 1) == 0);
  EXPECT_TRUE(pathwatch_allow_out_of_scope_links(nullptr, 1) == 0);
}

TEST(filesystem_pathwatch_test, lifecycle_controls_work_when_watcher_is_created) {
  pathwatch watcher = pathwatch_create(1);
  if (watcher.native_handle == nullptr) {
    GTEST_SKIP() << "pathwatch backend not available";
  }

  EXPECT_TRUE(pathwatch_start(&watcher) != 0);
  EXPECT_TRUE(pathwatch_pause(&watcher) != 0);
  EXPECT_TRUE(pathwatch_resume(&watcher) != 0);
  EXPECT_TRUE(pathwatch_follow_symlinks(&watcher, 1) != 0);
  EXPECT_TRUE(pathwatch_allow_out_of_scope_links(&watcher, 1) != 0);
  EXPECT_TRUE(pathwatch_stop(&watcher) != 0);

  i32 drained = pathwatch_drain();
  EXPECT_GE(drained, 0);
  (void)pathwatch_get_last_error();

  pathwatch_destroy(&watcher);
  EXPECT_EQ(nullptr, watcher.native_handle);
}
