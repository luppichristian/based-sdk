// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/audio_device.h"
#include "input/devices.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "../sdl3_include.h"
#include <SDL3/SDL_hidapi.h>
#include "basic/profiler.h"
#include "basic/safe.h"

typedef struct device_handle_entry {
  device_type type;
  u64 instance;
} device_handle_entry;

static const u64 DEVICES_AUDIO_RECORDING_BIT = 1ULL << 63;
static const sz DEVICES_HANDLE_CAP = 1024;

global_var device_handle_entry device_handles[DEVICES_HANDLE_CAP] = {0};

func void devices_clear_name(c8* dst, sz capacity) {
  profile_func_begin;
  if (!dst || !capacity) {
    profile_func_end;
    return;
  }

  dst[0] = '\0';
  profile_func_end;
}

func void devices_cpy_cstring(c8* dst, sz capacity, cstr8 src) {
  profile_func_begin;
  sz idx = 0;

  if (!dst || !capacity) {
    profile_func_end;
    return;
  }

  if (!src) {
    devices_clear_name(dst, capacity);
    profile_func_end;
    return;
  }

  safe_while (src[idx] && (idx + 1) < capacity) {
    dst[idx] = src[idx];
    idx += 1;
  }

  dst[idx] = '\0';
  profile_func_end;
}

func void devices_cpy_wide_ascii(c8* dst, sz capacity, const wchar_t* src) {
  profile_func_begin;
  sz idx = 0;

  if (!dst || !capacity) {
    profile_func_end;
    return;
  }

  if (!src) {
    devices_clear_name(dst, capacity);
    profile_func_end;
    return;
  }

  safe_while (src[idx] && (idx + 1) < capacity) {
    wchar_t code_unit = src[idx];
    dst[idx] = (code_unit >= 1 && code_unit <= 0x7F) ? (c8)code_unit : '?';
    idx += 1;
  }

  dst[idx] = '\0';
  profile_func_end;
}

func device_handle_entry* devices_lookup_entry(device src) {
  if (!src) {
    return NULL;
  }

  safe_for (sz item_idx = 0; item_idx < DEVICES_HANDLE_CAP; item_idx += 1) {
    if ((device)&device_handles[item_idx] == src && device_handles[item_idx].type != DEVICE_TYPE_UNKNOWN &&
        device_handles[item_idx].instance != 0) {
      return &device_handles[item_idx];
    }
  }

  return NULL;
}

func device devices_make_id(device_type type, u64 instance) {
  profile_func_begin;
  if (type <= DEVICE_TYPE_UNKNOWN || type > DEVICE_TYPE_MONITOR || instance == 0) {
    profile_func_end;
    return NULL;
  }

  safe_for (sz item_idx = 0; item_idx < DEVICES_HANDLE_CAP; item_idx += 1) {
    if (device_handles[item_idx].type == type && device_handles[item_idx].instance == instance) {
      profile_func_end;
      return (device)&device_handles[item_idx];
    }
  }

  safe_for (sz item_idx = 0; item_idx < DEVICES_HANDLE_CAP; item_idx += 1) {
    if (device_handles[item_idx].type == DEVICE_TYPE_UNKNOWN && device_handles[item_idx].instance == 0) {
      device_handles[item_idx].type = type;
      device_handles[item_idx].instance = instance;
      profile_func_end;
      return (device)&device_handles[item_idx];
    }
  }

  thread_log_error("Device handle table is full type=%u instance=%llu",
                   (u32)type,
                   (unsigned long long)instance);
  profile_func_end;
  return NULL;
}

func u64 devices_audio_encode_instance(u64 native_id, audio_device_type audio_type) {
  profile_func_begin;
  u64 encoded = native_id & ~DEVICES_AUDIO_RECORDING_BIT;
  if (audio_type == AUDIO_DEVICE_TYPE_RECORDING) {
    encoded |= DEVICES_AUDIO_RECORDING_BIT;
  }
  profile_func_end;
  return encoded;
}

