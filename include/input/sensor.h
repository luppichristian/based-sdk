// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../strings/cstrings.h"
#include "devices.h"

typedef struct sensor_id {
  u64 opaque;
} sensor_id;

// Returns 1 if src refers to a concrete sensor id, 0 otherwise.
func b32 sensor_id_is_valid(sensor_id src);

// Builds a sensor_id from a backend-native sensor identifier.
func sensor_id sensor_from_native_id(u64 native_id);

// Returns the backend-native sensor identifier encoded in src.
func u64 sensor_to_native_id(sensor_id src);

// Returns the number of currently known sensor devices.
func sz sensor_get_count(void);

// Writes the sensor id at idx into out_id. Returns 1 on success, 0 otherwise.
func b32 sensor_get_id(sz idx, sensor_id* out_id);

// Returns a backend-defined sensor name for id, or NULL when unavailable.
func cstr8 sensor_get_name(sensor_id id);

// Returns the SDL-compatible sensor kind for id.
func sensor_kind sensor_get_kind(sensor_id id);

// Returns the backend-specific non-portable sensor kind for id.
func i32 sensor_get_non_portable_kind(sensor_id id);
