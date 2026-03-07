// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/timestamp.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"

#include "../sdl3_include.h"
#include "basic/profiler.h"

func i64 timestamp_round_to_i64(f64 value) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (value >= 0.0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return (i64)(value + 0.5);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return (i64)(value - 0.5);
}

func timestamp timestamp_zero(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  timestamp value = {.microseconds = 0};
  TracyCZoneEnd(__tracy_zone_ctx);
  return value;
}

func timestamp timestamp_now(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  SDL_Time current_time = 0;

  if (!SDL_GetCurrentTime(&current_time)) {
    thread_log_warn("timestamp_now: SDL_GetCurrentTime failed");
    TracyCZoneEnd(__tracy_zone_ctx);
    return timestamp_zero();
  }
  assert(current_time >= 0);

  TracyCZoneEnd(__tracy_zone_ctx);
  return timestamp_from_microseconds((i64)SDL_NS_TO_US(current_time));
}

func timestamp timestamp_from_microseconds(i64 microseconds) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  timestamp value = {.microseconds = microseconds};
  TracyCZoneEnd(__tracy_zone_ctx);
  return value;
}

func timestamp timestamp_from_milliseconds(i64 milliseconds) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return timestamp_from_microseconds(milliseconds * 1000);
}

func timestamp timestamp_from_seconds(f64 seconds) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return timestamp_from_microseconds(timestamp_round_to_i64(seconds * 1000000.0));
}

func timestamp timestamp_from_minutes(f64 minutes) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return timestamp_from_seconds(minutes * 60.0);
}

func timestamp timestamp_from_hours(f64 hours) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return timestamp_from_minutes(hours * 60.0);
}

func i64 timestamp_as_microseconds(timestamp value) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return value.microseconds;
}

func f64 timestamp_as_milliseconds(timestamp value) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (f64)value.microseconds / 1000.0;
}

func f64 timestamp_as_seconds(timestamp value) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (f64)value.microseconds / 1000000.0;
}

func f64 timestamp_as_minutes(timestamp value) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return timestamp_as_seconds(value) / 60.0;
}

func f64 timestamp_as_hours(timestamp value) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return timestamp_as_minutes(value) / 60.0;
}

func b32 timestamp_is_zero(timestamp value) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return value.microseconds == 0 ? 1 : 0;
}

func timestamp timestamp_add(timestamp lhs, timestamp rhs) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return timestamp_from_microseconds(lhs.microseconds + rhs.microseconds);
}

func timestamp timestamp_sub(timestamp lhs, timestamp rhs) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return timestamp_from_microseconds(lhs.microseconds - rhs.microseconds);
}

func timestamp timestamp_scale(timestamp value, f64 factor) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return timestamp_from_microseconds(timestamp_round_to_i64((f64)value.microseconds * factor));
}

func timestamp timestamp_abs(timestamp value) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (value.microseconds < 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return timestamp_from_microseconds(-value.microseconds);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return value;
}

func timestamp timestamp_clamp(timestamp value, timestamp min_value, timestamp max_value) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (timestamp_cmp(value, min_value) < 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return min_value;
  }
  if (timestamp_cmp(value, max_value) > 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return max_value;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return value;
}

func i32 timestamp_cmp(timestamp lhs, timestamp rhs) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (lhs.microseconds < rhs.microseconds) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return -1;
  }
  if (lhs.microseconds > rhs.microseconds) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
}
