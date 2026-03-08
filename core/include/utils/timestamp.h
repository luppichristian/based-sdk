// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// Duration-like timestamp stored as signed microseconds.
typedef struct timestamp {
  i64 microseconds;
} timestamp;

static_assert(size_of(timestamp) == size_of(i64));

// Constructs a zero timestamp.
func timestamp timestamp_zero(void);

// Returns the current wall-clock time as Unix-epoch microseconds, or zero on failure.
func timestamp timestamp_now(void);

// Constructs a timestamp from whole microseconds.
func timestamp timestamp_from_microseconds(i64 microseconds);

// Constructs a timestamp from whole milliseconds.
func timestamp timestamp_from_milliseconds(i64 milliseconds);

// Constructs a timestamp from seconds.
func timestamp timestamp_from_seconds(f64 seconds);

// Constructs a timestamp from minutes.
func timestamp timestamp_from_minutes(f64 minutes);

// Constructs a timestamp from hours.
func timestamp timestamp_from_hours(f64 hours);

// Returns the raw microsecond count.
func i64 timestamp_as_microseconds(timestamp value);

// Returns the value in milliseconds.
func f64 timestamp_as_milliseconds(timestamp value);

// Returns the value in seconds.
func f64 timestamp_as_seconds(timestamp value);

// Returns the value in minutes.
func f64 timestamp_as_minutes(timestamp value);

// Returns the value in hours.
func f64 timestamp_as_hours(timestamp value);

// Returns 1 if the timestamp is zero, 0 otherwise.
func b32 timestamp_is_zero(timestamp value);

// Adds two timestamps.
func timestamp timestamp_add(timestamp lhs, timestamp rhs);

// Subtracts rhs from lhs.
func timestamp timestamp_sub(timestamp lhs, timestamp rhs);

// Multiplies the timestamp by factor.
func timestamp timestamp_scale(timestamp value, f64 factor);

// Returns the absolute value.
func timestamp timestamp_abs(timestamp value);

// Clamps value into [min_value, max_value].
func timestamp timestamp_clamp(timestamp value, timestamp min_value, timestamp max_value);

// Compares two timestamps. Returns 0 if equal, <0 or >0 otherwise.
func i32 timestamp_cmp(timestamp lhs, timestamp rhs);
