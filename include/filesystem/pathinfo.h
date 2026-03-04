// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../utils/timestamp.h"
#include "path.h"

typedef enum pathinfo_type {
  PATHINFO_TYPE_NONE = 0,
  PATHINFO_TYPE_FILE = 1,
  PATHINFO_TYPE_DIRECTORY = 2,
  PATHINFO_TYPE_SYMLINK = 3,
  PATHINFO_TYPE_OTHER = 4,
} pathinfo_type;

typedef struct pathinfo {
  pathinfo_type kind;
  sz size;
  timestamp create_time;
  timestamp access_time;
  timestamp write_time;
  b32 exists;
  b32 is_read_only;
  b32 hidden;
} pathinfo;

// Queries metadata for src. Returns 1 on success, 0 otherwise.
func b32 path_get_info(const path* src, pathinfo* out_info);