func u64 devices_audio_decode_native_id(u64 instance) {
  return instance & ~DEVICES_AUDIO_RECORDING_BIT;
}

func u64 devices_get_instance(device src) {
  device_handle_entry* handle = devices_lookup_entry(src);
  return handle ? handle->instance : 0;
}

func b32 device_is_valid(device src) {
  return devices_lookup_entry(src) != NULL;
}

func device_type devices_get_type(device src) {
  device_handle_entry* handle = devices_lookup_entry(src);
  return handle ? handle->type : DEVICE_TYPE_UNKNOWN;
}

func device devices_make_audio_device(u64 native_id, audio_device_type audio_type) {
  profile_func_begin;
  if (!audio_device_type_is_valid(audio_type)) {
    profile_func_end;
    return NULL;
  }

  profile_func_end;
  return devices_make_id(DEVICE_TYPE_AUDIO, devices_audio_encode_instance(native_id, audio_type));
}

func u64 devices_get_audio_native_id(device src) {
  profile_func_begin;
  if (devices_get_type(src) != DEVICE_TYPE_AUDIO) {
    profile_func_end;
    return 0;
  }

  profile_func_end;
  return devices_audio_decode_native_id(devices_get_instance(src));
}

func sz devices_get_audio_count_for_type(audio_device_type audio_type) {
  int count = 0;

  if (audio_type == AUDIO_DEVICE_TYPE_PLAYBACK) {
    SDL_AudioDeviceID* ids = SDL_GetAudioPlaybackDevices(&count);
    if (ids) {
      SDL_free(ids);
    }
    return count > 0 ? (sz)count : 0;
  }

  if (audio_type == AUDIO_DEVICE_TYPE_RECORDING) {
    SDL_AudioDeviceID* ids = SDL_GetAudioRecordingDevices(&count);
    if (ids) {
      SDL_free(ids);
    }
    return count > 0 ? (sz)count : 0;
  }

  return 0;
}

func device devices_get_audio_device_by_type(audio_device_type audio_type, sz idx) {
  profile_func_begin;
  int count = 0;
  SDL_AudioDeviceID* ids = NULL;
  device result = NULL;

  if (audio_type == AUDIO_DEVICE_TYPE_PLAYBACK) {
    ids = SDL_GetAudioPlaybackDevices(&count);
  } else if (audio_type == AUDIO_DEVICE_TYPE_RECORDING) {
    ids = SDL_GetAudioRecordingDevices(&count);
  } else {
    profile_func_end;
    return NULL;
  }

  if (ids != NULL && idx < (sz)count) {
    result = devices_make_audio_device((u64)ids[idx], audio_type);
  }

  if (ids) {
    SDL_free(ids);
  }

  profile_func_end;
  return result;
}

func b32 devices_try_fill_tablet_info(SDL_hid_device_info* entry, device_info* out_info) {
  profile_func_begin;
  if (!entry || entry->usage_page != 0x0D || !out_info) {
    profile_func_end;
    return false;
  }

  *out_info = (device_info) {0};
  out_info->id = devices_make_id(DEVICE_TYPE_TABLET, cstr8_hash64(entry->path));
  out_info->connected = 1;
  out_info->vendor_id = (u16)entry->vendor_id;
  out_info->product_id = (u16)entry->product_id;
  out_info->usage_page = (u16)entry->usage_page;
  out_info->usage = (u16)entry->usage;

  if (entry->product_string && entry->product_string[0]) {
    devices_cpy_wide_ascii(out_info->name, size_of(out_info->name), entry->product_string);
  } else if (entry->path) {
    devices_cpy_cstring(out_info->name, size_of(out_info->name), entry->path);
  }

  profile_func_end;
  return true;
}

