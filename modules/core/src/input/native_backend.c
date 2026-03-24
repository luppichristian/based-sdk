// MIT License
// Copyright (c) 2026 Christian Luppi

#include "../internal.h"
#include "../sdl3_include.h"
#include "../platform_includes.h"
#include "context/thread_ctx.h"
#include "basic/profiler.h"
#include "basic/safe.h"
#include "basic/utility_defines.h"

typedef enum native_pending_kind {
  NATIVE_PENDING_KIND_NONE = 0,
  NATIVE_PENDING_KIND_KEYBOARD = 1,
  NATIVE_PENDING_KIND_MOUSE_MOTION = 2,
  NATIVE_PENDING_KIND_MOUSE_BUTTON = 3,
  NATIVE_PENDING_KIND_MOUSE_WHEEL = 4,
  NATIVE_PENDING_KIND_PEN = 5,
  NATIVE_PENDING_KIND_TOUCH = 6,
} native_pending_kind;

typedef struct native_pending_event {
  native_pending_kind kind;
  device device;
  b32 down;
  vkey key;
  mouse_button button;
  i32 xrel;
  i32 yrel;
  i32 wheel_x;
  i32 wheel_y;
} native_pending_event;

typedef struct native_known_device {
  device device;
  device_type type;
  void* handle;
  b32 connected;
  str8_short name;
} native_known_device;

global_var native_pending_event native_pending_events[DEVICES_HANDLE_CAP] = {0};
global_var sz native_pending_write_idx = 0;
global_var sz native_pending_read_idx = 0;
global_var native_known_device native_known_devices[DEVICES_HANDLE_CAP] = {0};
global_var b32 native_backend_initialized = false;

func sz native_pending_next_idx(sz idx) {
  return (idx + 1) % DEVICES_HANDLE_CAP;
}

func void native_backend_push_pending(native_pending_event pending) {
  profile_func_begin;
  sz next_idx = native_pending_next_idx(native_pending_write_idx);
  if (next_idx == native_pending_read_idx) {
    thread_log_warn("Native input pending queue overflowed kind=%u", (u32)pending.kind);
    native_pending_read_idx = native_pending_next_idx(native_pending_read_idx);
  }

  native_pending_events[native_pending_write_idx] = pending;
  native_pending_write_idx = next_idx;
  profile_func_end;
}

func native_known_device* native_backend_find_known_by_handle(void* handle, device_type type, b32 create_if_missing) {
  profile_func_begin;
  safe_for (sz item_idx = 0; item_idx < DEVICES_HANDLE_CAP; item_idx += 1) {
    if (native_known_devices[item_idx].handle == handle && native_known_devices[item_idx].type == type) {
      profile_func_end;
      return &native_known_devices[item_idx];
    }
  }

  if (!create_if_missing) {
    profile_func_end;
    return NULL;
  }

  safe_for (sz item_idx = 0; item_idx < DEVICES_HANDLE_CAP; item_idx += 1) {
    if (native_known_devices[item_idx].device == NULL) {
      native_known_devices[item_idx].device = devices_make_id(type, (u64)(up)handle);
      native_known_devices[item_idx].type = type;
      native_known_devices[item_idx].handle = handle;
      native_known_devices[item_idx].connected = 1;
      devices_update_runtime(native_known_devices[item_idx].device, 1, handle);
      profile_func_end;
      return &native_known_devices[item_idx];
    }
  }

  thread_log_error("Native input device table is full type=%u", (u32)type);
  profile_func_end;
  return NULL;
}

func void native_backend_record_device(void* handle, device_type type, b32 connected, cstr8 name) {
  profile_func_begin;
  if (handle == NULL || (type != DEVICE_TYPE_KEYBOARD && type != DEVICE_TYPE_MOUSE && type != DEVICE_TYPE_TABLET &&
                         type != DEVICE_TYPE_TOUCH)) {
    profile_func_end;
    return;
  }

  native_known_device* known = native_backend_find_known_by_handle(handle, type, connected != 0);
  if (known == NULL) {
    profile_func_end;
    return;
  }

  known->connected = connected;
  if (name != NULL && name[0] != '\0') {
    cstr8_cpy(known->name, size_of(known->name), name);
  }
  devices_update_runtime(known->device, connected, handle);
  profile_func_end;
}

func b32 native_backend_pending_matches(const native_pending_event* pending, native_pending_kind kind, const void* native_event) {
  profile_func_begin;
  const SDL_Event* event = (const SDL_Event*)native_event;
  if (pending == NULL || event == NULL || pending->kind != kind) {
    profile_func_end;
    return false;
  }

  switch (kind) {
    case NATIVE_PENDING_KIND_KEYBOARD:
      profile_func_end;
      return pending->down == (event->type == SDL_EVENT_KEY_DOWN ? true : false) &&
             pending->key == keyboard_internal_vkey_from_scancode((u32)event->key.scancode);
    case NATIVE_PENDING_KIND_MOUSE_MOTION:
      profile_func_end;
      return pending->xrel == (i32)event->motion.xrel && pending->yrel == (i32)event->motion.yrel;
    case NATIVE_PENDING_KIND_MOUSE_BUTTON:
      profile_func_end;
      return pending->down == (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN ? true : false) &&
             pending->button == mouse_button_from_sdl((u8)event->button.button);
    case NATIVE_PENDING_KIND_MOUSE_WHEEL:
      profile_func_end;
      return (pending->wheel_x == 0 || ((pending->wheel_x > 0) == (event->wheel.x > 0))) &&
             (pending->wheel_y == 0 || ((pending->wheel_y > 0) == (event->wheel.y > 0)));
    case NATIVE_PENDING_KIND_PEN:
      profile_func_end;
      return event->type >= SDL_EVENT_PEN_PROXIMITY_IN && event->type <= SDL_EVENT_PEN_AXIS;
    case NATIVE_PENDING_KIND_TOUCH:
      profile_func_end;
      return event->type >= SDL_EVENT_FINGER_DOWN && event->type <= SDL_EVENT_FINGER_CANCELED;
    default:
      profile_func_end;
      return false;
  }
}

