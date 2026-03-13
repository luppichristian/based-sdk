// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../memory/buffer.h"
#include "../strings/cstrings.h"
#include "devices.h"

// =========================================================================
c_begin;
// =========================================================================

// Returns 1 if src refers to a concrete sensor id, 0 otherwise.
func b32 sensor_is_valid(sensor src);

// Returns the number of currently known sensor devices.
func sz sensor_get_total_count(void);

// Writes the sensor id at idx into out_id. Returns 1 on success, 0 otherwise.
func b32 sensor_get_from_idx(sz idx, sensor* out_id);

// Returns a backend-defined sensor name for id, or NULL when unavailable.
func cstr8 sensor_get_name(sensor sen_id);

// Returns the SDL-compatible sensor kind for id.
func sensor_kind sensor_get_kind(sensor sen_id);

// Device lifecycle and sample retrieval.
func b32 sensor_open(sensor sen_id);
func b32 sensor_close(sensor sen_id);
func b32 sensor_start(sensor sen_id);
func b32 sensor_stop(sensor sen_id);
func b32 sensor_read(sensor sen_id, buffer* out_samples);

// =========================================================================
c_end;
// =========================================================================
