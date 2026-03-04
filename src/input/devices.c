// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/devices.h"
#include "../sdl3_include.h"
#include <SDL3/SDL_hidapi.h>

func void devices_clear_name(c8* dst, sz capacity) {
  if (!dst || !capacity) {
    return;
  }

  dst[0] = '\0';
}

func void devices_copy_cstring(c8* dst, sz capacity, const c8* src) {
  sz index = 0;

  if (!dst || !capacity) {
    return;
  }

  if (!src) {
    devices_clear_name(dst, capacity);
    return;
  }

  while (src[index] && (index + 1) < capacity) {
    dst[index] = src[index];
    index += 1;
  }

  dst[index] = '\0';
}

func void devices_copy_wide_ascii(c8* dst, sz capacity, const wchar_t* src) {
  sz index = 0;

  if (!dst || !capacity) {
    return;
  }

  if (!src) {
    devices_clear_name(dst, capacity);
    return;
  }

  while (src[index] && (index + 1) < capacity) {
    wchar_t code_unit = src[index];
    dst[index] = (code_unit >= 1 && code_unit <= 0x7F) ? (c8)code_unit : '?';
    index += 1;
  }

  dst[index] = '\0';
}

func u64 devices_hash_path(const c8* src) {
  u64 hash_value = 1469598103934665603ULL;
  sz index = 0;

  if (!src) {
    return 0;
  }

  while (src[index]) {
    hash_value ^= (u8)src[index];
    hash_value *= 1099511628211ULL;
    index += 1;
  }

  return hash_value;
}

func device_id devices_make_id(device_type type, u64 instance) {
  device_id result = {0};
  result.type = type;
  result.instance = instance;
  return result;
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

func b32 devices_find_tablet_by_index(sz index, device_id* out_id) {
  SDL_hid_device_info* head = SDL_hid_enumerate(0, 0);
  SDL_hid_device_info* entry = head;
  sz current_index = 0;
  b32 found = 0;

  while (entry) {
    if (entry->usage_page == 0x0D) {
      if (current_index == index) {
        if (out_id) {
          *out_id = devices_make_id(DEVICE_TYPE_TABLET, devices_hash_path(entry->path));
        }
        found = 1;
        break;
      }

      current_index += 1;
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

func const c8* devices_get_type_name(device_type type) {
  switch (type) {
    case DEVICE_TYPE_KEYBOARD:
      return "keyboard";
    case DEVICE_TYPE_MOUSE:
      return "mouse";
    case DEVICE_TYPE_GAMEPAD:
      return "gamepad";
    case DEVICE_TYPE_TABLET:
      return "tablet";
    case DEVICE_TYPE_TOUCH:
      return "touch";
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
    case DEVICE_TYPE_UNKNOWN:
    default:
      return 0;
  }
}

func b32 devices_get_device(device_type type, sz index, device_id* out_id) {
  int count = 0;

  if (out_id) {
    *out_id = (device_id) {0};
  }

  switch (type) {
    case DEVICE_TYPE_KEYBOARD: {
      SDL_KeyboardID* ids = SDL_GetKeyboards(&count);
      b32 found = ids && index < (sz)count;

      if (found && out_id) {
        *out_id = devices_make_id(DEVICE_TYPE_KEYBOARD, (u64)ids[index]);
      }
      if (ids) {
        SDL_free(ids);
      }
      return found;
    }
    case DEVICE_TYPE_MOUSE: {
      SDL_MouseID* ids = SDL_GetMice(&count);
      b32 found = ids && index < (sz)count;

      if (found && out_id) {
        *out_id = devices_make_id(DEVICE_TYPE_MOUSE, (u64)ids[index]);
      }
      if (ids) {
        SDL_free(ids);
      }
      return found;
    }
    case DEVICE_TYPE_GAMEPAD: {
      SDL_JoystickID* ids = SDL_GetGamepads(&count);
      b32 found = ids && index < (sz)count;

      if (found && out_id) {
        *out_id = devices_make_id(DEVICE_TYPE_GAMEPAD, (u64)ids[index]);
      }
      if (ids) {
        SDL_free(ids);
      }
      return found;
    }
    case DEVICE_TYPE_TOUCH: {
      SDL_TouchID* ids = SDL_GetTouchDevices(&count);
      b32 found = ids && index < (sz)count;

      if (found && out_id) {
        *out_id = devices_make_id(DEVICE_TYPE_TOUCH, (u64)ids[index]);
      }
      if (ids) {
        SDL_free(ids);
      }
      return found;
    }
    case DEVICE_TYPE_TABLET:
      return devices_find_tablet_by_index(index, out_id);
    case DEVICE_TYPE_UNKNOWN:
    default:
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

      for (int index = 0; ids && index < count; index += 1) {
        if ((u64)ids[index] == id.instance) {
          found = 1;
          if (out_info) {
            out_info->connected = 1;
            devices_copy_cstring(out_info->name, size_of(out_info->name), SDL_GetKeyboardNameForID(ids[index]));
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

      for (int index = 0; ids && index < count; index += 1) {
        if ((u64)ids[index] == id.instance) {
          found = 1;
          if (out_info) {
            out_info->connected = 1;
            devices_copy_cstring(out_info->name, size_of(out_info->name), SDL_GetMouseNameForID(ids[index]));
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

      for (int index = 0; ids && index < count; index += 1) {
        if ((u64)ids[index] == id.instance) {
          found = 1;
          if (out_info) {
            out_info->connected = 1;
            devices_copy_cstring(out_info->name, size_of(out_info->name), SDL_GetGamepadNameForID(ids[index]));
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

      for (int index = 0; ids && index < count; index += 1) {
        if ((u64)ids[index] == id.instance) {
          found = 1;
          if (out_info) {
            out_info->connected = 1;
            devices_copy_cstring(out_info->name, size_of(out_info->name), SDL_GetTouchDeviceName(ids[index]));
            out_info->usage = (u16)SDL_GetTouchDeviceType(ids[index]);
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
    case DEVICE_TYPE_UNKNOWN:
    default:
      return 0;
  }
}
