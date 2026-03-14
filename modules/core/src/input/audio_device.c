// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/audio_device.h"
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
  return (u64)(up)src;
}

func b32 audio_device_is_valid(audio_device src) {
  return src != NULL;
}

func audio_device audio_device_from_device(device src) {
  if (devices_get_type(src) != DEVICE_TYPE_AUDIO) {
    return NULL;
  }

  return (audio_device)(up)devices_get_instance(src);
}

func audio_device audio_device_from_native_id(up native_id, audio_device_type audio_type) {
  profile_func_begin;
  if (!audio_device_type_is_valid(audio_type) || native_id == 0) {
    profile_func_end;
    return NULL;
  }

  profile_func_end;
  return (audio_device)(up)audio_device_encode_instance((u64)native_id, audio_type);
}

func up audio_device_to_native_id(audio_device src) {
  if (!audio_device_is_valid(src)) {
    return 0;
  }

  return (up)audio_device_decode_native_id(audio_device_get_instance(src));
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

func b32 audio_device_get_id(audio_device_type audio_type, sz idx, audio_device* out_id) {
  profile_func_begin;
  int count = 0;
  SDL_AudioDeviceID* ids = NULL;
  b32 found = false;

  if (out_id) {
    *out_id = NULL;
  }

  if (audio_type == AUDIO_DEVICE_TYPE_PLAYBACK) {
    ids = SDL_GetAudioPlaybackDevices(&count);
  } else if (audio_type == AUDIO_DEVICE_TYPE_RECORDING) {
    ids = SDL_GetAudioRecordingDevices(&count);
  } else {
    profile_func_end;
    return false;
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

  if (found && out_id) {
    *out_id = audio_device_from_native_id((up)ids[idx], audio_type);
  }

  if (ids) {
    SDL_free(ids);
  }

  profile_func_end;
  return found;
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
