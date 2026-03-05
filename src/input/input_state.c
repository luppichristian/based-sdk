// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/input_state.h"

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

  for (sz slot_index = 0; slot_index < GAMEPADS_MAX_COUNT; slot_index += 1) {
    device_id slot_id = {0};
    if (!gamepads_get_device_id(slot_index, &slot_id)) {
      continue;
    }

    if (slot_id.instance == device.instance) {
      return slot_index;
    }
  }

  return GAMEPADS_MAX_COUNT;
}

func void input_state_sync_gamepads(input_state* src, input_key key) {
  for (sz slot_index = 0; slot_index < GAMEPADS_MAX_COUNT; slot_index += 1) {
    b32 connected = gamepads_is_connected(slot_index);
    src->gamepad_connected[slot_index] = connected;

    for (sz button_index = 0; button_index < GAMEPAD_BUTTON_COUNT; button_index += 1) {
      src->gamepad_button_down[slot_index][button_index] =
        connected ? gamepads_get_button(key, slot_index, (gamepad_button)button_index) : 0;
    }

    for (sz axis_index = 0; axis_index < GAMEPAD_AXIS_COUNT; axis_index += 1) {
      src->gamepad_axis[slot_index][axis_index] =
        connected ? gamepads_get_axis(key, slot_index, (gamepad_axis)axis_index) : 0;
    }
  }
}

func void input_state_sync_mouse_buttons_from_mask(input_state* src) {
  for (sz button_index = 0; button_index < INPUT_STATE_MOUSE_BUTTON_CAP; button_index += 1) {
    if (button_index == 0) {
      src->mouse_button_down[button_index] = 0;
      continue;
    }

    src->mouse_button_down[button_index] = (src->mouse_button_mask & (1u << (button_index - 1))) != 0 ? 1 : 0;
  }
}

func void input_state_clear(input_state* src) {
  if (!src) {
    return;
  }

  *src = (input_state) {0};
}

