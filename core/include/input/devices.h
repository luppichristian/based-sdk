// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../strings/cstrings.h"

#ifndef BASED_CORE_OPAQUE_DEVICE_HANDLES_DEFINED
#  define BASED_CORE_OPAQUE_DEVICE_HANDLES_DEFINED
typedef void* device;
typedef void* camera;
typedef void* sensor;
typedef void* monitor;
#endif

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

typedef enum battery_state {
  BATTERY_STATE_UNKNOWN = 0,
  BATTERY_STATE_ERROR = 1,
  BATTERY_STATE_ON_BATTERY = 2,
  BATTERY_STATE_NO_BATTERY = 3,
  BATTERY_STATE_CHARGING = 4,
  BATTERY_STATE_CHARGED = 5,
} battery_state;

typedef enum audio_device_type {
  AUDIO_DEVICE_TYPE_UNKNOWN = 0,
  AUDIO_DEVICE_TYPE_PLAYBACK = 1,
  AUDIO_DEVICE_TYPE_RECORDING = 2,
} audio_device_type;

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

typedef enum joystick_hat_state {
  JOYSTICK_HAT_STATE_CENTERED = 0x00,
  JOYSTICK_HAT_STATE_UP = 0x01,
  JOYSTICK_HAT_STATE_RIGHT = 0x02,
  JOYSTICK_HAT_STATE_DOWN = 0x04,
  JOYSTICK_HAT_STATE_LEFT = 0x08,
  JOYSTICK_HAT_STATE_RIGHT_UP = 0x03,
  JOYSTICK_HAT_STATE_RIGHT_DOWN = 0x06,
  JOYSTICK_HAT_STATE_LEFT_UP = 0x09,
  JOYSTICK_HAT_STATE_LEFT_DOWN = 0x0C,
} joystick_hat_state;

// Snapshot of the metadata currently known for an input device.
typedef struct device_info {
  device id;
  b32 connected;
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

// Returns 1 when src is a valid audio device direction, 0 otherwise.
func b32 audio_device_type_is_valid(audio_device_type src);

// Returns a stable display name for type.
func cstr8 devices_get_type_name(device_type type);

// Returns a stable display name for audio_type.
func cstr8 devices_get_audio_type_name(audio_device_type audio_type);

// Returns the number of devices of type currently known to the backend.
func sz devices_get_count(device_type type);

// Returns the device at idx, or NULL when idx is unavailable.
func device devices_get_device(device_type type, sz idx);

// Returns the audio device at idx for audio_type, or NULL when idx is unavailable.
func device devices_get_audio_device(audio_device_type audio_type, sz idx);

// Returns the encoded audio device direction for id.
func audio_device_type devices_get_audio_device_type(device dev_id);

// Converts src into a camera handle when it refers to a camera device.
func camera camera_from_device(device src);

// Converts src into a sensor handle when it refers to a sensor device.
func sensor sensor_from_device(device src);

// Converts src into a monitor handle when it refers to a monitor device.
func monitor monitor_from_device(device src);

// Returns 1 if id is still connected, 0 otherwise.
func b32 devices_is_connected(device dev_id);

// Fills out_info with the latest metadata for id. Returns 1 on success, 0 otherwise.
func b32 devices_get_info(device dev_id, device_info* out_info);

// =========================================================================
c_end;
// =========================================================================