func device native_backend_pop_matching(native_pending_kind kind, const void* native_event, device fallback_device) {
  profile_func_begin;
  sz scan_idx = native_pending_read_idx;
  while (scan_idx != native_pending_write_idx) {
    native_pending_event pending = native_pending_events[scan_idx];
    if (native_backend_pending_matches(&pending, kind, native_event)) {
      native_pending_events[scan_idx] = (native_pending_event) {0};
      while (native_pending_read_idx != native_pending_write_idx &&
             native_pending_events[native_pending_read_idx].kind == NATIVE_PENDING_KIND_NONE) {
        native_pending_read_idx = native_pending_next_idx(native_pending_read_idx);
      }
      profile_func_end;
      return pending.device;
    }
    scan_idx = native_pending_next_idx(scan_idx);
  }

  profile_func_end;
  return fallback_device;
}

func sz input_native_backend_get_device_count(device_type type) {
  profile_func_begin;
  if (type != DEVICE_TYPE_KEYBOARD && type != DEVICE_TYPE_MOUSE && type != DEVICE_TYPE_TABLET && type != DEVICE_TYPE_TOUCH) {
    profile_func_end;
    return 0;
  }

  sz total = 0;
  safe_for (sz item_idx = 0; item_idx < DEVICES_HANDLE_CAP; item_idx += 1) {
    if (native_known_devices[item_idx].device != NULL && native_known_devices[item_idx].type == type &&
        native_known_devices[item_idx].connected) {
      total += 1;
    }
  }
  profile_func_end;
  return total;
}

func device input_native_backend_get_device(device_type type, sz idx) {
  profile_func_begin;
  if (type != DEVICE_TYPE_KEYBOARD && type != DEVICE_TYPE_MOUSE && type != DEVICE_TYPE_TABLET && type != DEVICE_TYPE_TOUCH) {
    profile_func_end;
    return NULL;
  }

  sz seen = 0;
  safe_for (sz item_idx = 0; item_idx < DEVICES_HANDLE_CAP; item_idx += 1) {
    if (native_known_devices[item_idx].device == NULL || native_known_devices[item_idx].type != type ||
        !native_known_devices[item_idx].connected) {
      continue;
    }
    if (seen == idx) {
      profile_func_end;
      return native_known_devices[item_idx].device;
    }
    seen += 1;
  }
  profile_func_end;
  return NULL;
}

func b32 input_native_backend_get_info(device src, device_info* out_info) {
  profile_func_begin;
  device_type type = devices_get_type(src);
  if (type != DEVICE_TYPE_KEYBOARD && type != DEVICE_TYPE_MOUSE && type != DEVICE_TYPE_TABLET && type != DEVICE_TYPE_TOUCH) {
    profile_func_end;
    return false;
  }

  safe_for (sz item_idx = 0; item_idx < DEVICES_HANDLE_CAP; item_idx += 1) {
    if (native_known_devices[item_idx].device != src) {
      continue;
    }

    if (out_info != NULL) {
      out_info->id = src;
      out_info->connected = native_known_devices[item_idx].connected;
      out_info->device_state = device_get_state(src);
      out_info->battery_state = BATTERY_STATE_UNKNOWN;
      out_info->handle = native_known_devices[item_idx].handle;
      cstr8_cpy(out_info->name, size_of(out_info->name), native_known_devices[item_idx].name);
    }
    profile_func_end;
    return true;
  }

  profile_func_end;
  return false;
}

func device input_native_backend_resolve_keyboard_device(const void* native_event, device fallback_device) {
  return native_backend_pop_matching(NATIVE_PENDING_KIND_KEYBOARD, native_event, fallback_device);
}

func device input_native_backend_resolve_mouse_motion_device(const void* native_event, device fallback_device) {
  return native_backend_pop_matching(NATIVE_PENDING_KIND_MOUSE_MOTION, native_event, fallback_device);
}

func device input_native_backend_resolve_mouse_button_device(const void* native_event, device fallback_device) {
  return native_backend_pop_matching(NATIVE_PENDING_KIND_MOUSE_BUTTON, native_event, fallback_device);
}

func device input_native_backend_resolve_mouse_wheel_device(const void* native_event, device fallback_device) {
  return native_backend_pop_matching(NATIVE_PENDING_KIND_MOUSE_WHEEL, native_event, fallback_device);
}

func device input_native_backend_resolve_pen_device(const void* native_event, device fallback_device) {
  return native_backend_pop_matching(NATIVE_PENDING_KIND_PEN, native_event, fallback_device);
}

func device input_native_backend_resolve_touch_device(const void* native_event, device fallback_device) {
  return native_backend_pop_matching(NATIVE_PENDING_KIND_TOUCH, native_event, fallback_device);
}

#if defined(_WIN32)
typedef struct native_window_hook {
  window window_id;
  HWND hwnd;
  WNDPROC old_proc;
} native_window_hook;

global_var native_window_hook native_window_hooks[DEVICES_HANDLE_CAP] = {0};

func void native_backend_record_raw_handle(HANDLE handle, b32 connected);
func void native_backend_handle_win_pointer(WPARAM wparam);

func native_window_hook* native_backend_find_hook(HWND hwnd, b32 create_if_missing, window window_id) {
  profile_func_begin;
  safe_for (sz item_idx = 0; item_idx < DEVICES_HANDLE_CAP; item_idx += 1) {
    if (native_window_hooks[item_idx].hwnd == hwnd && hwnd != NULL) {
      profile_func_end;
      return &native_window_hooks[item_idx];
    }
  }

  if (!create_if_missing) {
    profile_func_end;
    return NULL;
  }

  safe_for (sz item_idx = 0; item_idx < DEVICES_HANDLE_CAP; item_idx += 1) {
    if (native_window_hooks[item_idx].hwnd == NULL) {
      native_window_hooks[item_idx].hwnd = hwnd;
      native_window_hooks[item_idx].window_id = window_id;
      profile_func_end;
      return &native_window_hooks[item_idx];
    }
  }

  profile_func_end;
  return NULL;
}

