// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "assert.h"
#include "utility_defines.h"

// This is the max number of iterations a safe loop can perform.
// These loops are capped to a high value so that we can catch infinite loops.
// Once the iteration count hits, an assertion is triggered.
#define SAFE_LOOP_MAX_ITERATION_COUNT 10000

// You can define UNSAFE if you want to disable these.

// =========================================================================
// Safe while loop
// =========================================================================

#define SAFE_WHILE_IMPL(cond, id)        \
  for (int cat_exp(__it_count_, id) = 0; \
       (cond) && (assert(cat_exp(__it_count_, id)++ < SAFE_LOOP_MAX_ITERATION_COUNT), 1);)

#ifdef UNSAFE
#  define safe_while(cond) while (cond)
#else
#  define safe_while(cond) SAFE_WHILE_IMPL(cond, __COUNTER__)
#endif

// =========================================================================
// Safe for loop
// =========================================================================

#define SAFE_FOR_IMPL(id, ...)                                                         \
  for (int cat_exp(__run_, id) = 1, cat_exp(__it_count_, id) = 0; cat_exp(__run_, id); \
       cat_exp(__run_, id) = 0)                                                        \
    for (__VA_ARGS__)                                                                  \
      if ((assert(cat_exp(__it_count_, id)++ < SAFE_LOOP_MAX_ITERATION_COUNT), 0)) {   \
      } else

#ifdef UNSAFE
#  define safe_for(...) for (__VA_ARGS__)
#else
#  define safe_for(...) SAFE_FOR_IMPL(__COUNTER__, __VA_ARGS__)
#endif
