// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/input_state.h"
#include "basic/assert.h"
#include "input/msg_core.h"

#include <string.h>

typedef struct input_state_blob_header {
  u32 magic;
  u32 version;
  u32 payload_size;
  u32 reserved;
} input_state_blob_header;

const_var u32 INPUT_STATE_SERIALIZATION_MAGIC = 0x49535441u;
const_var u32 INPUT_STATE_SERIALIZATION_VERSION = 1u;

func sz input_state_find_gamepad_slot(device_id device) {
  if (device.type != DEVICE_TYPE_GAMEPAD) {
    return GAMEPADS_MAX_COUNT;
  }

  for (sz slot_idx = 0; slot_idx < GAMEPADS_MAX_COUNT; slot_idx += 1) {
    device_id slot_id = {0};
    if (!gamepads_get_device_id(slot_idx, &slot_id)) {
      continue;
    }

    if (slot_id.instance == device.instance) {
      return slot_idx;
    }
  }

  return GAMEPADS_MAX_COUNT;
}

func void input_state_sync_gamepads(input_state* src, input_key key) {
  for (sz slot_idx = 0; slot_idx < GAMEPADS_MAX_COUNT; slot_idx += 1) {
    b32 connected = gamepads_is_connected(slot_idx);
    src->gamepad_connected[slot_idx] = connected;

    for (sz button_idx = 0; button_idx < GAMEPAD_BUTTON_COUNT; button_idx += 1) {
      src->gamepad_button_down[slot_idx][button_idx] =
          connected ? gamepads_get_button(key, slot_idx, (gamepad_button)button_idx) : 0;
    }

    for (sz axis_idx = 0; axis_idx < GAMEPAD_AXIS_COUNT; axis_idx += 1) {
      src->gamepad_axis[slot_idx][axis_idx] =
          connected ? gamepads_get_axis(key, slot_idx, (gamepad_axis)axis_idx) : 0;
    }
  }
}

func void input_state_sync_mouse_buttons_from_mask(input_state* src) {
  for (sz button_idx = 0; button_idx < INPUT_STATE_MOUSE_BUTTON_CAP; button_idx += 1) {
    if (button_idx == 0) {
      src->mouse_button_down[button_idx] = 0;
      continue;
    }

    src->mouse_button_down[button_idx] = (src->mouse_button_mask & (1u << (button_idx - 1))) != 0 ? 1 : 0;
  }
}

func void input_state_clear(input_state* src) {
  if (!src) {
    return;
  }
  assert(src != NULL);

  *src = (input_state) {0};
}

func b32 input_state_capture(input_key key, input_state* out_state) {
  if (!out_state) {
    return 0;
  }
  assert(out_state != NULL);

  *out_state = (input_state) {0};

  out_state->keyboard_available = keyboard_is_available();
  out_state->mouse_available = mouse_is_available();
  out_state->keyboard_mods = (u16)keyboard_get_mods();

  for (sz scancode_idx = 0; scancode_idx < INPUT_STATE_KEY_CAP; scancode_idx += 1) {
    out_state->keyboard_down[scancode_idx] = keyboard_is_key_down(key, (keyboard_scancode)scancode_idx);
    out_state->keyboard_repeat[scancode_idx] = keyboard_get_key_repeat_count(key, (keyboard_scancode)scancode_idx);
  }

  mouse_state local_state = mouse_get_state(key);
  mouse_state global_state = mouse_get_global_state(key);
  mouse_state relative_state = mouse_get_relative_state(key);

  out_state->mouse_x = local_state.x;
  out_state->mouse_y = local_state.y;
  out_state->mouse_global_x = global_state.x;
  out_state->mouse_global_y = global_state.y;
  out_state->mouse_relative_x = relative_state.x;
  out_state->mouse_relative_y = relative_state.y;
  out_state->mouse_button_mask = local_state.button_mask;
  input_state_sync_mouse_buttons_from_mask(out_state);

  input_state_sync_gamepads(out_state, key);
  return 1;
}