func HWND native_backend_get_window_hwnd(window window_id) {
  profile_func_begin;
  SDL_Window* window_ptr = window_resolve(window_id);
  if (window_ptr == NULL) {
    profile_func_end;
    return NULL;
  }
  SDL_PropertiesID props = SDL_GetWindowProperties(window_ptr);
  HWND hwnd = (HWND)SDL_GetPointerProperty(props, "SDL.window.win32.hwnd", NULL);
  profile_func_end;
  return hwnd;
}

func void native_backend_register_raw_input(HWND hwnd) {
  profile_func_begin;
  RAWINPUTDEVICE raw_devices[2] = {
      {.usUsagePage = HID_USAGE_PAGE_GENERIC, .usUsage = HID_USAGE_GENERIC_KEYBOARD, .dwFlags = RIDEV_DEVNOTIFY, .hwndTarget = hwnd},
      {.usUsagePage = HID_USAGE_PAGE_GENERIC,    .usUsage = HID_USAGE_GENERIC_MOUSE, .dwFlags = RIDEV_DEVNOTIFY, .hwndTarget = hwnd},
  };

  if (!RegisterRawInputDevices(raw_devices, count_of(raw_devices), size_of(RAWINPUTDEVICE))) {
    thread_log_warn("Failed to register raw input devices error=%lu", GetLastError());
  }
  profile_func_end;
}

func void native_backend_enumerate_raw_devices(void) {
  profile_func_begin;
  UINT device_count = 0;
  if (GetRawInputDeviceList(NULL, &device_count, size_of(RAWINPUTDEVICELIST)) != 0 || device_count == 0) {
    profile_func_end;
    return;
  }

  RAWINPUTDEVICELIST* device_list = (RAWINPUTDEVICELIST*)SDL_malloc((sz)device_count * size_of(RAWINPUTDEVICELIST));
  if (device_list == NULL) {
    profile_func_end;
    return;
  }

  if (GetRawInputDeviceList(device_list, &device_count, size_of(RAWINPUTDEVICELIST)) != (UINT)-1) {
    safe_for (UINT item_idx = 0; item_idx < device_count; item_idx += 1) {
      native_backend_record_raw_handle(device_list[item_idx].hDevice, true);
    }
  }

  SDL_free(device_list);
  profile_func_end;
}

func device_type native_backend_get_rim_type(HANDLE handle) {
  profile_func_begin;
  RID_DEVICE_INFO info = {0};
  UINT info_size = size_of(info);
  info.cbSize = size_of(info);
  if (GetRawInputDeviceInfoW(handle, RIDI_DEVICEINFO, &info, &info_size) == (UINT)-1) {
    profile_func_end;
    return DEVICE_TYPE_UNKNOWN;
  }

  if (info.dwType == RIM_TYPEKEYBOARD) {
    profile_func_end;
    return DEVICE_TYPE_KEYBOARD;
  }
  if (info.dwType == RIM_TYPEMOUSE) {
    profile_func_end;
    return DEVICE_TYPE_MOUSE;
  }
  profile_func_end;
  return DEVICE_TYPE_UNKNOWN;
}

func void native_backend_record_raw_handle(HANDLE handle, b32 connected) {
  profile_func_begin;
  device_type type = native_backend_get_rim_type(handle);
  if (type == DEVICE_TYPE_UNKNOWN) {
    RID_DEVICE_INFO info = {0};
    UINT info_size = size_of(info);
    info.cbSize = size_of(info);
    if (GetRawInputDeviceInfoW(handle, RIDI_DEVICEINFO, &info, &info_size) != (UINT)-1 && info.dwType == RIM_TYPEHID &&
        info.hid.usUsagePage == HID_USAGE_PAGE_DIGITIZER) {
      type = DEVICE_TYPE_TABLET;
    }
  }
  if (type != DEVICE_TYPE_KEYBOARD && type != DEVICE_TYPE_MOUSE && type != DEVICE_TYPE_TABLET) {
    profile_func_end;
    return;
  }
  native_backend_record_device((void*)handle,
                               type,
                               connected,
                               type == DEVICE_TYPE_KEYBOARD ? "Raw Keyboard"
                               : type == DEVICE_TYPE_MOUSE  ? "Raw Mouse"
                                                            : "Raw Tablet");
  profile_func_end;
}

