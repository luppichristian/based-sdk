// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/audio_device.h"
#include "basic/assert.h"
#include "../internal.h"
#include "context/thread_ctx.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

static const u64 AUDIO_DEVICE_RECORDING_BIT = 1ULL << 63;

func u64 audio_device_encode_instance(u64 native_id, audio_device_type audio_type) {
  profile_func_begin;
  u64 encoded = native_id & ~AUDIO_DEVICE_RECORDING_BIT;
  if (audio_type == AUDIO_DEVICE_TYPE_RECORDING) {
    encoded |= AUDIO_DEVICE_RECORDING_BIT;
  }
  profile_func_end;
  return encoded;
}

func u64 audio_device_decode_native_id(u64 instance) {
  return instance & ~AUDIO_DEVICE_RECORDING_BIT;
}

func u64 audio_device_get_instance(audio_device src) {
  return devices_get_instance(audio_device_to_device(src));
}

func b32 audio_device_is_valid(audio_device src) {
  return src != NULL && devices_get_type((device)src) == DEVICE_TYPE_AUDIO;
}

func audio_device device_get_audio_device(device src) {
  if (devices_get_type(src) != DEVICE_TYPE_AUDIO) {
    invalid_code_path;
    return NULL;
  }

  return (audio_device)src;
}

func audio_device audio_device_from_native_id(up native_id, audio_device_type audio_type) {
  profile_func_begin;
  if (!audio_device_type_is_valid(audio_type) || native_id == 0) {
    profile_func_end;
    return NULL;
  }

  profile_func_end;
  return device_get_audio_device(devices_make_audio_device((u64)native_id, audio_type));
}

func up audio_device_to_native_id(audio_device src) {
  if (!audio_device_is_valid(src)) {
    return 0;
  }

  return (up)audio_device_decode_native_id(audio_device_get_instance(src));
}

func device audio_device_to_device(audio_device src) {
  if (!audio_device_is_valid(src)) {
    invalid_code_path;
    return NULL;
  }

  return (device)src;
}

func b32 audio_device_type_is_valid(audio_device_type src) {
  return src == AUDIO_DEVICE_TYPE_PLAYBACK || src == AUDIO_DEVICE_TYPE_RECORDING;
}

func cstr8 audio_device_get_type_name(audio_device_type audio_type) {
  profile_func_begin;
  switch (audio_type) {
    case AUDIO_DEVICE_TYPE_PLAYBACK:
      profile_func_end;
      return "playback";
    case AUDIO_DEVICE_TYPE_RECORDING:
      profile_func_end;
      return "recording";
    case AUDIO_DEVICE_TYPE_UNKNOWN:
    default:
      profile_func_end;
      return "unknown";
  }
}

func sz audio_device_get_total_count(audio_device_type audio_type) {
  profile_func_begin;
  int count = 0;

  if (audio_type == AUDIO_DEVICE_TYPE_PLAYBACK) {
    SDL_AudioDeviceID* ids = SDL_GetAudioPlaybackDevices(&count);
    if (ids) {
      SDL_free(ids);
    }
    profile_func_end;
    return count > 0 ? (sz)count : 0;
  }

  if (audio_type == AUDIO_DEVICE_TYPE_RECORDING) {
    SDL_AudioDeviceID* ids = SDL_GetAudioRecordingDevices(&count);
    if (ids) {
      SDL_free(ids);
    }
    profile_func_end;
    return count > 0 ? (sz)count : 0;
  }

  profile_func_end;
  return 0;
}

func audio_device audio_device_get_from_idx(audio_device_type audio_type, sz idx) {
  profile_func_begin;
  int count = 0;
  SDL_AudioDeviceID* ids = NULL;
  b32 found = false;
  audio_device out_id = NULL;

  if (audio_type == AUDIO_DEVICE_TYPE_PLAYBACK) {
    ids = SDL_GetAudioPlaybackDevices(&count);
  } else if (audio_type == AUDIO_DEVICE_TYPE_RECORDING) {
    ids = SDL_GetAudioRecordingDevices(&count);
  } else {
    profile_func_end;
    return NULL;
  }

  found = ids != NULL && idx < (sz)count;
  if (!found && ids == NULL && count != 0) {
    thread_log_error("Failed to enumerate %s audio devices idx=%zu error=%s",
                     audio_device_get_type_name(audio_type),
                     (size_t)idx,
                     SDL_GetError());
  } else if (!found) {
    thread_log_warn("Audio device id lookup missed type=%s idx=%zu count=%d",
                    audio_device_get_type_name(audio_type),
                    (size_t)idx,
                    count);
  }

  if (found) {
    out_id = audio_device_from_native_id((up)ids[idx], audio_type);
    devices_update_runtime(audio_device_to_device(out_id), 1, (void*)(up)ids[idx]);
  }

  if (ids) {
    SDL_free(ids);
  }

  profile_func_end;
  return out_id;
}

func audio_device audio_device_get_primary(audio_device_type audio_type) {
  return audio_device_get_from_idx(audio_type, 0);
}

func audio_device audio_device_get_focused(audio_device_type audio_type) {
  device src = devices_get_focused_device(DEVICE_TYPE_AUDIO);
  if (!device_is_valid(src)) {
    return audio_device_get_primary(audio_type);
  }

  audio_device result = device_get_audio_device(src);
  return audio_device_get_type(result) == audio_type ? result : audio_device_get_primary(audio_type);
}

func audio_device_type audio_device_get_type(audio_device aud_id) {
  if (!audio_device_is_valid(aud_id)) {
    return AUDIO_DEVICE_TYPE_UNKNOWN;
  }

  return (audio_device_get_instance(aud_id) & AUDIO_DEVICE_RECORDING_BIT) != 0 ? AUDIO_DEVICE_TYPE_RECORDING
                                                                               : AUDIO_DEVICE_TYPE_PLAYBACK;
}

func cstr8 audio_device_get_name(audio_device aud_id) {
  profile_func_begin;
  if (!audio_device_is_valid(aud_id)) {
    thread_log_warn("Rejected audio device name query for invalid audio device");
    profile_func_end;
    return NULL;
  }

  cstr8 result = SDL_GetAudioDeviceName((SDL_AudioDeviceID)audio_device_to_native_id(aud_id));
  if (result == NULL) {
    thread_log_warn("Audio device name is unavailable audio_device=%llu",
                    (unsigned long long)audio_device_to_native_id(aud_id));
  }
  profile_func_end;
  return result;
}
