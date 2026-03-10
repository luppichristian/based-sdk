// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// =========================================================================
c_begin;
// =========================================================================

// A large sentinel value that can be used to force timer-based checks to pass
// without accumulating floating-point error over long runtimes.
const_var f32 VERY_HIGH_TIMER_VALUE = 100000.0f;

// Sets the timer to zero.
func void timer_clear(f32* timer);

// Sets the timer to a very high value.
func void timer_bump(f32* timer);

// Adds dt * scale to the timer.
func void timer_increment(f32* timer, f32 dt, f32 scale);

// If the timer has reached rate, subtracts rate and returns 1. Otherwise returns 0.
func b32 timer_consume(f32* timer, f32 rate);

// If the timer has reached rate, clears it and returns 1. Otherwise returns 0.
func b32 timer_consume_once(f32* timer, f32 rate);

// =========================================================================
c_end;
// =========================================================================