func LRESULT CALLBACK native_backend_window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
  native_window_hook* hook = native_backend_find_hook(hwnd, false, NULL);

  if (msg == WM_INPUT_DEVICE_CHANGE) {
    native_backend_record_raw_handle((HANDLE)lparam, wparam == GIDC_ARRIVAL ? true : false);
  } else if (msg == WM_INPUT) {
    UINT data_size = 0;
    if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, NULL, &data_size, size_of(RAWINPUTHEADER)) == 0 && data_size > 0) {
      BYTE raw_buffer[256] = {0};
      BYTE* raw_ptr = raw_buffer;
      if (data_size > size_of(raw_buffer)) {
        raw_ptr = (BYTE*)SDL_malloc(data_size);
      }
      if (raw_ptr != NULL &&
          GetRawInputData((HRAWINPUT)lparam, RID_INPUT, raw_ptr, &data_size, size_of(RAWINPUTHEADER)) == data_size) {
        RAWINPUT* raw = (RAWINPUT*)raw_ptr;
        if (raw->header.dwType == RIM_TYPEKEYBOARD) {
          native_backend_record_raw_handle(raw->header.hDevice, true);
          native_known_device* known = native_backend_find_known_by_handle(raw->header.hDevice, DEVICE_TYPE_KEYBOARD, true);
          if (known != NULL) {
            native_backend_push_pending((native_pending_event) {
                .kind = NATIVE_PENDING_KIND_KEYBOARD,
                .device = known->device,
                .down = (raw->data.keyboard.Flags & RI_KEY_BREAK) == 0,
                .key = keyboard_internal_vkey_from_scancode((u32)raw->data.keyboard.MakeCode),
            });
          }
        } else if (raw->header.dwType == RIM_TYPEMOUSE) {
          native_backend_record_raw_handle(raw->header.hDevice, true);
          native_known_device* known = native_backend_find_known_by_handle(raw->header.hDevice, DEVICE_TYPE_MOUSE, true);
          if (known != NULL) {
            if (raw->data.mouse.lLastX != 0 || raw->data.mouse.lLastY != 0) {
              native_backend_push_pending((native_pending_event) {
                  .kind = NATIVE_PENDING_KIND_MOUSE_MOTION,
                  .device = known->device,
                  .xrel = (i32)raw->data.mouse.lLastX,
                  .yrel = (i32)raw->data.mouse.lLastY,
              });
            }
            if ((raw->data.mouse.usButtonFlags & (RI_MOUSE_LEFT_BUTTON_DOWN | RI_MOUSE_LEFT_BUTTON_UP)) != 0) {
              native_backend_push_pending((native_pending_event) {
                  .kind = NATIVE_PENDING_KIND_MOUSE_BUTTON,
                  .device = known->device,
                  .button = MOUSE_BUTTON_LEFT,
                  .down = (raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) != 0,
              });
            }
            if ((raw->data.mouse.usButtonFlags & (RI_MOUSE_RIGHT_BUTTON_DOWN | RI_MOUSE_RIGHT_BUTTON_UP)) != 0) {
              native_backend_push_pending((native_pending_event) {
                  .kind = NATIVE_PENDING_KIND_MOUSE_BUTTON,
                  .device = known->device,
                  .button = MOUSE_BUTTON_RIGHT,
                  .down = (raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) != 0,
              });
            }
            if ((raw->data.mouse.usButtonFlags & (RI_MOUSE_MIDDLE_BUTTON_DOWN | RI_MOUSE_MIDDLE_BUTTON_UP)) != 0) {
              native_backend_push_pending((native_pending_event) {
                  .kind = NATIVE_PENDING_KIND_MOUSE_BUTTON,
                  .device = known->device,
                  .button = MOUSE_BUTTON_MIDDLE,
                  .down = (raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) != 0,
              });
            }
            if ((raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL) != 0) {
              native_backend_push_pending((native_pending_event) {
                  .kind = NATIVE_PENDING_KIND_MOUSE_WHEEL,
                  .device = known->device,
                  .wheel_y = (SHORT)raw->data.mouse.usButtonData,
              });
            }
          }
        } else if (raw->header.dwType == RIM_TYPEHID) {
          native_backend_record_raw_handle(raw->header.hDevice, true);
          native_known_device* known = native_backend_find_known_by_handle(raw->header.hDevice, DEVICE_TYPE_TABLET, false);
          if (known != NULL) {
            native_backend_push_pending((native_pending_event) {
                .kind = NATIVE_PENDING_KIND_PEN,
                .device = known->device,
            });
          }
        }
      }
      if (raw_ptr != raw_buffer && raw_ptr != NULL) {
        SDL_free(raw_ptr);
      }
    }
  } else if (msg == WM_POINTERDOWN || msg == WM_POINTERUPDATE || msg == WM_POINTERUP) {
    native_backend_handle_win_pointer(wparam);
  }

  return hook && hook->old_proc ? CallWindowProcW(hook->old_proc, hwnd, msg, wparam, lparam) : DefWindowProcW(hwnd, msg, wparam, lparam);
}

typedef BOOL(WINAPI* get_pointer_info_fn)(UINT32 pointer_id, POINTER_INFO* pointer_info);
typedef BOOL(WINAPI* get_pointer_type_fn)(UINT32 pointer_id, POINTER_INPUT_TYPE* pointer_type);

func void native_backend_handle_win_pointer(WPARAM wparam) {
  profile_func_begin;
  HMODULE user32_module = GetModuleHandleW(L"user32.dll");
  get_pointer_info_fn get_pointer_info_ptr =
      user32_module ? (get_pointer_info_fn)GetProcAddress(user32_module, "GetPointerInfo") : NULL;
  get_pointer_type_fn get_pointer_type_ptr =
      user32_module ? (get_pointer_type_fn)GetProcAddress(user32_module, "GetPointerType") : NULL;
  UINT32 pointer_id = GET_POINTERID_WPARAM(wparam);
  POINTER_INFO pointer_info = {0};
  POINTER_INPUT_TYPE pointer_type = PT_POINTER;

  if (get_pointer_info_ptr == NULL || get_pointer_type_ptr == NULL) {
    profile_func_end;
    return;
  }
  if (!get_pointer_type_ptr(pointer_id, &pointer_type) || !get_pointer_info_ptr(pointer_id, &pointer_info)) {
    profile_func_end;
    return;
  }

  if (pointer_type == PT_TOUCH) {
    native_backend_record_device((void*)pointer_info.sourceDevice, DEVICE_TYPE_TOUCH, true, "Raw Touch");
    native_known_device* known = native_backend_find_known_by_handle((void*)pointer_info.sourceDevice, DEVICE_TYPE_TOUCH, true);
    if (known != NULL) {
      native_backend_push_pending((native_pending_event) {
          .kind = NATIVE_PENDING_KIND_TOUCH,
          .device = known->device,
      });
    }
  } else if (pointer_type == PT_PEN) {
    native_backend_record_device((void*)pointer_info.sourceDevice, DEVICE_TYPE_TABLET, true, "Raw Pen");
    native_known_device* known = native_backend_find_known_by_handle((void*)pointer_info.sourceDevice, DEVICE_TYPE_TABLET, true);
    if (known != NULL) {
      native_backend_push_pending((native_pending_event) {
          .kind = NATIVE_PENDING_KIND_PEN,
          .device = known->device,
      });
    }
  }
  profile_func_end;
}
#endif

#if defined(__linux__)
typedef struct native_linux_state {
  struct udev* udev_ctx;
  struct libinput* input_ctx;
} native_linux_state;

