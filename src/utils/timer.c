// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/timer.h"
#include "basic/assert.h"
#include "basic/profiler.h"

func void timer_clear(f32* timer) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (timer == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(timer != NULL);
  *timer = 0.0f;
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void timer_bump(f32* timer) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (timer == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(timer != NULL);
  *timer = VERY_HIGH_TIMER_VALUE;
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void timer_increment(f32* timer, f32 dt, f32 scale) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (timer == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(timer != NULL);
  *timer += dt * scale;
  TracyCZoneEnd(__tracy_zone_ctx);
}

func b32 timer_consume(f32* timer, f32 rate) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (timer == NULL || rate <= 0.0f || *timer < rate) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(timer != NULL);

  *timer -= rate;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 timer_consume_once(f32* timer, f32 rate) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (timer == NULL || rate <= 0.0f || *timer < rate) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(timer != NULL);

  timer_clear(timer);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}
