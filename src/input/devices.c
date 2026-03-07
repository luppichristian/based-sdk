// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/devices.h"
#include "basic/assert.h"
#include "../sdl3_include.h"
#include <SDL3/SDL_hidapi.h>
#include "basic/utility_defines.h"

const_var u64 DEVICES_AUDIO_RECORDING_BIT = 1ull << 63;

func void devices_clear_name(c8* dst, sz capacity) {
  if (!dst || !capacity) {
    return;
  }

  dst[0] = '\0';
}

func void devices_copy_cstring(c8* dst, sz capacity, cstr8 src) {
  sz idx = 0;

  if (!dst || !capacity) {
    return;
  }

  if (!src) {
    devices_clear_name(dst, capacity);
    return;
  }

  while (src[idx] && (idx + 1) < capacity) {
    dst[idx] = src[idx];
    idx += 1;
  }

  dst[idx] = '\0';
}

func void devices_copy_wide_ascii(c8* dst, sz capacity, const wchar_t* src) {
  sz idx = 0;

  if (!dst || !capacity) {
    return;
  }

  if (!src) {
    devices_clear_name(dst, capacity);
    return;
  }

  while (src[idx] && (idx + 1) < capacity) {
    wchar_t code_unit = src[idx];
    dst[idx] = (code_unit >= 1 && code_unit <= 0x7F) ? (c8)code_unit : '?';
    idx += 1;
  }

  dst[idx] = '\0';
}

func u64 devices_hash_path(cstr8 src) {
  u64 hash_value = 1469598103934665603ULL;
  sz idx = 0;

  if (!src) {
    return 0;
  }

  while (src[idx]) {
    hash_value ^= (u8)src[idx];
    hash_value *= 1099511628211ULL;
    idx += 1;
  }

  return hash_value;
}

func device_id devices_make_id(device_type type, u64 instance) {
  device_id result = {0};
  assert(type >= DEVICE_TYPE_UNKNOWN && type <= DEVICE_TYPE_AUDIO);
  result.type = type;
  result.instance = instance;
  return result;
}

func u64 devices_audio_encode_instance(u64 native_id, audio_device_type audio_type) {
  u64 encoded = native_id & ~DEVICES_AUDIO_RECORDING_BIT;
  if (audio_type == AUDIO_DEVICE_TYPE_RECORDING) {
    encoded |= DEVICES_AUDIO_RECORDING_BIT;
  }
  return encoded;
}

func u64 devices_audio_decode_native_id(u64 instance) {
  return instance & ~DEVICES_AUDIO_RECORDING_BIT;
}

func b32 audio_device_type_is_valid(audio_device_type src) {
  return src == AUDIO_DEVICE_TYPE_PLAYBACK || src == AUDIO_DEVICE_TYPE_RECORDING;
}

func cstr8 devices_get_audio_type_name(audio_device_type audio_type) {
  switch (audio_type) {
    case AUDIO_DEVICE_TYPE_PLAYBACK:
      return "playback";
    case AUDIO_DEVICE_TYPE_RECORDING:
      return "recording";
    case AUDIO_DEVICE_TYPE_UNKNOWN:
    default:
      return "unknown";
  }
}

func audio_device_type devices_get_audio_device_type(device_id id) {
  if (id.type != DEVICE_TYPE_AUDIO) {
    return AUDIO_DEVICE_TYPE_UNKNOWN;
  }

  return (id.instance & DEVICES_AUDIO_RECORDING_BIT) != 0 ? AUDIO_DEVICE_TYPE_RECORDING : AUDIO_DEVICE_TYPE_PLAYBACK;
}

func device_id devices_make_audio_device_id(u64 native_id, audio_device_type audio_type) {
  if (!audio_device_type_is_valid(audio_type)) {
    return (device_id) {0};
  }

  return devices_make_id(DEVICE_TYPE_AUDIO, devices_audio_encode_instance(native_id, audio_type));
}

