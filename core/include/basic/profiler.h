// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "env_defines.h"
#include "primitive_types.h"

// =========================================================================
// Tracy include
// =========================================================================

// Respect build-system definitions and only enable Tracy when profiling is on.
#ifdef BASED_PROFILER_ENABLED
#  if !defined(TRACY_ENABLE)
#    define TRACY_ENABLE    1
#    define TRACY_CALLSTACK 32
#  endif

// Include Tracy profiler, macros expand to nothing if tracy is disabled.
#  include <tracy/TracyC.h>

// Helper functions for convenience
#  define profile_func_begin TracyCZoneN(__tracy_zone_ctx, __func__, 1)
#  define profile_func_end   TracyCZoneEnd(__tracy_zone_ctx)
#else
#  define profile_func_begin     ((void)0)
#  define profile_func_end       ((void)0)
#  define TracyCAlloc(ptr, size) ((void)(ptr), (void)(size))
#  define TracyCFree(ptr)        ((void)(ptr))
#endif
