// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(basic_env_defines_test, one_platform_and_one_arch_are_detected) {
  i32 platform_count = 0;
#if defined(PLATFORM_WINDOWS)
  platform_count += 1;
#endif
#if defined(PLATFORM_LINUX)
  platform_count += 1;
#endif
#if defined(PLATFORM_MACOS)
  platform_count += 1;
#endif

  i32 arch_count = 0;
#if defined(ARCH_X86_64)
  arch_count += 1;
#endif
#if defined(ARCH_ARM64)
  arch_count += 1;
#endif

  EXPECT_EQ(platform_count, 1);
  EXPECT_EQ(arch_count, 1);
}

TEST(basic_env_defines_test, build_configuration_is_selected) {
  i32 build_count = 0;
#if defined(BUILD_DEBUG)
  build_count += 1;
#endif
#if defined(BUILD_RELEASE)
  build_count += 1;
#endif
#if defined(BUILD_PROFILE)
  build_count += 1;
#endif
#if defined(BUILD_TEST)
  build_count += 1;
#endif
#if defined(BUILD_SANITIZE)
  build_count += 1;
#endif

  EXPECT_GT(build_count, 0);
}