func u64 devices_get_audio_native_id(device_id id) {
  if (id.type != DEVICE_TYPE_AUDIO) {
    return 0;
  }

  return devices_audio_decode_native_id(id.instance);
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

func b32 devices_get_audio_device(audio_device_type audio_type, sz idx, device_id* out_id) {
  int count = 0;
  SDL_AudioDeviceID* ids = NULL;

  if (out_id) {
    *out_id = (device_id) {0};
  }

  if (audio_type == AUDIO_DEVICE_TYPE_PLAYBACK) {
    ids = SDL_GetAudioPlaybackDevices(&count);
  } else if (audio_type == AUDIO_DEVICE_TYPE_RECORDING) {
    ids = SDL_GetAudioRecordingDevices(&count);
  } else {
    return 0;
  }

  b32 found = ids != NULL && idx < (sz)count;

  if (found && out_id) {
    *out_id = devices_make_audio_device_id((u64)ids[idx], audio_type);
  }

  if (ids) {
    SDL_free(ids);
  }

  return found;
}

func b32 devices_try_fill_tablet_info(SDL_hid_device_info* entry, device_info* out_info) {
  if (!entry || entry->usage_page != 0x0D || !out_info) {
    return 0;
  }

  *out_info = (device_info) {0};
  out_info->id = devices_make_id(DEVICE_TYPE_TABLET, devices_hash_path(entry->path));
  out_info->connected = 1;
  out_info->vendor_id = (u16)entry->vendor_id;
  out_info->product_id = (u16)entry->product_id;
  out_info->usage_page = (u16)entry->usage_page;
  out_info->usage = (u16)entry->usage;

  if (entry->product_string && entry->product_string[0]) {
    devices_copy_wide_ascii(out_info->name, size_of(out_info->name), entry->product_string);
  } else if (entry->path) {
    devices_copy_cstring(out_info->name, size_of(out_info->name), entry->path);
  }

  return 1;
}

func b32 devices_find_tablet_by_idx(sz idx, device_id* out_id) {
  SDL_hid_device_info* head = SDL_hid_enumerate(0, 0);
  SDL_hid_device_info* entry = head;
  sz current_idx = 0;
  b32 found = 0;

  while (entry) {
    if (entry->usage_page == 0x0D) {
      if (current_idx == idx) {
        if (out_id) {
          *out_id = devices_make_id(DEVICE_TYPE_TABLET, devices_hash_path(entry->path));
        }
        found = 1;
        break;
      }

      current_idx += 1;
    }

    entry = entry->next;
  }

  if (head) {
    SDL_hid_free_enumeration(head);
  }

  return found;
}

func b32 devices_find_tablet_info(device_id id, device_info* out_info) {
  SDL_hid_device_info* head = SDL_hid_enumerate(0, 0);
  SDL_hid_device_info* entry = head;
  b32 found = 0;

  while (entry) {
    if (entry->usage_page == 0x0D && devices_hash_path(entry->path) == id.instance) {
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

  return found;
}

func b32 device_id_is_valid(device_id src) {
  return src.type != DEVICE_TYPE_UNKNOWN && src.instance != 0;
}

func cstr8 devices_get_type_name(device_type type) {
  switch (type) {
    case DEVICE_TYPE_KEYBOARD:
      return "keyboard";
    case DEVICE_TYPE_MOUSE:
      return "mouse";
    case DEVICE_TYPE_GAMEPAD:
      return "gamepad";
    case DEVICE_TYPE_JOYSTICK:
      return "joystick";
    case DEVICE_TYPE_TABLET:
      return "tablet";
    case DEVICE_TYPE_TOUCH:
      return "touch";
    case DEVICE_TYPE_AUDIO:
      return "audio";
    case DEVICE_TYPE_UNKNOWN:
    default:
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

      while (entry) {
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
      return devices_get_audio_count_for_type(AUDIO_DEVICE_TYPE_PLAYBACK) +
             devices_get_audio_count_for_type(AUDIO_DEVICE_TYPE_RECORDING);
    case DEVICE_TYPE_UNKNOWN:
    default:
      return 0;
  }
}

func b32 devices_get_device(device_type type, sz idx, device_id* out_id) {
  int count = 0;

  if (out_id) {
    *out_id = (device_id) {0};
  }

  switch (type) {
    case DEVICE_TYPE_KEYBOARD: {
      SDL_KeyboardID* ids = SDL_GetKeyboards(&count);
      b32 found = ids && idx < (sz)count;

      if (found && out_id) {
        *out_id = devices_make_id(DEVICE_TYPE_KEYBOARD, (u64)ids[idx]);
      }
      if (ids) {
        SDL_free(ids);
      }
      return found;
    }
    case DEVICE_TYPE_MOUSE: {
      SDL_MouseID* ids = SDL_GetMice(&count);
      b32 found = ids && idx < (sz)count;

      if (found && out_id) {
        *out_id = devices_make_id(DEVICE_TYPE_MOUSE, (u64)ids[idx]);
      }
      if (ids) {
        SDL_free(ids);
      }
      return found;
    }
    case DEVICE_TYPE_GAMEPAD: {
      SDL_JoystickID* ids = SDL_GetGamepads(&count);
      b32 found = ids && idx < (sz)count;

      if (found && out_id) {
        *out_id = devices_make_id(DEVICE_TYPE_GAMEPAD, (u64)ids[idx]);
      }
      if (ids) {
        SDL_free(ids);
      }
      return found;
    }
    case DEVICE_TYPE_JOYSTICK: {
      SDL_JoystickID* ids = SDL_GetJoysticks(&count);
      b32 found = ids && idx < (sz)count;

      if (found && out_id) {
        *out_id = devices_make_id(DEVICE_TYPE_JOYSTICK, (u64)ids[idx]);
      }
      if (ids) {
        SDL_free(ids);
      }
      return found;
    }
    case DEVICE_TYPE_TOUCH: {
      SDL_TouchID* ids = SDL_GetTouchDevices(&count);
      b32 found = ids && idx < (sz)count;

      if (found && out_id) {
        *out_id = devices_make_id(DEVICE_TYPE_TOUCH, (u64)ids[idx]);
      }
      if (ids) {
        SDL_free(ids);
      }
      return found;
    }
    case DEVICE_TYPE_TABLET:
      return devices_find_tablet_by_idx(idx, out_id);
    case DEVICE_TYPE_AUDIO: {
      sz playback_count = devices_get_audio_count_for_type(AUDIO_DEVICE_TYPE_PLAYBACK);
      if (idx < playback_count) {
        return devices_get_audio_device(AUDIO_DEVICE_TYPE_PLAYBACK, idx, out_id);
      }
      return devices_get_audio_device(AUDIO_DEVICE_TYPE_RECORDING, idx - playback_count, out_id);
    }
    case DEVICE_TYPE_UNKNOWN:
    default:
      assert(type == DEVICE_TYPE_UNKNOWN);
      return 0;
  }
}

func b32 devices_is_connected(device_id id) {
  return devices_get_info(id, NULL);
}

func b32 devices_get_info(device_id id, device_info* out_info) {
  int count = 0;

  if (!device_id_is_valid(id)) {
    return 0;
  }

  if (out_info) {
    *out_info = (device_info) {0};
    out_info->id = id;
  }

  switch (id.type) {
    case DEVICE_TYPE_KEYBOARD: {
      SDL_KeyboardID* ids = SDL_GetKeyboards(&count);
      b32 found = 0;

      for (int idx = 0; ids && idx < count; idx += 1) {
        if ((u64)ids[idx] == id.instance) {
          found = 1;
          if (out_info) {
            out_info->connected = 1;
            devices_copy_cstring(out_info->name, size_of(out_info->name), SDL_GetKeyboardNameForID(ids[idx]));
          }
          break;
        }
      }

      if (ids) {
        SDL_free(ids);
      }

      return found;
    }
    case DEVICE_TYPE_MOUSE: {
      SDL_MouseID* ids = SDL_GetMice(&count);
      b32 found = 0;

      for (int idx = 0; ids && idx < count; idx += 1) {
        if ((u64)ids[idx] == id.instance) {
          found = 1;
          if (out_info) {
            out_info->connected = 1;
            devices_copy_cstring(out_info->name, size_of(out_info->name), SDL_GetMouseNameForID(ids[idx]));
          }
          break;
        }
      }

      if (ids) {
        SDL_free(ids);
      }

      return found;
    }
    case DEVICE_TYPE_GAMEPAD: {
      SDL_JoystickID* ids = SDL_GetGamepads(&count);
      b32 found = 0;

      for (int idx = 0; ids && idx < count; idx += 1) {
        if ((u64)ids[idx] == id.instance) {
          found = 1;
          if (out_info) {
            out_info->connected = 1;
            devices_copy_cstring(out_info->name, size_of(out_info->name), SDL_GetGamepadNameForID(ids[idx]));
          }
          break;
        }
      }

      if (ids) {
        SDL_free(ids);
      }

      return found;
    }
    case DEVICE_TYPE_JOYSTICK: {
      SDL_JoystickID* ids = SDL_GetJoysticks(&count);
      b32 found = 0;

      for (int idx = 0; ids && idx < count; idx += 1) {
        if ((u64)ids[idx] == id.instance) {
          found = 1;
          if (out_info) {
            out_info->connected = 1;
            devices_copy_cstring(out_info->name, size_of(out_info->name), SDL_GetJoystickNameForID(ids[idx]));
          }
          break;
        }
      }

      if (ids) {
        SDL_free(ids);
      }

      return found;
    }
    case DEVICE_TYPE_TOUCH: {
      SDL_TouchID* ids = SDL_GetTouchDevices(&count);
      b32 found = 0;

      for (int idx = 0; ids && idx < count; idx += 1) {
        if ((u64)ids[idx] == id.instance) {
          found = 1;
          if (out_info) {
            out_info->connected = 1;
            devices_copy_cstring(out_info->name, size_of(out_info->name), SDL_GetTouchDeviceName(ids[idx]));
            out_info->usage = (u16)SDL_GetTouchDeviceType(ids[idx]);
          }
          break;
        }
      }

      if (ids) {
        SDL_free(ids);
      }

      return found;
    }
    case DEVICE_TYPE_TABLET:
      return devices_find_tablet_info(id, out_info);
    case DEVICE_TYPE_AUDIO: {
      int audio_count = 0;
      u64 native_id = devices_audio_decode_native_id(id.instance);
      audio_device_type audio_type = devices_get_audio_device_type(id);
      SDL_AudioDeviceID* ids = NULL;
      b32 found = 0;

      if (audio_type == AUDIO_DEVICE_TYPE_PLAYBACK) {
        ids = SDL_GetAudioPlaybackDevices(&audio_count);
      } else if (audio_type == AUDIO_DEVICE_TYPE_RECORDING) {
        ids = SDL_GetAudioRecordingDevices(&audio_count);
      } else {
        return 0;
      }

      for (int idx = 0; ids && idx < audio_count; idx += 1) {
        if ((u64)ids[idx] == native_id) {
          found = 1;
          if (out_info) {
            out_info->connected = 1;
            out_info->usage = (u16)audio_type;
            devices_copy_cstring(out_info->name, size_of(out_info->name), SDL_GetAudioDeviceName(ids[idx]));
          }
          break;
        }
      }

      if (ids) {
        SDL_free(ids);
      }

      return found;
    }
    case DEVICE_TYPE_UNKNOWN:
    default:
      return 0;
  }
}
