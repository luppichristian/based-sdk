// MIT License
// Copyright (c) 2026 Christian Luppi

#include "system/cpu_info.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "basic/env_defines.h"
#include "basic/profiler.h"

#include <stddef.h>
#include <string.h>

#if defined(PLATFORM_WINDOWS)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#elif defined(PLATFORM_UNIX)
#  include <unistd.h>
#endif

#if defined(COMPILER_MSVC) && (defined(ARCH_X86) || defined(ARCH_X86_64))
#  include <intrin.h>
#elif (defined(COMPILER_GCC) || defined(COMPILER_CLANG) || defined(COMPILER_APPLE_CLANG)) && \
    (defined(ARCH_X86) || defined(ARCH_X86_64))
#  include <cpuid.h>
#endif

func void cpu_copy_string(c8* dst_ptr, sz dst_cap, cstr8 src_ptr) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (dst_ptr == NULL || dst_cap == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(dst_cap > 0);

  dst_ptr[0] = '\0';
  if (src_ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  sz src_len = cstr8_len(src_ptr);
  if (src_len >= dst_cap) {
    src_len = dst_cap - 1;
  }

  memcpy(dst_ptr, src_ptr, src_len);
  dst_ptr[src_len] = '\0';
  TracyCZoneEnd(__tracy_zone_ctx);
}

func u32 cpu_query_logical_cores(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
#if defined(PLATFORM_WINDOWS)
  SYSTEM_INFO native_info;
  GetNativeSystemInfo(&native_info);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (u32)native_info.dwNumberOfProcessors;
#elif defined(PLATFORM_UNIX)
  sp core_count = (sp)sysconf(_SC_NPROCESSORS_ONLN);
  if (core_count > 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return (u32)core_count;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
#else
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
#endif
}

func void cpu_set_compile_time_fallback(cpu_info* out_info) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
#if defined(ARCH_64)
  out_info->supports_64bit = 1;
#endif

#if defined(ARCH_ARM64) || defined(ARCH_ARM)
  cpu_copy_string(out_info->vendor_name, size_of(out_info->vendor_name), "ARM");
  cpu_copy_string(out_info->brand_name, size_of(out_info->brand_name), "ARM processor");
#elif defined(ARCH_X86_64) || defined(ARCH_X86)
  cpu_copy_string(out_info->vendor_name, size_of(out_info->vendor_name), "x86");
  cpu_copy_string(out_info->brand_name, size_of(out_info->brand_name), "x86 processor");
#else
  cpu_copy_string(out_info->vendor_name, size_of(out_info->vendor_name), "unknown");
  cpu_copy_string(out_info->brand_name, size_of(out_info->brand_name), "unknown");
#endif

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
  out_info->instruction_sets.neon = 1;
#endif
#if defined(__ARM_FEATURE_CRC32)
  out_info->instruction_sets.crc32 = 1;
#endif
  TracyCZoneEnd(__tracy_zone_ctx);
}

#if defined(ARCH_X86) || defined(ARCH_X86_64)
func b32 cpu_read_cpuid(u32 leaf_id, u32 subleaf_id, i32 out_regs[4]) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (out_regs == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(out_regs != NULL);
#  if defined(COMPILER_MSVC)
  __cpuidex(out_regs, (i32)leaf_id, (i32)subleaf_id);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
#  elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || defined(COMPILER_APPLE_CLANG)
  u32 eax_reg = 0;
  u32 ebx_reg = 0;
  u32 ecx_reg = 0;
  u32 edx_reg = 0;
  if (__get_cpuid_count(leaf_id, subleaf_id, &eax_reg, &ebx_reg, &ecx_reg, &edx_reg) == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  out_regs[0] = (i32)eax_reg;
  out_regs[1] = (i32)ebx_reg;
  out_regs[2] = (i32)ecx_reg;
  out_regs[3] = (i32)edx_reg;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
#  else
  (void)leaf_id;
  (void)subleaf_id;
  (void)out_regs;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
#  endif
}

func void cpu_fill_x86_strings(cpu_info* out_info) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (out_info == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(out_info != NULL);
  i32 base_regs[4];
  if (cpu_read_cpuid(0, 0, base_regs)) {
    c8 vendor_name[13];
    memcpy(&vendor_name[0], &base_regs[1], 4);
    memcpy(&vendor_name[4], &base_regs[3], 4);
    memcpy(&vendor_name[8], &base_regs[2], 4);
    vendor_name[12] = '\0';
    cpu_copy_string(out_info->vendor_name, size_of(out_info->vendor_name), vendor_name);
  }

  i32 ext_regs[4];
  if (!cpu_read_cpuid(0x80000000U, 0, ext_regs)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  if ((u32)ext_regs[0] < 0x80000004U) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  i32 brand_regs[12];
  c8 brand_name[49];
  memset(brand_regs, 0, size_of(brand_regs));
  memset(brand_name, 0, size_of(brand_name));
  cpu_read_cpuid(0x80000002U, 0, &brand_regs[0]);
  cpu_read_cpuid(0x80000003U, 0, &brand_regs[4]);
  cpu_read_cpuid(0x80000004U, 0, &brand_regs[8]);
  memcpy(brand_name, brand_regs, 48);
  cpu_copy_string(out_info->brand_name, size_of(out_info->brand_name), brand_name);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void cpu_fill_x86_features(cpu_info* out_info) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (out_info == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(out_info != NULL);
  i32 base_regs[4];
  if (!cpu_read_cpuid(1, 0, base_regs)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  u32 ebx_reg = (u32)base_regs[1];
  u32 ecx_reg = (u32)base_regs[2];
  u32 edx_reg = (u32)base_regs[3];

  out_info->cache_line_bytes = ((ebx_reg >> 8U) & 0xFFU) * 8U;
  out_info->instruction_sets.mmx = (edx_reg & (1U << 23U)) != 0U;
  out_info->instruction_sets.sse = (edx_reg & (1U << 25U)) != 0U;
  out_info->instruction_sets.sse2 = (edx_reg & (1U << 26U)) != 0U;
  out_info->instruction_sets.sse3 = (ecx_reg & (1U << 0U)) != 0U;
  out_info->instruction_sets.ssse3 = (ecx_reg & (1U << 9U)) != 0U;
  out_info->instruction_sets.sse41 = (ecx_reg & (1U << 19U)) != 0U;
  out_info->instruction_sets.sse42 = (ecx_reg & (1U << 20U)) != 0U;
  out_info->instruction_sets.aes = (ecx_reg & (1U << 25U)) != 0U;
  out_info->instruction_sets.avx = (ecx_reg & (1U << 28U)) != 0U;
  out_info->instruction_sets.fma = (ecx_reg & (1U << 12U)) != 0U;
  out_info->instruction_sets.popcnt = (ecx_reg & (1U << 23U)) != 0U;

  i32 ext_regs[4];
  if (cpu_read_cpuid(7, 0, ext_regs)) {
    u32 ebx_ext = (u32)ext_regs[1];
    out_info->instruction_sets.bmi1 = (ebx_ext & (1U << 3U)) != 0U;
    out_info->instruction_sets.avx2 = (ebx_ext & (1U << 5U)) != 0U;
    out_info->instruction_sets.bmi2 = (ebx_ext & (1U << 8U)) != 0U;
    out_info->instruction_sets.avx512f = (ebx_ext & (1U << 16U)) != 0U;
  }

  i32 long_mode_regs[4];
  if (cpu_read_cpuid(0x80000001U, 0, long_mode_regs)) {
    out_info->supports_64bit = (((u32)long_mode_regs[3]) & (1U << 29U)) != 0U;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}
#endif

func b32 cpu_info_query(cpu_info* out_info) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (out_info == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(out_info != NULL);

  memset(out_info, 0, size_of(*out_info));
  out_info->logical_core_count = cpu_query_logical_cores();
  out_info->cache_line_bytes = 64;
  cpu_set_compile_time_fallback(out_info);

#if defined(ARCH_X86) || defined(ARCH_X86_64)
  cpu_fill_x86_strings(out_info);
  cpu_fill_x86_features(out_info);
#endif

  thread_log_trace("cpu_info_query: cores=%u cache_line=%u", out_info->logical_core_count, out_info->cache_line_bytes);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