global_var native_linux_state native_linux = {0};

func i32 native_linux_open_restricted(const char* path, i32 flags, void* user_data) {
  (void)user_data;
  return open(path, flags);
}

func void native_linux_close_restricted(i32 fd, void* user_data) {
  (void)user_data;
  if (fd >= 0) {
    close(fd);
  }
}

global_var struct libinput_interface native_linux_interface = {
    .open_restricted = native_linux_open_restricted,
    .close_restricted = native_linux_close_restricted,
};

func mouse_button native_linux_button_from_code(u32 button_code) {
  switch (button_code) {
    case 0x110:
      return MOUSE_BUTTON_LEFT;
    case 0x111:
      return MOUSE_BUTTON_RIGHT;
    case 0x112:
      return MOUSE_BUTTON_MIDDLE;
    case 0x113:
      return MOUSE_BUTTON_X1;
    case 0x114:
      return MOUSE_BUTTON_X2;
    default:
      return MOUSE_BUTTON_COUNT;
  }
}

func void native_linux_record_libinput_device(struct libinput_device* input_device, b32 connected) {
  profile_func_begin;
  if (input_device == NULL) {
    profile_func_end;
    return;
  }

  void* handle = (void*)input_device;
  if (libinput_device_has_capability(input_device, LIBINPUT_DEVICE_CAP_KEYBOARD)) {
    native_backend_record_device(handle, DEVICE_TYPE_KEYBOARD, connected, libinput_device_get_name(input_device));
  }
  if (libinput_device_has_capability(input_device, LIBINPUT_DEVICE_CAP_POINTER)) {
    native_backend_record_device(handle, DEVICE_TYPE_MOUSE, connected, libinput_device_get_name(input_device));
  }
  if (libinput_device_has_capability(input_device, LIBINPUT_DEVICE_CAP_TABLET_TOOL) ||
      libinput_device_has_capability(input_device, LIBINPUT_DEVICE_CAP_TABLET_PAD)) {
    native_backend_record_device(handle, DEVICE_TYPE_TABLET, connected, libinput_device_get_name(input_device));
  }
  if (libinput_device_has_capability(input_device, LIBINPUT_DEVICE_CAP_TOUCH)) {
    native_backend_record_device(handle, DEVICE_TYPE_TOUCH, connected, libinput_device_get_name(input_device));
  }
  profile_func_end;
}

func void native_linux_handle_event(struct libinput_event* event) {
  profile_func_begin;
  if (event == NULL) {
    profile_func_end;
    return;
  }

  enum libinput_event_type event_type = libinput_event_get_type(event);
  struct libinput_device* input_device = libinput_event_get_device(event);
  native_known_device* known = NULL;

  switch (event_type) {
    case LIBINPUT_EVENT_DEVICE_ADDED:
      native_linux_record_libinput_device(input_device, true);
      break;
    case LIBINPUT_EVENT_DEVICE_REMOVED:
      native_linux_record_libinput_device(input_device, false);
      break;
    case LIBINPUT_EVENT_KEYBOARD_KEY: {
      struct libinput_event_keyboard* keyboard_event = libinput_event_get_keyboard_event(event);
      native_linux_record_libinput_device(input_device, true);
      known = native_backend_find_known_by_handle((void*)input_device, DEVICE_TYPE_KEYBOARD, true);
      if (known != NULL) {
        native_backend_push_pending((native_pending_event) {
            .kind = NATIVE_PENDING_KIND_KEYBOARD,
            .device = known->device,
            .down = libinput_event_keyboard_get_key_state(keyboard_event) == LIBINPUT_KEY_STATE_PRESSED,
            .key = keyboard_internal_vkey_from_scancode((u32)(libinput_event_keyboard_get_key(keyboard_event) + 8)),
        });
      }
    } break;
    case LIBINPUT_EVENT_POINTER_MOTION: {
      struct libinput_event_pointer* pointer_event = libinput_event_get_pointer_event(event);
      native_linux_record_libinput_device(input_device, true);
      known = native_backend_find_known_by_handle((void*)input_device, DEVICE_TYPE_MOUSE, true);
      if (known != NULL) {
        native_backend_push_pending((native_pending_event) {
            .kind = NATIVE_PENDING_KIND_MOUSE_MOTION,
            .device = known->device,
            .xrel = (i32)libinput_event_pointer_get_dx(pointer_event),
            .yrel = (i32)libinput_event_pointer_get_dy(pointer_event),
        });
      }
    } break;
    case LIBINPUT_EVENT_POINTER_BUTTON: {
      struct libinput_event_pointer* pointer_event = libinput_event_get_pointer_event(event);
      mouse_button button = native_linux_button_from_code(libinput_event_pointer_get_button(pointer_event));
      native_linux_record_libinput_device(input_device, true);
      known = native_backend_find_known_by_handle((void*)input_device, DEVICE_TYPE_MOUSE, true);
      if (known != NULL && button < MOUSE_BUTTON_COUNT) {
        native_backend_push_pending((native_pending_event) {
            .kind = NATIVE_PENDING_KIND_MOUSE_BUTTON,
            .device = known->device,
            .button = button,
            .down = libinput_event_pointer_get_button_state(pointer_event) == LIBINPUT_BUTTON_STATE_PRESSED,
        });
      }
    } break;
    case LIBINPUT_EVENT_POINTER_SCROLL_WHEEL:
    case LIBINPUT_EVENT_POINTER_SCROLL_FINGER:
    case LIBINPUT_EVENT_POINTER_SCROLL_CONTINUOUS: {
      struct libinput_event_pointer* pointer_event = libinput_event_get_pointer_event(event);
      native_linux_record_libinput_device(input_device, true);
      known = native_backend_find_known_by_handle((void*)input_device, DEVICE_TYPE_MOUSE, true);
      if (known != NULL) {
        native_backend_push_pending((native_pending_event) {
            .kind = NATIVE_PENDING_KIND_MOUSE_WHEEL,
            .device = known->device,
            .wheel_x = libinput_event_pointer_has_axis(pointer_event, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL)
                           ? (i32)libinput_event_pointer_get_scroll_value_v120(
                                 pointer_event,
                                 LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL)
                           : 0,
            .wheel_y = libinput_event_pointer_has_axis(pointer_event, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL)
                           ? (i32)libinput_event_pointer_get_scroll_value_v120(
                                 pointer_event,
                                 LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL)
                           : 0,
        });
      }
    } break;
    case LIBINPUT_EVENT_TABLET_TOOL_PROXIMITY:
    case LIBINPUT_EVENT_TABLET_TOOL_TIP:
    case LIBINPUT_EVENT_TABLET_TOOL_BUTTON:
    case LIBINPUT_EVENT_TABLET_TOOL_AXIS:
    case LIBINPUT_EVENT_TABLET_PAD_BUTTON:
    case LIBINPUT_EVENT_TABLET_PAD_RING:
    case LIBINPUT_EVENT_TABLET_PAD_STRIP:      {
      native_linux_record_libinput_device(input_device, true);
      known = native_backend_find_known_by_handle((void*)input_device, DEVICE_TYPE_TABLET, true);
      if (known != NULL) {
        native_backend_push_pending((native_pending_event) {
            .kind = NATIVE_PENDING_KIND_PEN,
            .device = known->device,
        });
      }
    } break;
    case LIBINPUT_EVENT_TOUCH_DOWN:
    case LIBINPUT_EVENT_TOUCH_UP:
    case LIBINPUT_EVENT_TOUCH_MOTION:
    case LIBINPUT_EVENT_TOUCH_CANCEL:
    case LIBINPUT_EVENT_TOUCH_FRAME:  {
      native_linux_record_libinput_device(input_device, true);
      known = native_backend_find_known_by_handle((void*)input_device, DEVICE_TYPE_TOUCH, true);
      if (known != NULL) {
        native_backend_push_pending((native_pending_event) {
            .kind = NATIVE_PENDING_KIND_TOUCH,
            .device = known->device,
        });
      }
    } break;
    default:
      break;
  }
  profile_func_end;
}
#endif

