// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(input_bindings_test, add_remove_enable_and_query_binding_descriptors) {
  bindings_clear();

  EXPECT_TRUE(bindings_add(nullptr) == 0);

  binding_desc bad_desc = {};
  bad_desc.binding_id = 0;
  bad_desc.enabled = 1;
  EXPECT_TRUE(bindings_add(&bad_desc) == 0);

  binding_desc ok_desc = {};
  ok_desc.binding_id = 1001;
  ok_desc.enabled = 1;
  ok_desc.keyboard.count = 1;
  ok_desc.keyboard.scancodes[0] = 4;
  ok_desc.keyboard.required_mods = KEYMOD_NONE;
  ok_desc.keyboard.forbidden_mods = KEYMOD_NONE;

  ASSERT_TRUE(bindings_add(&ok_desc) != 0);
  EXPECT_TRUE(bindings_has(ok_desc.binding_id) != 0);
  EXPECT_TRUE(bindings_is_pressed(INPUT_KEY_DEFAULT, ok_desc.binding_id) == 0);
  EXPECT_TRUE(bindings_is_released(INPUT_KEY_DEFAULT, ok_desc.binding_id) == 0);

  EXPECT_TRUE(bindings_set_enabled(ok_desc.binding_id, 0) != 0);
  EXPECT_TRUE(bindings_is_down(ok_desc.binding_id) == 0);

  EXPECT_TRUE(bindings_remove(ok_desc.binding_id) != 0);
  EXPECT_TRUE(bindings_has(ok_desc.binding_id) == 0);
  EXPECT_TRUE(bindings_remove(ok_desc.binding_id) == 0);
}
