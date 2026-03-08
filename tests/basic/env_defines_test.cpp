// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(basic_env_defines_test, exactly_one_binary_kind_is_defined) {
  i32 kind_count = 0;
#if defined(BIN_RUNNABLE)
  kind_count += 1;
#endif
#if defined(BIN_DYNAMIC_LIB)
  kind_count += 1;
#endif
#if defined(BIN_STATIC_LIB)
  kind_count += 1;
#endif
#if defined(BIN_OBJ_LIB)
  kind_count += 1;
#endif

  EXPECT_EQ(kind_count, 1);
}

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
#if defined(ARCH_X86)
  arch_count += 1;
#endif
#if defined(ARCH_X86_64)
  arch_count += 1;
#endif
#if defined(ARCH_ARM)
  arch_count += 1;
#endif
#if defined(ARCH_ARM64)
  arch_count += 1;
#endif
#if defined(ARCH_RISCV)
  arch_count += 1;
#endif
#if defined(ARCH_POWERPC)
  arch_count += 1;
#endif
#if defined(ARCH_MIPS)
  arch_count += 1;
#endif
#if defined(ARCH_SPARC)
  arch_count += 1;
#endif

  EXPECT_EQ(platform_count, 1);
  EXPECT_EQ(arch_count, 1);
}

TEST(basic_env_defines_test, one_compiler_and_one_word_size_macro_are_detected) {
  i32 compiler_count = 0;
#if defined(COMPILER_MSVC)
  compiler_count += 1;
#endif
#if defined(COMPILER_GCC)
  compiler_count += 1;
#endif
#if defined(COMPILER_CLANG)
  compiler_count += 1;
#endif
#if defined(COMPILER_INTEL)
  compiler_count += 1;
#endif
#if defined(COMPILER_APPLE_CLANG)
  compiler_count += 1;
#endif

  i32 word_size_count = 0;
#if defined(ARCH_32)
  word_size_count += 1;
#endif
#if defined(ARCH_64)
  word_size_count += 1;
#endif

  EXPECT_EQ(compiler_count, 1);
  EXPECT_EQ(word_size_count, 1);
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