func b32 input_state_capture(input_key key, input_state* out_state) {
  if (!out_state) {
    return 0;
  }

  *out_state = (input_state) {0};

  out_state->keyboard_available = keyboard_is_available();
  out_state->mouse_available = mouse_is_available();
  out_state->keyboard_mods = keyboard_get_mods();

  for (sz scancode_index = 0; scancode_index < INPUT_STATE_KEY_CAP; scancode_index += 1) {
    out_state->keyboard_down[scancode_index] = keyboard_is_key_down(key, (u32)scancode_index);
    out_state->keyboard_repeat[scancode_index] = keyboard_get_key_repeat_count(key, (u32)scancode_index);
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
  if (event_msg->type != MSG_TYPE_KEY_DOWN && event_msg->type != MSG_TYPE_KEY_UP) {
    return;
  }

  if (event_msg->keyboard.scancode >= INPUT_STATE_KEY_CAP) {
    return;
  }

  sz scancode_index = (sz)event_msg->keyboard.scancode;
  src->keyboard_available = 1;
  src->keyboard_mods = event_msg->keyboard.modifiers;

  if (event_msg->type == MSG_TYPE_KEY_DOWN) {
    src->keyboard_down[scancode_index] = 1;
    if (event_msg->keyboard.repeat) {
      src->keyboard_repeat[scancode_index] += 1;
    } else {
      src->keyboard_repeat[scancode_index] = 0;
    }
  } else {
    src->keyboard_down[scancode_index] = 0;
    src->keyboard_repeat[scancode_index] = 0;
  }
}

func void input_state_apply_mouse_msg(input_state* src, const msg* event_msg) {
  if (event_msg->type == MSG_TYPE_MOUSE_ADDED) {
    src->mouse_available = 1;
    return;
  }

  if (event_msg->type == MSG_TYPE_MOUSE_REMOVED) {
    src->mouse_available = 0;
    src->mouse_button_mask = 0;
    input_state_sync_mouse_buttons_from_mask(src);
    return;
  }

  if (event_msg->type == MSG_TYPE_MOUSE_MOTION) {
    src->mouse_available = 1;
    src->mouse_x = event_msg->mouse_motion.x;
    src->mouse_y = event_msg->mouse_motion.y;
    src->mouse_relative_x = event_msg->mouse_motion.xrel;
    src->mouse_relative_y = event_msg->mouse_motion.yrel;
    src->mouse_button_mask = event_msg->mouse_motion.button_mask;
    input_state_sync_mouse_buttons_from_mask(src);
    return;
  }

  if (event_msg->type != MSG_TYPE_MOUSE_BUTTON_DOWN && event_msg->type != MSG_TYPE_MOUSE_BUTTON_UP) {
    return;
  }

  src->mouse_available = 1;
  i32 button_value = (i32)event_msg->mouse_button.button;
  if (button_value > 0 && (sz)button_value < INPUT_STATE_MOUSE_BUTTON_CAP) {
    sz button_index = (sz)button_value;
    b32 down_state = event_msg->type == MSG_TYPE_MOUSE_BUTTON_DOWN ? 1 : 0;
    src->mouse_button_down[button_index] = down_state;

    if (down_state) {
      src->mouse_button_mask |= (1u << (button_index - 1));
    } else {
      src->mouse_button_mask &= ~(1u << (button_index - 1));
    }
  }

  src->mouse_x = event_msg->mouse_button.x;
  src->mouse_y = event_msg->mouse_button.y;
}

func void input_state_apply_gamepad_msg(input_state* src, const msg* event_msg) {
  if (event_msg->type == MSG_TYPE_GAMEPAD_ADDED || event_msg->type == MSG_TYPE_GAMEPAD_REMOVED ||
      event_msg->type == MSG_TYPE_GAMEPAD_REMAPPED || event_msg->type == MSG_TYPE_GAMEPAD_UPDATE_COMPLETE) {
    input_state_sync_gamepads(src, INPUT_KEY_DEFAULT);
    return;
  }

  if (event_msg->type == MSG_TYPE_GAMEPAD_BUTTON_DOWN || event_msg->type == MSG_TYPE_GAMEPAD_BUTTON_UP) {
    sz slot_index = input_state_find_gamepad_slot(event_msg->gamepad_button.device);
    i32 button_value = (i32)event_msg->gamepad_button.button;
    if (slot_index >= GAMEPADS_MAX_COUNT || button_value < 0 || button_value >= GAMEPAD_BUTTON_COUNT) {
      return;
    }

    src->gamepad_connected[slot_index] = 1;
    src->gamepad_button_down[slot_index][(sz)button_value] = event_msg->type == MSG_TYPE_GAMEPAD_BUTTON_DOWN ? 1 : 0;
    return;
  }

  if (event_msg->type == MSG_TYPE_GAMEPAD_AXIS_MOTION) {
    sz slot_index = input_state_find_gamepad_slot(event_msg->gamepad_axis.device);
    i32 axis_value = (i32)event_msg->gamepad_axis.axis;
    if (slot_index >= GAMEPADS_MAX_COUNT || axis_value < 0 || axis_value >= GAMEPAD_AXIS_COUNT) {
      return;
    }

    src->gamepad_connected[slot_index] = 1;
    src->gamepad_axis[slot_index][(sz)axis_value] = event_msg->gamepad_axis.value;
  }
}

func void input_state_apply_msg(input_state* src, const msg* event_msg) {
  if (!src || !event_msg) {
    return;
  }

  src->timestamp = event_msg->timestamp;

  if (event_msg->type == MSG_TYPE_KEYBOARD_ADDED) {
    src->keyboard_available = 1;
  } else if (event_msg->type == MSG_TYPE_KEYBOARD_REMOVED) {
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

  input_state_blob_header header = {0};
  memcpy(&header, src, size_of(header));

  if (header.magic != INPUT_STATE_SERIALIZATION_MAGIC || header.version != INPUT_STATE_SERIALIZATION_VERSION ||
      header.payload_size != (u32)size_of(input_state)) {
    return 0;
  }

  memcpy(out_state, (const u8*)src + size_of(header), size_of(input_state));
  return 1;
}