func device devices_find_tablet_by_idx(sz idx) {
  profile_func_begin;
  SDL_hid_device_info* head = SDL_hid_enumerate(0, 0);
  SDL_hid_device_info* entry = head;
  sz current_idx = 0;
  device result = NULL;

  safe_while (entry) {
    if (entry->usage_page == 0x0D) {
      if (current_idx == idx) {
        result = devices_make_id(DEVICE_TYPE_TABLET, cstr8_hash64(entry->path));
        break;
      }

      current_idx += 1;
    }

    entry = entry->next;
  }

  if (head) {
    SDL_hid_free_enumeration(head);
  }

  profile_func_end;
  return result;
}

func b32 devices_find_tablet_info(device dev_id, device_info* out_info) {
  profile_func_begin;
  SDL_hid_device_info* head = SDL_hid_enumerate(0, 0);
  SDL_hid_device_info* entry = head;
  u64 instance = devices_get_instance(dev_id);
  b32 found = false;

  safe_while (entry) {
    if (entry->usage_page == 0x0D && cstr8_hash64(entry->path) == instance) {
      if (out_info) {
        found = devices_try_fill_tablet_info(entry, out_info);
      } else {
        found = 1;
      }
      break;
    }

    entry = entry->next;
  }

  if (head) {
    SDL_hid_free_enumeration(head);
  }

  profile_func_end;
  return found;
}

func cstr8 devices_get_type_name(device_type type) {
  profile_func_begin;
  switch (type) {
    case DEVICE_TYPE_KEYBOARD:
      profile_func_end;
      return "keyboard";
    case DEVICE_TYPE_MOUSE:
      profile_func_end;
      return "mouse";
    case DEVICE_TYPE_GAMEPAD:
      profile_func_end;
      return "gamepad";
    case DEVICE_TYPE_JOYSTICK:
      profile_func_end;
      return "joystick";
    case DEVICE_TYPE_TABLET:
      profile_func_end;
      return "tablet";
    case DEVICE_TYPE_TOUCH:
      profile_func_end;
      return "touch";
    case DEVICE_TYPE_AUDIO:
      profile_func_end;
      return "audio";
    case DEVICE_TYPE_CAMERA:
      profile_func_end;
      return "camera";
    case DEVICE_TYPE_SENSOR:
      profile_func_end;
      return "sensor";
    case DEVICE_TYPE_MONITOR:
      profile_func_end;
      return "monitor";
    case DEVICE_TYPE_UNKNOWN:
    default:
      profile_func_end;
      return "unknown";
  }
}

func sz devices_get_count(device_type type) {
  int count = 0;

  switch (type) {
    case DEVICE_TYPE_KEYBOARD: {
      SDL_KeyboardID* ids = SDL_GetKeyboards(&count);
      if (ids) {
        SDL_free(ids);
      }
      return count > 0 ? (sz)count : 0;
    }
    case DEVICE_TYPE_MOUSE: {
      SDL_MouseID* ids = SDL_GetMice(&count);
      if (ids) {
        SDL_free(ids);
      }
      return count > 0 ? (sz)count : 0;
    }
    case DEVICE_TYPE_GAMEPAD: {
      SDL_JoystickID* ids = SDL_GetGamepads(&count);
      if (ids) {
        SDL_free(ids);
      }
      return count > 0 ? (sz)count : 0;
    }
    case DEVICE_TYPE_JOYSTICK: {
      SDL_JoystickID* ids = SDL_GetJoysticks(&count);
      if (ids) {
        SDL_free(ids);
      }
      return count > 0 ? (sz)count : 0;
    }
    case DEVICE_TYPE_TOUCH: {
      SDL_TouchID* ids = SDL_GetTouchDevices(&count);
      if (ids) {
        SDL_free(ids);
      }
      return count > 0 ? (sz)count : 0;
    }
    case DEVICE_TYPE_TABLET: {
      SDL_hid_device_info* head = SDL_hid_enumerate(0, 0);
      SDL_hid_device_info* entry = head;
      sz total = 0;

      safe_while (entry) {
        if (entry->usage_page == 0x0D) {
          total += 1;
        }
        entry = entry->next;
      }

      if (head) {
        SDL_hid_free_enumeration(head);
      }

      return total;
    }
    case DEVICE_TYPE_AUDIO:
      return audio_device_get_total_count(AUDIO_DEVICE_TYPE_PLAYBACK) +
             audio_device_get_total_count(AUDIO_DEVICE_TYPE_RECORDING);
    case DEVICE_TYPE_CAMERA: {
      SDL_CameraID* ids = SDL_GetCameras(&count);
      if (ids) {
        SDL_free(ids);
      }
      return count > 0 ? (sz)count : 0;
    }
    case DEVICE_TYPE_SENSOR: {
      SDL_SensorID* ids = SDL_GetSensors(&count);
      if (ids) {
        SDL_free(ids);
      }
      return count > 0 ? (sz)count : 0;
    }
    case DEVICE_TYPE_MONITOR: {
      SDL_DisplayID* ids = SDL_GetDisplays(&count);
      if (ids) {
        SDL_free(ids);
      }
      return count > 0 ? (sz)count : 0;
    }
    case DEVICE_TYPE_UNKNOWN:
    default:
      return 0;
  }
}

