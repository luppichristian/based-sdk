// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

#define SYSTEM_INFO_OS_NAME_CAP       64
#define SYSTEM_INFO_OS_VERSION_CAP    128
#define SYSTEM_INFO_ARCH_NAME_CAP     32
#define SYSTEM_INFO_COMPUTER_NAME_CAP 128
#define SYSTEM_INFO_USER_NAME_CAP     128
#define SYSTEM_INFO_HOME_PATH_CAP     512

typedef struct system_info {
  c8 os_name[SYSTEM_INFO_OS_NAME_CAP];
  c8 os_version[SYSTEM_INFO_OS_VERSION_CAP];
  c8 architecture_name[SYSTEM_INFO_ARCH_NAME_CAP];
  c8 computer_name[SYSTEM_INFO_COMPUTER_NAME_CAP];
  c8 user_name[SYSTEM_INFO_USER_NAME_CAP];
  c8 user_home[SYSTEM_INFO_HOME_PATH_CAP];
  sz page_size;
  sz allocation_granularity;
} system_info;

// Queries coarse OS, machine, and active-user information for the current host.
func b32 system_info_query(system_info* out_info);