func void input_state_apply_keyboard_msg(input_state* src, const msg* event_msg) {
  if (event_msg->type != MSG_CORE_TYPE_KEY_DOWN && event_msg->type != MSG_CORE_TYPE_KEY_UP) {
    return;
  }
  msg* event_msg_mut = (msg*)event_msg;

  if (msg_core_get_keyboard(event_msg_mut)->scancode >= INPUT_STATE_KEY_CAP) {
    return;
  }

  sz scancode_idx = (sz)msg_core_get_keyboard(event_msg_mut)->scancode;
  src->keyboard_available = 1;
  src->keyboard_mods = (u16)msg_core_get_keyboard(event_msg_mut)->modifiers;

  if (event_msg->type == MSG_CORE_TYPE_KEY_DOWN) {
    src->keyboard_down[scancode_idx] = 1;
    if (msg_core_get_keyboard(event_msg_mut)->repeat) {
      src->keyboard_repeat[scancode_idx] += 1;
    } else {
      src->keyboard_repeat[scancode_idx] = 0;
    }
  } else {
    src->keyboard_down[scancode_idx] = 0;
    src->keyboard_repeat[scancode_idx] = 0;
  }
}

func void input_state_apply_mouse_msg(input_state* src, const msg* event_msg) {
  msg* event_msg_mut = (msg*)event_msg;

  if (event_msg->type == MSG_CORE_TYPE_MOUSE_ADDED) {
    src->mouse_available = 1;
    return;
  }

  if (event_msg->type == MSG_CORE_TYPE_MOUSE_REMOVED) {
    src->mouse_available = 0;
    src->mouse_button_mask = 0;
    input_state_sync_mouse_buttons_from_mask(src);
    return;
  }

  if (event_msg->type == MSG_CORE_TYPE_MOUSE_MOTION) {
    src->mouse_available = 1;
    src->mouse_x = msg_core_get_mouse_motion(event_msg_mut)->x;
    src->mouse_y = msg_core_get_mouse_motion(event_msg_mut)->y;
    src->mouse_relative_x = msg_core_get_mouse_motion(event_msg_mut)->xrel;
    src->mouse_relative_y = msg_core_get_mouse_motion(event_msg_mut)->yrel;
    src->mouse_button_mask = msg_core_get_mouse_motion(event_msg_mut)->button_mask;
    input_state_sync_mouse_buttons_from_mask(src);
    return;
  }

  if (event_msg->type != MSG_CORE_TYPE_MOUSE_BUTTON_DOWN && event_msg->type != MSG_CORE_TYPE_MOUSE_BUTTON_UP) {
    return;
  }

  src->mouse_available = 1;
  i32 button_value = (i32)msg_core_get_mouse_button(event_msg_mut)->button;
  if (button_value > 0 && (sz)button_value < INPUT_STATE_MOUSE_BUTTON_CAP) {
    sz button_idx = (sz)button_value;
    b32 down_state = event_msg->type == MSG_CORE_TYPE_MOUSE_BUTTON_DOWN ? 1 : 0;
    src->mouse_button_down[button_idx] = down_state;

    if (down_state) {
      src->mouse_button_mask |= (1u << (button_idx - 1));
    } else {
      src->mouse_button_mask &= ~(1u << (button_idx - 1));
    }
  }

  src->mouse_x = msg_core_get_mouse_button(event_msg_mut)->x;
  src->mouse_y = msg_core_get_mouse_button(event_msg_mut)->y;
}

