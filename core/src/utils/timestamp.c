// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/timestamp.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"
#include "based_core.h"

func timestamp timestamp_zero(void) {
  timestamp value = {.microseconds = 0};
  return value;
}

func timestamp timestamp_now(void) {
  profile_func_begin;
  SDL_Time current_time = 0;

  if (!SDL_GetCurrentTime(&current_time)) {
    thread_log_warn("timestamp_now: SDL_GetCurrentTime failed");
    profile_func_end;
    return timestamp_zero();
  }
  assert(current_time >= 0);
  timestamp stamp = timestamp_from_microseconds((i64)SDL_NS_TO_US(current_time));
  profile_func_end;
  return stamp;
}

func timestamp timestamp_from_microseconds(i64 microseconds) {
  timestamp value = {.microseconds = microseconds};
  return value;
}

func timestamp timestamp_from_milliseconds(i64 milliseconds) {
  return timestamp_from_microseconds(milliseconds * 1000);
}

func timestamp timestamp_from_seconds(f64 seconds) {
  return timestamp_from_microseconds((i64)round_f64(seconds * 1000000.0));
}

func timestamp timestamp_from_minutes(f64 minutes) {
  return timestamp_from_seconds(minutes * 60.0);
}

func timestamp timestamp_from_hours(f64 hours) {
  return timestamp_from_minutes(hours * 60.0);
}

func i64 timestamp_as_microseconds(timestamp value) {
  return value.microseconds;
}

func f64 timestamp_as_milliseconds(timestamp value) {
  return (f64)value.microseconds / 1000.0;
}

func f64 timestamp_as_seconds(timestamp value) {
  return (f64)value.microseconds / 1000000.0;
}

func f64 timestamp_as_minutes(timestamp value) {
  return timestamp_as_seconds(value) / 60.0;
}

func f64 timestamp_as_hours(timestamp value) {
  return timestamp_as_minutes(value) / 60.0;
}

func b32 timestamp_is_zero(timestamp value) {
  return value.microseconds == 0 ? true : false;
}

func timestamp timestamp_add(timestamp lhs, timestamp rhs) {
  return timestamp_from_microseconds(lhs.microseconds + rhs.microseconds);
}

func timestamp timestamp_sub(timestamp lhs, timestamp rhs) {
  return timestamp_from_microseconds(lhs.microseconds - rhs.microseconds);
}

func timestamp timestamp_scale(timestamp value, f64 factor) {
  return timestamp_from_microseconds(round((f64)value.microseconds * factor));
}

func timestamp timestamp_abs(timestamp value) {
  if (value.microseconds < 0) {
    return timestamp_from_microseconds(-value.microseconds);
  }
  return value;
}

func timestamp timestamp_clamp(timestamp value, timestamp min_value, timestamp max_value) {
  profile_func_begin;
  if (timestamp_cmp(value, min_value) < 0) {
    return min_value;
  }
  if (timestamp_cmp(value, max_value) > 0) {
    return max_value;
  }
  return value;
}

func i32 timestamp_cmp(timestamp lhs, timestamp rhs) {
  if (lhs.microseconds < rhs.microseconds) {
    return -1;
  }
  if (lhs.microseconds > rhs.microseconds) {
    return 1;
  }
  return 0;
}
