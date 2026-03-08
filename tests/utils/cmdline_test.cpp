// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(utils_cmdline_test, build_empty) {
  cmdline cmdl = cmdline_build(0, nullptr);
  EXPECT_EQ(0U, cmdline_get_count(cmdl));
  EXPECT_TRUE(cmdline_is_empty(cmdl) != 0);
}

TEST(utils_cmdline_test, build_with_args) {
  c8* argv[] = {(c8*)"prog", (c8*)"arg1", (c8*)"arg2"};
  cmdline cmdl = cmdline_build(3, argv);
  EXPECT_EQ(3U, cmdline_get_count(cmdl));
  EXPECT_FALSE(cmdline_is_empty(cmdl) != 0);
}

TEST(utils_cmdline_test, get_program) {
  c8* argv[] = {(c8*)"myprogram", (c8*)"arg1"};
  cmdline cmdl = cmdline_build(2, argv);
  cstr8 prog = cmdline_get_program(cmdl);
  EXPECT_STREQ("myprogram", prog);
}

TEST(utils_cmdline_test, get_arg) {
  c8* argv[] = {(c8*)"prog", (c8*)"first", (c8*)"second"};
  cmdline cmdl = cmdline_build(3, argv);
  cstr8 arg0 = cmdline_get_arg(cmdl, 0);
  cstr8 arg1 = cmdline_get_arg(cmdl, 1);
  cstr8 arg2 = cmdline_get_arg(cmdl, 2);
  cstr8 arg3 = cmdline_get_arg(cmdl, 3);
  EXPECT_STREQ("prog", arg0);
  EXPECT_STREQ("first", arg1);
  EXPECT_STREQ("second", arg2);
  EXPECT_EQ(nullptr, arg3);
}

TEST(utils_cmdline_test, find) {
  c8* argv[] = {(c8*)"prog", (c8*)"-v", (c8*)"--verbose"};
  cmdline cmdl = cmdline_build(3, argv);
  sz idx;
  b32 found = cmdline_find(cmdl, "--verbose", &idx);
  EXPECT_TRUE(found != 0);
  EXPECT_EQ(2U, idx);
}

TEST(utils_cmdline_test, has) {
  c8* argv[] = {(c8*)"prog", (c8*)"-v", (c8*)"--verbose"};
  cmdline cmdl = cmdline_build(3, argv);
  EXPECT_TRUE(cmdline_has(cmdl, "-v") != 0);
  EXPECT_TRUE(cmdline_has(cmdl, "--verbose") != 0);
  EXPECT_FALSE(cmdline_has(cmdl, "--help") != 0);
}

TEST(utils_cmdline_test, get_option) {
  c8* argv[] = {(c8*)"prog", (c8*)"--name", (c8*)"value"};
  cmdline cmdl = cmdline_build(3, argv);
  cstr8 opt = cmdline_get_option(cmdl, "name");
  EXPECT_STREQ("value", opt);
}

TEST(utils_cmdline_test, get_option_equals) {
  c8* argv[] = {(c8*)"prog", (c8*)"--name=value"};
  cmdline cmdl = cmdline_build(2, argv);
  cstr8 opt = cmdline_get_option(cmdl, "name");
  EXPECT_STREQ("value", opt);
}

TEST(utils_cmdline_test, get_option_missing) {
  c8* argv[] = {(c8*)"prog", (c8*)"--other", (c8*)"val"};
  cmdline cmdl = cmdline_build(3, argv);
  cstr8 opt = cmdline_get_option(cmdl, "name");
  EXPECT_EQ(nullptr, opt);
}

TEST(utils_cmdline_test, get_option_i64) {
  c8* argv[] = {(c8*)"prog", (c8*)"--count", (c8*)"42"};
  cmdline cmdl = cmdline_build(3, argv);
  i64 val = 0;
  b32 success = cmdline_get_option_i64(cmdl, "count", &val);
  EXPECT_TRUE(success != 0);
  EXPECT_EQ(42LL, val);
}

TEST(utils_cmdline_test, get_option_f64) {
  c8* argv[] = {(c8*)"prog", (c8*)"--rate", (c8*)"3.14"};
  cmdline cmdl = cmdline_build(3, argv);
  f64 val = 0.0;
  b32 success = cmdline_get_option_f64(cmdl, "rate", &val);
  EXPECT_TRUE(success != 0);
  EXPECT_DOUBLE_EQ(3.14, val);
}
