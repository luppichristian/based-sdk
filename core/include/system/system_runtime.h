// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// =========================================================================
c_begin;
// =========================================================================

typedef struct system_runtime_info {
  f32 cpu_usage_percent;
  sz memory_total;
  sz memory_used;
  sz memory_available;
  sz process_memory_used;
  sz process_memory_peak;
} system_runtime_info;

// Queries current host and process runtime metrics.
// cpu_usage_percent is measured as a delta from the previous call and may be 0 on the first sample.
func b32 system_runtime_query(system_runtime_info* out_info);

// =========================================================================
c_end;
// =========================================================================
