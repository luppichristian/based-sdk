// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// =========================================================================
c_begin;
// =========================================================================

// Returns the number of base-10 digits needed to represent the value.
func sz u8_digits(u8 value);
func sz u16_digits(u16 value);
func sz u32_digits(u32 value);
func sz u64_digits(u64 value);

// Returns the number of characters needed for the base-10 representation,
// including the minus sign for negative values.
func sz i8_digits(i8 value);
func sz i16_digits(i16 value);
func sz i32_digits(i32 value);
func sz i64_digits(i64 value);

// Returns the number of base-10 digits needed to represent the value.
func sz u8x_digits(u8x value);
func sz u16x_digits(u16x value);
func sz u32x_digits(u32x value);
func sz u64x_digits(u64x value);
func sz sz_digits(sz value);
func sz up_digits(up value);

// Returns the number of characters needed for the base-10 representation,
// including the minus sign for negative values.
func sz i8x_digits(i8x value);
func sz i16x_digits(i16x value);
func sz i32x_digits(i32x value);
func sz i64x_digits(i64x value);
func sz sp_digits(sp value);
func sz dp_digits(dp value);

// =========================================================================
c_end;
// =========================================================================