func device devices_get_device(device_type type, sz idx) {
  profile_func_begin;
  int count = 0;

  switch (type) {
    case DEVICE_TYPE_KEYBOARD: {
      SDL_KeyboardID* ids = SDL_GetKeyboards(&count);
      device result = ids && idx < (sz)count ? devices_make_id(DEVICE_TYPE_KEYBOARD, (u64)ids[idx]) : NULL;
      if (ids) {
        SDL_free(ids);
      }
      profile_func_end;
      return result;
    }
    case DEVICE_TYPE_MOUSE: {
      SDL_MouseID* ids = SDL_GetMice(&count);
      device result = ids && idx < (sz)count ? devices_make_id(DEVICE_TYPE_MOUSE, (u64)ids[idx]) : NULL;
      if (ids) {
        SDL_free(ids);
      }
      profile_func_end;
      return result;
    }
    case DEVICE_TYPE_GAMEPAD: {
      SDL_JoystickID* ids = SDL_GetGamepads(&count);
      device result = ids && idx < (sz)count ? devices_make_id(DEVICE_TYPE_GAMEPAD, (u64)ids[idx]) : NULL;
      if (ids) {
        SDL_free(ids);
      }
      profile_func_end;
      return result;
    }
    case DEVICE_TYPE_JOYSTICK: {
      SDL_JoystickID* ids = SDL_GetJoysticks(&count);
      device result = ids && idx < (sz)count ? devices_make_id(DEVICE_TYPE_JOYSTICK, (u64)ids[idx]) : NULL;
      if (ids) {
        SDL_free(ids);
      }
      profile_func_end;
      return result;
    }
    case DEVICE_TYPE_TOUCH: {
      SDL_TouchID* ids = SDL_GetTouchDevices(&count);
      device result = ids && idx < (sz)count ? devices_make_id(DEVICE_TYPE_TOUCH, (u64)ids[idx]) : NULL;
      if (ids) {
        SDL_free(ids);
      }
      profile_func_end;
      return result;
    }
    case DEVICE_TYPE_TABLET:
      profile_func_end;
      return devices_find_tablet_by_idx(idx);
    case DEVICE_TYPE_AUDIO: {
      sz playback_count = audio_device_get_total_count(AUDIO_DEVICE_TYPE_PLAYBACK);
      profile_func_end;
      return idx < playback_count ? devices_get_audio_device_by_type(AUDIO_DEVICE_TYPE_PLAYBACK, idx)
                                  : devices_get_audio_device_by_type(AUDIO_DEVICE_TYPE_RECORDING, idx - playback_count);
    }
    case DEVICE_TYPE_CAMERA: {
      SDL_CameraID* ids = SDL_GetCameras(&count);
      device result = ids && idx < (sz)count ? devices_make_id(DEVICE_TYPE_CAMERA, (u64)ids[idx]) : NULL;
      if (ids) {
        SDL_free(ids);
      }
      profile_func_end;
      return result;
    }
    case DEVICE_TYPE_SENSOR: {
      SDL_SensorID* ids = SDL_GetSensors(&count);
      device result = ids && idx < (sz)count ? devices_make_id(DEVICE_TYPE_SENSOR, (u64)ids[idx]) : NULL;
      if (ids) {
        SDL_free(ids);
      }
      profile_func_end;
      return result;
    }
    case DEVICE_TYPE_MONITOR: {
      SDL_DisplayID* ids = SDL_GetDisplays(&count);
      device result = ids && idx < (sz)count ? devices_make_id(DEVICE_TYPE_MONITOR, (u64)ids[idx]) : NULL;
      if (ids) {
        SDL_free(ids);
      }
      profile_func_end;
      return result;
    }
    case DEVICE_TYPE_UNKNOWN:
    default:
      profile_func_end;
      return NULL;
  }
}