func void input_state_apply_gamepad_msg(input_state* src, const msg* event_msg) {
  msg* event_msg_mut = (msg*)event_msg;

  if (event_msg->type == MSG_CORE_TYPE_GAMEPAD_ADDED || event_msg->type == MSG_CORE_TYPE_GAMEPAD_REMOVED ||
      event_msg->type == MSG_CORE_TYPE_GAMEPAD_REMAPPED || event_msg->type == MSG_CORE_TYPE_GAMEPAD_UPDATE_COMPLETE) {
    input_state_sync_gamepads(src, INPUT_KEY_DEFAULT);
    return;
  }

  if (event_msg->type == MSG_CORE_TYPE_GAMEPAD_BUTTON_DOWN || event_msg->type == MSG_CORE_TYPE_GAMEPAD_BUTTON_UP) {
    sz slot_idx = input_state_find_gamepad_slot(msg_core_get_gamepad_button(event_msg_mut)->device);
    i32 button_value = (i32)msg_core_get_gamepad_button(event_msg_mut)->button;
    if (slot_idx >= GAMEPADS_MAX_COUNT || button_value < 0 || button_value >= GAMEPAD_BUTTON_COUNT) {
      return;
    }

    src->gamepad_connected[slot_idx] = 1;
    src->gamepad_button_down[slot_idx][(sz)button_value] = event_msg->type == MSG_CORE_TYPE_GAMEPAD_BUTTON_DOWN ? 1 : 0;
    return;
  }

  if (event_msg->type == MSG_CORE_TYPE_GAMEPAD_AXIS_MOTION) {
    sz slot_idx = input_state_find_gamepad_slot(msg_core_get_gamepad_axis(event_msg_mut)->device);
    i32 axis_value = (i32)msg_core_get_gamepad_axis(event_msg_mut)->axis;
    if (slot_idx >= GAMEPADS_MAX_COUNT || axis_value < 0 || axis_value >= GAMEPAD_AXIS_COUNT) {
      return;
    }

    src->gamepad_connected[slot_idx] = 1;
    src->gamepad_axis[slot_idx][(sz)axis_value] = msg_core_get_gamepad_axis(event_msg_mut)->value;
  }
}

func void input_state_apply_msg(input_state* src, const msg* event_msg) {
  if (!src || !event_msg) {
    return;
  }
  assert(src != NULL);
  assert(event_msg != NULL);

  src->timestamp = event_msg->timestamp;

  if (event_msg->type == MSG_CORE_TYPE_KEYBOARD_ADDED) {
    src->keyboard_available = 1;
  } else if (event_msg->type == MSG_CORE_TYPE_KEYBOARD_REMOVED) {
    src->keyboard_available = 0;
  }

  input_state_apply_keyboard_msg(src, event_msg);
  input_state_apply_mouse_msg(src, event_msg);
  input_state_apply_gamepad_msg(src, event_msg);
}

func sz input_state_serialized_size(void) {
  return size_of(input_state_blob_header) + size_of(input_state);
}

func b32 input_state_serialize(const input_state* src, void* dst, sz dst_cap, sz* out_size) {
  if (out_size) {
    *out_size = 0;
  }

  if (!src || !dst) {
    return 0;
  }
  assert(src != NULL);
  assert(dst != NULL);

  sz total_size = input_state_serialized_size();
  if (dst_cap < total_size) {
    return 0;
  }

  input_state_blob_header header = {
      .magic = INPUT_STATE_SERIALIZATION_MAGIC,
      .version = INPUT_STATE_SERIALIZATION_VERSION,
      .payload_size = (u32)size_of(input_state),
      .reserved = 0,
  };

  memcpy(dst, &header, size_of(header));
  memcpy((u8*)dst + size_of(header), src, size_of(input_state));

  if (out_size) {
    *out_size = total_size;
  }

  return 1;
}

func b32 input_state_deserialize(const void* src, sz src_size, input_state* out_state) {
  if (!src || !out_state || src_size < size_of(input_state_blob_header) + size_of(input_state)) {
    return 0;
  }
  assert(src != NULL);
  assert(out_state != NULL);

  input_state_blob_header header = {0};
  memcpy(&header, src, size_of(header));

  if (header.magic != INPUT_STATE_SERIALIZATION_MAGIC || header.version != INPUT_STATE_SERIALIZATION_VERSION ||
      header.payload_size != (u32)size_of(input_state)) {
    return 0;
  }

  memcpy(out_state, (const u8*)src + size_of(header), size_of(input_state));
  return 1;
}
