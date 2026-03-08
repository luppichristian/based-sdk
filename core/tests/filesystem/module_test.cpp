// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(filesystem_module_test, extension_is_available_and_invalid_open_fails) {
  cstr8 ext_ptr = mod_get_extension();
  EXPECT_NE(nullptr, ext_ptr);
  EXPECT_TRUE(ext_ptr[0] != '\0');

  path empty_path = path_from_cstr("");
  mod mod_val = mod_open(&empty_path);
  EXPECT_TRUE(mod_is_open(&mod_val) == 0);

  EXPECT_EQ(nullptr, mod_get_func(&mod_val, "anything"));
  EXPECT_EQ(nullptr, mod_get_func(nullptr, "anything"));
  EXPECT_EQ(nullptr, mod_get_func(&mod_val, nullptr));

  mod_close(&mod_val);
  EXPECT_TRUE(mod_is_open(&mod_val) == 0);
}
