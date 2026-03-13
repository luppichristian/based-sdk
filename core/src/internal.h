// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../include/input/camera.h"
#include "../include/input/devices.h"
#include "../include/input/msg.h"
#include "../include/input/sensor.h"
#include "../include/interface/monitor.h"
#include "../include/interface/window.h"

// This header is internal to the core module and is not part of the public API.

// =========================================================================
c_begin;
// =========================================================================

func camera camera_from_native_id(up native_id);
func up camera_to_native_id(camera src);

func device devices_make_id(device_type type, u64 instance);
func device devices_make_audio_device(u64 native_id, audio_device_type audio_type);
func u64 devices_get_instance(device src);
func u64 devices_get_audio_native_id(device src);

func b32 msg_from_native(const void* native_event, msg* out_msg);
func b32 msg_to_native(const msg* src, void* native_event);

func sensor sensor_from_native_id(up native_id);
func up sensor_to_native_id(sensor src);

func monitor monitor_from_native_id(up native_id);
func up monitor_to_native_id(monitor src);

func window window_from_native_id(up native_id);
func up window_to_native_id(window src);

// =========================================================================
c_end;
// =========================================================================
