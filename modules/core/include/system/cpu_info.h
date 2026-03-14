// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../strings/cstrings.h"

// =========================================================================
c_begin;
// =========================================================================

typedef struct cpu_instruction_set_info {
  // Legacy and baseline x86 SIMD feature flags.
  b32 mmx;
  b32 sse;
  b32 sse2;
  b32 sse3;
  b32 ssse3;
  b32 sse41;
  b32 sse42;

  // Wider vector and crypto/math acceleration support.
  b32 avx;
  b32 avx2;
  b32 avx512f;
  b32 aes;
  b32 fma;

  // Bit-manipulation and utility instruction support.
  b32 bmi1;
  b32 bmi2;
  b32 popcnt;

  // ARM-specific SIMD and checksum extensions.
  b32 neon;
  b32 crc32;
} cpu_instruction_set_info;

typedef struct cpu_info {
  // Vendor identifier such as "GenuineIntel", "AuthenticAMD", or a fallback label.
  str8_short vendor_name;

  // Human-readable marketing/model name when the platform exposes one.
  str8_short brand_name;

  // Online logical execution units visible to the OS, including SMT/hyper-thread siblings.
  u32 logical_core_count;

  // Smallest commonly shared hardware cache line size in bytes.
  // This is useful for alignment and false-sharing avoidance.
  u32 cache_line_bytes;

  // Fine-grained CPU feature bits grouped by instruction-family support.
  cpu_instruction_set_info instruction_sets;
} cpu_info;

// Queries coarse CPU identity, topology, and supported instruction sets.
func b32 cpu_info_query(cpu_info* out_info);

// =========================================================================
c_end;
// =========================================================================
