// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../strings/cstrings.h"

// =========================================================================
c_begin;
// =========================================================================

typedef struct system_info {
  str8_short os_name;
  str8_short os_version;
  str8_short architecture_name;
  str8_short computer_name;
  str8_short user_name;
  str8_short user_home;
  sz page_size;
  sz allocation_granularity;
} system_info;

// Queries coarse OS, machine, and active-user information for the current host.
func b32 system_info_query(system_info* out_info);

// =========================================================================
c_end;
// =========================================================================
