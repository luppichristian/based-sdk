// MIT License
// Copyright (c) 2026 Christian Luppi

#include "system/cpu_info.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "basic/env_defines.h"
#include "basic/profiler.h"
#include "memory/memops.h"
#include "platform_includes.h"

#include <stddef.h>
#include <string.h>

#if defined(PLATFORM_WINDOWS) && (defined(ARCH_X86) || defined(ARCH_X86_64))
#  include <intrin.h>
#elif (defined(COMPILER_CLANG) || defined(COMPILER_APPLE_CLANG)) && \
    (defined(ARCH_X86) || defined(ARCH_X86_64))
#  include <cpuid.h>
#endif

func u32 cpu_query_logical_cores(void) {
#if defined(PLATFORM_WINDOWS)
  SYSTEM_INFO native_info;
  GetNativeSystemInfo(&native_info);
  return (u32)native_info.dwNumberOfProcessors;
#elif defined(PLATFORM_UNIX)
  sp core_count = (sp)sysconf(_SC_NPROCESSORS_ONLN);
  if (core_count > 0) {
    return (u32)core_count;
  }
  thread_log_warn("Falling back to one logical core after sysconf failure");
  return 1;
#else
  invalid_code_path;
  return 1;
#endif
}

func void cpu_set_compile_time_fallback(cpu_info* out_info) {
  profile_func_begin;
#if defined(ARCH_64)
  out_info->supports_64bit = 1;
#endif

#if defined(ARCH_ARM64) || defined(ARCH_ARM)
  cstr8_cpy(out_info->vendor_name, size_of(out_info->vendor_name), "ARM");
  cstr8_cpy(out_info->brand_name, size_of(out_info->brand_name), "ARM processor");
#elif defined(ARCH_X86_64) || defined(ARCH_X86)
  cstr8_cpy(out_info->vendor_name, size_of(out_info->vendor_name), "x86");
  cstr8_cpy(out_info->brand_name, size_of(out_info->brand_name), "x86 processor");
#else
  cstr8_cpy(out_info->vendor_name, size_of(out_info->vendor_name), "unknown");
  cstr8_cpy(out_info->brand_name, size_of(out_info->brand_name), "unknown");
#endif

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
  out_info->instruction_sets.neon = 1;
#endif
#if defined(__ARM_FEATURE_CRC32)
  out_info->instruction_sets.crc32 = 1;
#endif
  profile_func_end;
}

#if defined(ARCH_X86) || defined(ARCH_X86_64)
func b32 cpu_read_cpuid(u32 leaf_id, u32 subleaf_id, i32 out_regs[4]) {
  profile_func_begin;
  if (out_regs == NULL) {
    profile_func_end;
    return false;
  }
  assert(out_regs != NULL);
#  if defined(PLATFORM_WINDOWS)
  __cpuidex(out_regs, (i32)leaf_id, (i32)subleaf_id);
  profile_func_end;
  return true;
#  elif defined(COMPILER_CLANG) || defined(COMPILER_APPLE_CLANG)
  u32 eax_reg = 0;
  u32 ebx_reg = 0;
  u32 ecx_reg = 0;
  u32 edx_reg = 0;
  if (__get_cpuid_count(leaf_id, subleaf_id, &eax_reg, &ebx_reg, &ecx_reg, &edx_reg) == 0) {
    thread_log_debug("CPUID leaf unavailable leaf=0x%08X subleaf=0x%08X", leaf_id, subleaf_id);
    profile_func_end;
    return false;
  }
  out_regs[0] = (i32)eax_reg;
  out_regs[1] = (i32)ebx_reg;
  out_regs[2] = (i32)ecx_reg;
  out_regs[3] = (i32)edx_reg;
  profile_func_end;
  return true;
#  else
  (void)leaf_id;
  (void)subleaf_id;
  (void)out_regs;
  profile_func_end;
  return false;
#  endif
}

func void cpu_fill_x86_strings(cpu_info* out_info) {
  profile_func_begin;
  if (out_info == NULL) {
    profile_func_end;
    return;
  }
  assert(out_info != NULL);
  i32 base_regs[4];
  if (cpu_read_cpuid(0, 0, base_regs)) {
    c8 vendor_name[13];
    mem_cpy(&vendor_name[0], &base_regs[1], 4);
    mem_cpy(&vendor_name[4], &base_regs[3], 4);
    mem_cpy(&vendor_name[8], &base_regs[2], 4);
    vendor_name[12] = '\0';
    cstr8_cpy(out_info->vendor_name, size_of(out_info->vendor_name), vendor_name);
  }

  i32 ext_regs[4];
  if (!cpu_read_cpuid(0x80000000U, 0, ext_regs)) {
    thread_log_debug("CPU extended CPUID range unavailable");
    profile_func_end;
    return;
  }
  if ((u32)ext_regs[0] < 0x80000004U) {
    thread_log_debug("CPU brand string leaves unavailable max_leaf=0x%08X", (u32)ext_regs[0]);
    profile_func_end;
    return;
  }

  i32 brand_regs[12];
  c8 brand_name[49];
  mem_zero(brand_regs, size_of(brand_regs));
  mem_zero(brand_name, size_of(brand_name));
  cpu_read_cpuid(0x80000002U, 0, &brand_regs[0]);
  cpu_read_cpuid(0x80000003U, 0, &brand_regs[4]);
  cpu_read_cpuid(0x80000004U, 0, &brand_regs[8]);
  mem_cpy(brand_name, brand_regs, 48);
  cstr8_cpy(out_info->brand_name, size_of(out_info->brand_name), brand_name);
  profile_func_end;
}

func void cpu_fill_x86_features(cpu_info* out_info) {
  profile_func_begin;
  if (out_info == NULL) {
    profile_func_end;
    return;
  }
  assert(out_info != NULL);
  i32 base_regs[4];
  if (!cpu_read_cpuid(1, 0, base_regs)) {
    thread_log_warn("Failed to query primary CPU feature leaf");
    profile_func_end;
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
  profile_func_end;
}
#endif

func b32 cpu_info_query(cpu_info* out_info) {
  profile_func_begin;
  if (out_info == NULL) {
    profile_func_end;
    return false;
  }
  assert(out_info != NULL);

  mem_zero(out_info, size_of(*out_info));
  out_info->logical_core_count = cpu_query_logical_cores();
  out_info->cache_line_bytes = 64;
  cpu_set_compile_time_fallback(out_info);

#if defined(ARCH_X86) || defined(ARCH_X86_64)
  cpu_fill_x86_strings(out_info);
  cpu_fill_x86_features(out_info);
#endif

  thread_log_trace("Queried CPU info cores=%u cache_line=%u", out_info->logical_core_count, out_info->cache_line_bytes);
  profile_func_end;
  return true;
}
