// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/timer.h"

func void timer_clear(f32* timer) {
  *timer = 0.0f;
}

func void timer_bump(f32* timer) {
  *timer = VERY_HIGH_TIMER_VALUE;
}

func void timer_increment(f32* timer, f32 dt, f32 scale) {
  *timer += dt * scale;
}

func b32 timer_consume(f32* timer, f32 rate) {
  if (rate <= 0.0f || *timer < rate) {
    return 0;
  }

  *timer -= rate;
  return 1;
}

func b32 timer_consume_once(f32* timer, f32 rate) {
  if (rate <= 0.0f || *timer < rate) {
    return 0;
  }

  timer_clear(timer);
  return 1;
}
