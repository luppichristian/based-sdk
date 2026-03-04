// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../strings/cstrings.h"

// =========================================================================
// Input Devices
// =========================================================================

// Stable device categories shared across the input modules.
typedef enum device_type {
  DEVICE_TYPE_UNKNOWN = 0,
  DEVICE_TYPE_KEYBOARD = 1,
  DEVICE_TYPE_MOUSE = 2,
  DEVICE_TYPE_GAMEPAD = 3,
  DEVICE_TYPE_TABLET = 4,
  DEVICE_TYPE_TOUCH = 5,
} device_type;

// Backend-defined device identifier.
typedef struct device_id {
  device_type type;
  u64 instance;
} device_id;

// Snapshot of the metadata currently known for an input device.
typedef struct device_info {
  device_id id;
  b32 connected;
  str8_short name;
  u16 vendor_id;
  u16 product_id;
  u16 usage_page;
  u16 usage;
} device_info;

// Returns 1 if src refers to a concrete device, 0 otherwise.
func b32 device_id_is_valid(device_id src);

// Returns a stable display name for type.
func const c8* devices_get_type_name(device_type type);

// Returns the number of devices of type currently known to the backend.
func sz devices_get_count(device_type type);

// Writes the device id at index into out_id. Returns 1 on success, 0 otherwise.
func b32 devices_get_device(device_type type, sz index, device_id* out_id);

// Returns 1 if id is still connected, 0 otherwise.
func b32 devices_is_connected(device_id id);

// Fills out_info with the latest metadata for id. Returns 1 on success, 0 otherwise.
func b32 devices_get_info(device_id id, device_info* out_info);
