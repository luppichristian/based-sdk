// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../strings/cstrings.h"

// =========================================================================
c_begin;
// =========================================================================

typedef struct cpu_instruction_set_info {
  b32 mmx;
  b32 sse;
  b32 sse2;
  b32 sse3;
  b32 ssse3;
  b32 sse41;
  b32 sse42;
  b32 avx;
  b32 avx2;
  b32 avx512f;
  b32 aes;
  b32 fma;
  b32 bmi1;
  b32 bmi2;
  b32 popcnt;
  b32 neon;
  b32 crc32;
} cpu_instruction_set_info;

typedef struct cpu_info {
  str8_short vendor_name;
  str8_short brand_name;
  u32 logical_core_count;
  u32 cache_line_bytes;
  b32 supports_64bit;
  cpu_instruction_set_info instruction_sets;
} cpu_info;

// Queries coarse CPU identity, topology, and supported instruction sets.
func b32 cpu_info_query(cpu_info* out_info);

// =========================================================================
c_end;
// =========================================================================