#if defined(__APPLE__)
typedef struct native_macos_state {
  IOHIDManagerRef hid_manager;
} native_macos_state;

global_var native_macos_state native_macos = {0};

func cstr8 native_macos_product_name(IOHIDDeviceRef device_ref, str8_short* out_name) {
  profile_func_begin;
  if (out_name != NULL) {
    out_name[0] = '\0';
  }
  if (device_ref == NULL || out_name == NULL) {
    profile_func_end;
    return "";
  }

  CFStringRef product = (CFStringRef)IOHIDDeviceGetProperty(device_ref, CFSTR(kIOHIDProductKey));
  if (product != NULL) {
    (void)CFStringGetCString(product, out_name, (CFIndex)size_of(*out_name), kCFStringEncodingUTF8);
  }
  profile_func_end;
  return out_name;
}

func mouse_button native_macos_button_from_usage(u32 usage) {
  switch (usage) {
    case 1:
      return MOUSE_BUTTON_LEFT;
    case 2:
      return MOUSE_BUTTON_RIGHT;
    case 3:
      return MOUSE_BUTTON_MIDDLE;
    case 4:
      return MOUSE_BUTTON_X1;
    case 5:
      return MOUSE_BUTTON_X2;
    default:
      return MOUSE_BUTTON_COUNT;
  }
}

func void native_macos_device_attached(void* context, IOReturn result, void* sender, IOHIDDeviceRef device_ref) {
  profile_func_begin;
  (void)context;
  (void)result;
  (void)sender;
  str8_short product_name = {0};
  if (device_ref != NULL) {
    native_backend_record_device((void*)device_ref, DEVICE_TYPE_KEYBOARD, false, "");
    native_backend_record_device((void*)device_ref, DEVICE_TYPE_MOUSE, false, "");
    native_backend_record_device((void*)device_ref, DEVICE_TYPE_TABLET, false, "");
    native_backend_record_device((void*)device_ref, DEVICE_TYPE_TOUCH, false, "");
    if (IOHIDDeviceConformsTo(device_ref, kHIDPage_GenericDesktop, kHIDUsage_GD_Keyboard) ||
        IOHIDDeviceConformsTo(device_ref, kHIDPage_GenericDesktop, kHIDUsage_GD_Keypad)) {
      native_backend_record_device((void*)device_ref, DEVICE_TYPE_KEYBOARD, true, native_macos_product_name(device_ref, &product_name));
    }
    if (IOHIDDeviceConformsTo(device_ref, kHIDPage_GenericDesktop, kHIDUsage_GD_Mouse) ||
        IOHIDDeviceConformsTo(device_ref, kHIDPage_GenericDesktop, kHIDUsage_GD_Pointer)) {
      native_backend_record_device((void*)device_ref, DEVICE_TYPE_MOUSE, true, native_macos_product_name(device_ref, &product_name));
    }
    if (IOHIDDeviceConformsTo(device_ref, kHIDPage_Digitizer, kHIDUsage_Dig_Pen) ||
        IOHIDDeviceConformsTo(device_ref, kHIDPage_Digitizer, kHIDUsage_Dig_TouchScreen) ||
        IOHIDDeviceConformsTo(device_ref, kHIDPage_Digitizer, kHIDUsage_Dig_TouchPad)) {
      native_backend_record_device((void*)device_ref, DEVICE_TYPE_TABLET, true, native_macos_product_name(device_ref, &product_name));
    }
    if (IOHIDDeviceConformsTo(device_ref, kHIDPage_Digitizer, kHIDUsage_Dig_TouchScreen) ||
        IOHIDDeviceConformsTo(device_ref, kHIDPage_Digitizer, kHIDUsage_Dig_TouchPad)) {
      native_backend_record_device((void*)device_ref, DEVICE_TYPE_TOUCH, true, native_macos_product_name(device_ref, &product_name));
    }
  }
  profile_func_end;
}

