// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../strings/cstrings.h"

typedef void* device;

typedef enum battery_state {
  BATTERY_STATE_UNKNOWN = 0,
  BATTERY_STATE_ERROR = 1,
  BATTERY_STATE_ON_BATTERY = 2,
  BATTERY_STATE_NO_BATTERY = 3,
  BATTERY_STATE_CHARGING = 4,
  BATTERY_STATE_CHARGED = 5,
} battery_state;

#define DEVICES_HANDLE_CAP ((sz)1024)

// =========================================================================
c_begin;
// =========================================================================

// =========================================================================
// Input Devices
// =========================================================================

// Stable device categories shared across the input modules.
typedef enum device_type {
  DEVICE_TYPE_UNKNOWN = 0,
  DEVICE_TYPE_KEYBOARD = 1,
  DEVICE_TYPE_MOUSE = 2,
  DEVICE_TYPE_GAMEPAD = 3,
  DEVICE_TYPE_JOYSTICK = 4,
  DEVICE_TYPE_TABLET = 5,
  DEVICE_TYPE_TOUCH = 6,
  DEVICE_TYPE_AUDIO = 7,
  DEVICE_TYPE_CAMERA = 8,
  DEVICE_TYPE_SENSOR = 9,
  DEVICE_TYPE_MONITOR = 10,
} device_type;

// Snapshot of the metadata currently known for an input device.
typedef struct device_info {
  device id;
  b32 connected;
  u64 device_state;
  battery_state battery_state;
  void* handle;
  str8_short name;
  u16 vendor_id;
  u16 product_id;
  u16 usage_page;
  u16 usage;
} device_info;

// Returns 1 if src refers to a concrete device, 0 otherwise.
func b32 device_is_valid(device src);

// Returns the stable device category encoded by src.
func device_type devices_get_type(device src);

// Returns the backend/native handle currently associated with src.
func void* device_get_handle(device src);

// Returns the lifecycle state value for src.
func u64 device_get_state(device src);

// Returns a stable display name for type.
func cstr8 devices_get_type_name(device_type type);

// Returns the number of devices of type currently known to the backend.
func sz devices_get_count(device_type type);

// Returns the number of devices of type currently known to the backend.
func u32 devices_get_type_count(device_type type);

// Returns the device at idx, or NULL when idx is unavailable.
func device devices_get_device(device_type type, sz idx);

// Returns 1 if id is still connected, 0 otherwise.
func b32 devices_is_connected(device dev_id);

// Fills out_info with the latest metadata for id. Returns 1 on success, 0 otherwise.
func b32 devices_get_info(device dev_id, device_info* out_info);

// =========================================================================
c_end;
// =========================================================================
