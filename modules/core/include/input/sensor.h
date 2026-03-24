// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../memory/buffer.h"
#include "../strings/cstrings.h"
#include "devices.h"

typedef void* sensor;

// =========================================================================
c_begin;
// =========================================================================

typedef enum sensor_kind {
  SENSOR_KIND_INVALID = -1,
  SENSOR_KIND_UNKNOWN = 0,
  SENSOR_KIND_ACCEL = 1,
  SENSOR_KIND_GYRO = 2,
  SENSOR_KIND_ACCEL_L = 3,
  SENSOR_KIND_GYRO_L = 4,
  SENSOR_KIND_ACCEL_R = 5,
  SENSOR_KIND_GYRO_R = 6,
} sensor_kind;

// Returns 1 if src refers to a concrete sensor id, 0 otherwise.
func b32 sensor_is_valid(sensor src);

// Converts src into a sensor handle when it refers to a sensor device.
func sensor device_get_sensor(device src);

// Converts src into a generic device handle.
func device sensor_to_device(sensor src);

// Returns the number of currently known sensor devices.
func sz sensor_get_total_count(void);

// Returns the sensor handle at idx, or NULL when unavailable.
func sensor sensor_get_from_idx(sz idx);

// Returns the primary sensor handle, or NULL when unavailable.
func sensor sensor_get_primary(void);

// Returns the focused sensor handle, or the primary sensor when unavailable.
func sensor sensor_get_focused(void);

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