func void native_macos_device_removed(void* context, IOReturn result, void* sender, IOHIDDeviceRef device_ref) {
  profile_func_begin;
  (void)context;
  (void)result;
  (void)sender;
  if (device_ref != NULL) {
    native_backend_record_device((void*)device_ref, DEVICE_TYPE_KEYBOARD, false, "");
    native_backend_record_device((void*)device_ref, DEVICE_TYPE_MOUSE, false, "");
    native_backend_record_device((void*)device_ref, DEVICE_TYPE_TABLET, false, "");
    native_backend_record_device((void*)device_ref, DEVICE_TYPE_TOUCH, false, "");
  }
  profile_func_end;
}

func void native_macos_input_value(void* context, IOReturn result, void* sender, IOHIDValueRef value_ref) {
  profile_func_begin;
  (void)context;
  (void)result;
  (void)sender;
  if (value_ref == NULL) {
    profile_func_end;
    return;
  }

  IOHIDElementRef element = IOHIDValueGetElement(value_ref);
  IOHIDDeviceRef device_ref = IOHIDElementGetDevice(element);
  u32 usage_page = (u32)IOHIDElementGetUsagePage(element);
  u32 usage = (u32)IOHIDElementGetUsage(element);
  CFIndex integer_value = IOHIDValueGetIntegerValue(value_ref);

  if (usage_page == kHIDPage_KeyboardOrKeypad) {
    native_known_device* known = native_backend_find_known_by_handle((void*)device_ref, DEVICE_TYPE_KEYBOARD, true);
    if (known != NULL) {
      native_backend_push_pending((native_pending_event) {
          .kind = NATIVE_PENDING_KIND_KEYBOARD,
          .device = known->device,
          .down = integer_value != 0,
          .key = keyboard_internal_vkey_from_scancode(usage),
      });
    }
  } else if (usage_page == kHIDPage_Button) {
    native_known_device* known = native_backend_find_known_by_handle((void*)device_ref, DEVICE_TYPE_MOUSE, true);
    mouse_button button = native_macos_button_from_usage(usage);
    if (known != NULL && button < MOUSE_BUTTON_COUNT) {
      native_backend_push_pending((native_pending_event) {
          .kind = NATIVE_PENDING_KIND_MOUSE_BUTTON,
          .device = known->device,
          .button = button,
          .down = integer_value != 0,
      });
    }
  } else if (usage_page == kHIDPage_GenericDesktop && (usage == kHIDUsage_GD_X || usage == kHIDUsage_GD_Y)) {
    native_known_device* known = native_backend_find_known_by_handle((void*)device_ref, DEVICE_TYPE_MOUSE, true);
    if (known != NULL) {
      native_backend_push_pending((native_pending_event) {
          .kind = NATIVE_PENDING_KIND_MOUSE_MOTION,
          .device = known->device,
          .xrel = usage == kHIDUsage_GD_X ? (i32)integer_value : 0,
          .yrel = usage == kHIDUsage_GD_Y ? (i32)integer_value : 0,
      });
    }
  } else if (usage_page == kHIDPage_GenericDesktop && usage == kHIDUsage_GD_Wheel) {
    native_known_device* known = native_backend_find_known_by_handle((void*)device_ref, DEVICE_TYPE_MOUSE, true);
    if (known != NULL) {
      native_backend_push_pending((native_pending_event) {
          .kind = NATIVE_PENDING_KIND_MOUSE_WHEEL,
          .device = known->device,
          .wheel_y = (i32)integer_value,
      });
    }
  } else if (usage_page == kHIDPage_Digitizer) {
    native_known_device* known_tablet = native_backend_find_known_by_handle((void*)device_ref, DEVICE_TYPE_TABLET, false);
    native_known_device* known_touch = native_backend_find_known_by_handle((void*)device_ref, DEVICE_TYPE_TOUCH, false);
    if (known_tablet != NULL) {
      native_backend_push_pending((native_pending_event) {
          .kind = NATIVE_PENDING_KIND_PEN,
          .device = known_tablet->device,
      });
    }
    if (known_touch != NULL) {
      native_backend_push_pending((native_pending_event) {
          .kind = NATIVE_PENDING_KIND_TOUCH,
          .device = known_touch->device,
      });
    }
  }
  profile_func_end;
}
#endif