func b32 devices_is_connected(device dev_id) {
  return devices_get_info(dev_id, NULL);
}

func b32 devices_get_info(device dev_id, device_info* out_info) {
  profile_func_begin;
  int count = 0;
  device_type type = devices_get_type(dev_id);
  u64 instance = devices_get_instance(dev_id);

  if (!device_is_valid(dev_id)) {
    profile_func_end;
    return false;
  }

  if (out_info) {
    *out_info = (device_info) {0};
    out_info->id = dev_id;
  }

  switch (type) {
    case DEVICE_TYPE_KEYBOARD: {
      SDL_KeyboardID* ids = SDL_GetKeyboards(&count);
      b32 found = false;

      safe_for (int idx = 0; ids && idx < count; idx += 1) {
        if ((u64)ids[idx] == instance) {
          found = 1;
          if (out_info) {
            out_info->connected = 1;
            devices_cpy_cstring(out_info->name, size_of(out_info->name), SDL_GetKeyboardNameForID(ids[idx]));
          }
          break;
        }
      }

      if (ids) {
        SDL_free(ids);
      }

      profile_func_end;
      return found;
    }
    case DEVICE_TYPE_MOUSE: {
      SDL_MouseID* ids = SDL_GetMice(&count);
      b32 found = false;

      safe_for (int idx = 0; ids && idx < count; idx += 1) {
        if ((u64)ids[idx] == instance) {
          found = 1;
          if (out_info) {
            out_info->connected = 1;
            devices_cpy_cstring(out_info->name, size_of(out_info->name), SDL_GetMouseNameForID(ids[idx]));
          }
          break;
        }
      }

      if (ids) {
        SDL_free(ids);
      }

      profile_func_end;
      return found;
    }
    case DEVICE_TYPE_GAMEPAD: {
      SDL_JoystickID* ids = SDL_GetGamepads(&count);
      b32 found = false;

      safe_for (int idx = 0; ids && idx < count; idx += 1) {
        if ((u64)ids[idx] == instance) {
          found = 1;
          if (out_info) {
            out_info->connected = 1;
            devices_cpy_cstring(out_info->name, size_of(out_info->name), SDL_GetGamepadNameForID(ids[idx]));
          }
          break;
        }
      }

      if (ids) {
        SDL_free(ids);
      }

      profile_func_end;
      return found;
    }
    case DEVICE_TYPE_JOYSTICK: {
      SDL_JoystickID* ids = SDL_GetJoysticks(&count);
      b32 found = false;

      safe_for (int idx = 0; ids && idx < count; idx += 1) {
        if ((u64)ids[idx] == instance) {
          found = 1;
          if (out_info) {
            out_info->connected = 1;
            devices_cpy_cstring(out_info->name, size_of(out_info->name), SDL_GetJoystickNameForID(ids[idx]));
          }
          break;
        }
      }

      if (ids) {
        SDL_free(ids);
      }

      profile_func_end;
      return found;
    }
    case DEVICE_TYPE_TOUCH: {
      SDL_TouchID* ids = SDL_GetTouchDevices(&count);
      b32 found = false;

      safe_for (int idx = 0; ids && idx < count; idx += 1) {
        if ((u64)ids[idx] == instance) {
          found = 1;
          if (out_info) {
            out_info->connected = 1;
            devices_cpy_cstring(out_info->name, size_of(out_info->name), SDL_GetTouchDeviceName(ids[idx]));
            out_info->usage = (u16)SDL_GetTouchDeviceType(ids[idx]);
          }
          break;
        }
      }

      if (ids) {
        SDL_free(ids);
      }

      profile_func_end;
      return found;
    }
    case DEVICE_TYPE_TABLET:
      profile_func_end;
      return devices_find_tablet_info(dev_id, out_info);
    case DEVICE_TYPE_AUDIO: {
      int audio_count = 0;
      u64 native_id = devices_audio_decode_native_id(instance);
      audio_device_type audio_type = (instance & DEVICES_AUDIO_RECORDING_BIT) != 0 ? AUDIO_DEVICE_TYPE_RECORDING
                                                                                   : AUDIO_DEVICE_TYPE_PLAYBACK;
      SDL_AudioDeviceID* ids = NULL;
      b32 found = false;

      if (audio_type == AUDIO_DEVICE_TYPE_PLAYBACK) {
        ids = SDL_GetAudioPlaybackDevices(&audio_count);
      } else if (audio_type == AUDIO_DEVICE_TYPE_RECORDING) {
        ids = SDL_GetAudioRecordingDevices(&audio_count);
      } else {
        profile_func_end;
        return false;
      }

      safe_for (int idx = 0; ids && idx < audio_count; idx += 1) {
        if ((u64)ids[idx] == native_id) {
          found = 1;
          if (out_info) {
            out_info->connected = 1;
            out_info->usage = (u16)audio_type;
            devices_cpy_cstring(out_info->name, size_of(out_info->name), SDL_GetAudioDeviceName(ids[idx]));
          }
          break;
        }
      }

      if (ids) {
        SDL_free(ids);
      }

      profile_func_end;
      return found;
    }
    case DEVICE_TYPE_CAMERA: {
      SDL_CameraID* ids = SDL_GetCameras(&count);
      b32 found = false;

      safe_for (int idx = 0; ids && idx < count; idx += 1) {
        if ((u64)ids[idx] == instance) {
          found = 1;
          if (out_info) {
            out_info->connected = 1;
            devices_cpy_cstring(out_info->name, size_of(out_info->name), SDL_GetCameraName(ids[idx]));
            out_info->usage = (u16)SDL_GetCameraPosition(ids[idx]);
          }
          break;
        }
      }

      if (ids) {
        SDL_free(ids);
      }

      profile_func_end;
      return found;
    }
    case DEVICE_TYPE_SENSOR: {
      SDL_SensorID* ids = SDL_GetSensors(&count);
      b32 found = false;

      safe_for (int idx = 0; ids && idx < count; idx += 1) {
        if ((u64)ids[idx] == instance) {
          found = 1;
          if (out_info) {
            out_info->connected = 1;
            devices_cpy_cstring(out_info->name, size_of(out_info->name), SDL_GetSensorNameForID(ids[idx]));
            out_info->usage = (u16)SDL_GetSensorTypeForID(ids[idx]);
          }
          break;
        }
      }

      if (ids) {
        SDL_free(ids);
      }

      profile_func_end;
      return found;
    }
    case DEVICE_TYPE_MONITOR: {
      SDL_DisplayID* ids = SDL_GetDisplays(&count);
      b32 found = false;

      safe_for (int idx = 0; ids && idx < count; idx += 1) {
        if ((u64)ids[idx] == instance) {
          found = 1;
          if (out_info) {
            out_info->connected = 1;
            devices_cpy_cstring(out_info->name, size_of(out_info->name), SDL_GetDisplayName(ids[idx]));
          }
          break;
        }
      }

      if (ids) {
        SDL_free(ids);
      }

      profile_func_end;
      return found;
    }
    case DEVICE_TYPE_UNKNOWN:
    default:
      profile_func_end;
      return false;
  }
}
