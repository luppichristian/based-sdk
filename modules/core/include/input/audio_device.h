// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "devices.h"

typedef void* audio_device;

// =========================================================================
c_begin;
// =========================================================================

typedef enum audio_device_type {
  AUDIO_DEVICE_TYPE_UNKNOWN = 0,
  AUDIO_DEVICE_TYPE_PLAYBACK = 1,
  AUDIO_DEVICE_TYPE_RECORDING = 2,
} audio_device_type;

// Returns 1 if src refers to a concrete audio device id, 0 otherwise.
func b32 audio_device_is_valid(audio_device src);

// Converts src into an audio device handle when it refers to an audio device.
func audio_device device_get_audio_device(device src);

// Converts src into a generic device handle.
func device audio_device_to_device(audio_device src);

// Returns 1 when src is a valid audio device direction, 0 otherwise.
func b32 audio_device_type_is_valid(audio_device_type src);

// Returns a stable display name for audio_type.
func cstr8 audio_device_get_type_name(audio_device_type audio_type);

// Returns the number of currently known audio devices for audio_type.
func sz audio_device_get_total_count(audio_device_type audio_type);

// Writes the audio device id at idx into out_id. Returns 1 on success, 0 otherwise.
func audio_device audio_device_get_from_idx(audio_device_type audio_type, sz idx);

// Returns the primary audio device for audio_type, or NULL when unavailable.
func audio_device audio_device_get_primary(audio_device_type audio_type);

// Returns the focused audio device for audio_type, or the primary device when unavailable.
func audio_device audio_device_get_focused(audio_device_type audio_type);

// Returns the encoded audio device direction for id.
func audio_device_type audio_device_get_type(audio_device aud_id);

// Returns a backend-defined audio device name for id, or NULL when unavailable.
func cstr8 audio_device_get_name(audio_device aud_id);

// =========================================================================
c_end;
// =========================================================================