func b32 input_native_backend_init(void) {
  profile_func_begin;
  if (native_backend_initialized) {
    profile_func_end;
    return true;
  }
  thread_log_info("Initializing native input backend");
#if defined(__linux__)
  native_linux.udev_ctx = udev_new();
  if (native_linux.udev_ctx == NULL) {
    thread_log_warn("Failed to create libudev context for native input");
    profile_func_end;
    return false;
  }
  native_linux.input_ctx = libinput_udev_create_context(&native_linux_interface, NULL, native_linux.udev_ctx);
  if (native_linux.input_ctx == NULL) {
    thread_log_warn("Failed to create libinput context");
    udev_unref(native_linux.udev_ctx);
    native_linux.udev_ctx = NULL;
    profile_func_end;
    return false;
  }
  if (libinput_udev_assign_seat(native_linux.input_ctx, "seat0") != 0) {
    thread_log_warn("Failed to assign libinput seat seat0");
    libinput_unref(native_linux.input_ctx);
    native_linux.input_ctx = NULL;
    udev_unref(native_linux.udev_ctx);
    native_linux.udev_ctx = NULL;
    profile_func_end;
    return false;
  }
  if (libinput_dispatch(native_linux.input_ctx) == 0) {
    struct libinput_event* event = NULL;
    while ((event = libinput_get_event(native_linux.input_ctx)) != NULL) {
      native_linux_handle_event(event);
      libinput_event_destroy(event);
      (void)libinput_dispatch(native_linux.input_ctx);
    }
  }
#elif defined(__APPLE__)
  native_macos.hid_manager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
  if (native_macos.hid_manager == NULL) {
    thread_log_warn("Failed to create IOHIDManager");
    profile_func_end;
    return false;
  }
  IOHIDManagerRegisterDeviceMatchingCallback(native_macos.hid_manager, native_macos_device_attached, NULL);
  IOHIDManagerRegisterDeviceRemovalCallback(native_macos.hid_manager, native_macos_device_removed, NULL);
  IOHIDManagerRegisterInputValueCallback(native_macos.hid_manager, native_macos_input_value, NULL);
  IOHIDManagerScheduleWithRunLoop(native_macos.hid_manager, CFRunLoopGetMain(), kCFRunLoopDefaultMode);
  IOHIDManagerSetDeviceMatching(native_macos.hid_manager, NULL);
  if (IOHIDManagerOpen(native_macos.hid_manager, kIOHIDOptionsTypeNone) != kIOReturnSuccess) {
    thread_log_warn("Failed to open IOHIDManager");
    CFRelease(native_macos.hid_manager);
    native_macos.hid_manager = NULL;
    profile_func_end;
    return false;
  }
  CFSetRef devices = IOHIDManagerCopyDevices(native_macos.hid_manager);
  if (devices != NULL) {
    CFIndex total = CFSetGetCount(devices);
    if (total > 0) {
      IOHIDDeviceRef device_refs[DEVICES_HANDLE_CAP] = {0};
      CFIndex limited_total = total > (CFIndex)DEVICES_HANDLE_CAP ? (CFIndex)DEVICES_HANDLE_CAP : total;
      CFSetGetValues(devices, (const void**)device_refs);
      safe_for (CFIndex item_idx = 0; item_idx < limited_total; item_idx += 1) {
        native_macos_device_attached(NULL, kIOReturnSuccess, NULL, device_refs[item_idx]);
      }
    }
    CFRelease(devices);
  }
#endif
  native_backend_initialized = true;
  profile_func_end;
  return true;
}

func void input_native_backend_quit(void) {
  profile_func_begin;
  if (!native_backend_initialized) {
    profile_func_end;
    return;
  }
#if defined(_WIN32)
  safe_for (sz item_idx = 0; item_idx < DEVICES_HANDLE_CAP; item_idx += 1) {
    if (native_window_hooks[item_idx].hwnd != NULL && native_window_hooks[item_idx].old_proc != NULL) {
      SetWindowLongPtrW(native_window_hooks[item_idx].hwnd, GWLP_WNDPROC, (LONG_PTR)native_window_hooks[item_idx].old_proc);
    }
    native_window_hooks[item_idx] = (native_window_hook) {0};
  }
#elif defined(__linux__)
  if (native_linux.input_ctx != NULL) {
    libinput_unref(native_linux.input_ctx);
    native_linux.input_ctx = NULL;
  }
  if (native_linux.udev_ctx != NULL) {
    udev_unref(native_linux.udev_ctx);
    native_linux.udev_ctx = NULL;
  }
#elif defined(__APPLE__)
  if (native_macos.hid_manager != NULL) {
    IOHIDManagerUnscheduleFromRunLoop(native_macos.hid_manager, CFRunLoopGetMain(), kCFRunLoopDefaultMode);
    IOHIDManagerClose(native_macos.hid_manager, kIOHIDOptionsTypeNone);
    CFRelease(native_macos.hid_manager);
    native_macos.hid_manager = NULL;
  }
#endif
  native_pending_read_idx = 0;
  native_pending_write_idx = 0;
  safe_for (sz item_idx = 0; item_idx < DEVICES_HANDLE_CAP; item_idx += 1) {
    native_pending_events[item_idx] = (native_pending_event) {0};
    native_known_devices[item_idx] = (native_known_device) {0};
  }
  native_backend_initialized = false;
  profile_func_end;
}

func void input_native_backend_pump(void) {
  profile_func_begin;
  if (!native_backend_initialized) {
    profile_func_end;
    return;
  }
#if defined(__linux__)
  if (native_linux.input_ctx != NULL) {
    struct pollfd poll_fd = {0};
    poll_fd.fd = libinput_get_fd(native_linux.input_ctx);
    poll_fd.events = POLLIN;
    if (poll_fd.fd >= 0 && poll(&poll_fd, 1, 0) > 0) {
      if (libinput_dispatch(native_linux.input_ctx) == 0) {
        struct libinput_event* event = NULL;
        while ((event = libinput_get_event(native_linux.input_ctx)) != NULL) {
          native_linux_handle_event(event);
          libinput_event_destroy(event);
          (void)libinput_dispatch(native_linux.input_ctx);
        }
      }
    }
  }
#elif defined(__APPLE__)
  if (native_macos.hid_manager != NULL) {
    (void)CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.0, true);
  }
#endif
  profile_func_end;
}

func void input_native_backend_on_window_created(window window_id) {
  profile_func_begin;
  if (!native_backend_initialized && !input_native_backend_init()) {
    profile_func_end;
    return;
  }
#if defined(_WIN32)
  native_backend_enumerate_raw_devices();
  HWND hwnd = native_backend_get_window_hwnd(window_id);
  if (hwnd != NULL) {
    native_window_hook* hook = native_backend_find_hook(hwnd, true, window_id);
    if (hook != NULL && hook->old_proc == NULL) {
      hook->old_proc = (WNDPROC)SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)native_backend_window_proc);
      native_backend_register_raw_input(hwnd);
    }
  }
#else
  (void)window_id;
#endif
  profile_func_end;
}

func void input_native_backend_on_window_destroyed(window window_id) {
  profile_func_begin;
  if (!native_backend_initialized) {
    profile_func_end;
    return;
  }
#if defined(_WIN32)
  HWND hwnd = native_backend_get_window_hwnd(window_id);
  if (hwnd != NULL) {
    native_window_hook* hook = native_backend_find_hook(hwnd, false, NULL);
    if (hook != NULL && hook->old_proc != NULL) {
      SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)hook->old_proc);
      *hook = (native_window_hook) {0};
    }
  }
#else
  (void)window_id;
#endif
  profile_func_end;
}
