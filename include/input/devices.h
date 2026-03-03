// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// =========================================================================
// Input Devices
// =========================================================================

// Maximum UTF-8 byte count stored in input_device_info.name, including the terminator.
#define INPUT_DEVICE_NAME_CAPACITY 128

// Stable device categories shared across the input modules.
typedef enum input_device_type {
  INPUT_DEVICE_TYPE_UNKNOWN = 0,
  INPUT_DEVICE_TYPE_KEYBOARD = 1,
  INPUT_DEVICE_TYPE_MOUSE = 2,
  INPUT_DEVICE_TYPE_GAMEPAD = 3,
  INPUT_DEVICE_TYPE_TABLET = 4,
  INPUT_DEVICE_TYPE_TOUCH = 5,
} input_device_type;

// Backend-defined device identifier.
typedef struct input_device_id {
  input_device_type type;
  u64 instance;
} input_device_id;

// Snapshot of the metadata currently known for an input device.
typedef struct input_device_info {
  input_device_id id;
  b32 connected;
  c8 name[INPUT_DEVICE_NAME_CAPACITY];
  u16 vendor_id;
  u16 product_id;
  u16 usage_page;
  u16 usage;
} input_device_info;

// Returns 1 if src refers to a concrete device, 0 otherwise.
func b32 input_device_id_is_valid(input_device_id src);

// Returns a stable display name for type.
func const c8* devices_get_type_name(input_device_type type);

// Returns the number of devices of type currently known to the backend.
func sz devices_get_count(input_device_type type);

// Writes the device id at index into out_id. Returns 1 on success, 0 otherwise.
func b32 devices_get_device(input_device_type type, sz index, input_device_id* out_id);

// Returns 1 if id is still connected, 0 otherwise.
func b32 devices_is_connected(input_device_id id);

// Fills out_info with the latest metadata for id. Returns 1 on success, 0 otherwise.
func b32 devices_get_info(input_device_id id, input_device_info* out_info);
