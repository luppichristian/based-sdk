// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/bindings.h"

typedef struct binding_entry {
  b32 used;
  b32 is_down;
  u32 pressed_generation;
  u32 released_generation;
  binding_desc desc;
} binding_entry;

global_var binding_entry bindings_entries[BINDINGS_MAX_COUNT] = {0};
global_var u32 bindings_pressed_seen[INPUT_CAPTURE_MAX_KEYS][BINDINGS_MAX_COUNT] = {0};
global_var u32 bindings_released_seen[INPUT_CAPTURE_MAX_KEYS][BINDINGS_MAX_COUNT] = {0};
global_var u32 bindings_pressed_seen_epoch[INPUT_CAPTURE_MAX_KEYS][BINDINGS_MAX_COUNT] = {0};
global_var u32 bindings_released_seen_epoch[INPUT_CAPTURE_MAX_KEYS][BINDINGS_MAX_COUNT] = {0};

func u32 bindings_next_generation(u32 value) {
  u32 result = value + 1;
  if (result == 0) {
    result = 1;
  }
  return result;
}

func sz bindings_find_slot_by_id(u32 binding_id) {
  for (sz slot_index = 0; slot_index < BINDINGS_MAX_COUNT; slot_index += 1) {
    if (bindings_entries[slot_index].used && bindings_entries[slot_index].desc.binding_id == binding_id) {
      return slot_index;
    }
  }

  return BINDINGS_MAX_COUNT;
}

func sz bindings_find_free_slot(void) {
  for (sz slot_index = 0; slot_index < BINDINGS_MAX_COUNT; slot_index += 1) {
    if (!bindings_entries[slot_index].used) {
      return slot_index;
    }
  }

  return BINDINGS_MAX_COUNT;
}

func b32 bindings_eval_keyboard(const binding_keyboard_combo* combo) {
  if (!combo || combo->count == 0) {
    return 1;
  }

  if (combo->count > BINDING_MAX_KEYS) {
    return 0;
  }

  if (!keyboard_has_mods_exact(combo->required_mods, combo->forbidden_mods)) {
    return 0;
  }

  for (sz item_index = 0; item_index < combo->count; item_index += 1) {
    if (!keyboard_is_key_down(INPUT_KEY_DEFAULT, combo->scancodes[item_index])) {
      return 0;
    }
  }

  return 1;
}

func b32 bindings_eval_mouse(const binding_mouse_combo* combo) {
  if (!combo || combo->count == 0) {
    return 1;
  }

  if (combo->count > BINDING_MAX_MOUSE_BUTTONS) {
    return 0;
  }

  for (sz item_index = 0; item_index < combo->count; item_index += 1) {
    if (!mouse_is_button_down(INPUT_KEY_DEFAULT, combo->buttons[item_index])) {
      return 0;
    }
  }

  return 1;
}

func b32 bindings_eval_gamepad_slot(const binding_gamepad_combo* combo, sz slot_index) {
  if (!gamepads_is_connected(slot_index)) {
    return 0;
  }

  for (sz item_index = 0; item_index < combo->count; item_index += 1) {
    if (!gamepads_get_button(INPUT_KEY_DEFAULT, slot_index, combo->buttons[item_index])) {
      return 0;
    }
  }

  return 1;
}

func b32 bindings_eval_gamepad(const binding_gamepad_combo* combo) {
  if (!combo || combo->count == 0) {
    return 1;
  }

  if (combo->count > BINDING_MAX_GAMEPAD_BUTTONS) {
    return 0;
  }

  if (combo->any_slot) {
    for (sz slot_index = 0; slot_index < GAMEPADS_MAX_COUNT; slot_index += 1) {
      if (bindings_eval_gamepad_slot(combo, slot_index)) {
        return 1;
      }
    }

    return 0;
  }

  if (combo->slot_index >= GAMEPADS_MAX_COUNT) {
    return 0;
  }

  return bindings_eval_gamepad_slot(combo, combo->slot_index);
}

func b32 bindings_desc_has_any_combo(const binding_desc* desc) {
  if (!desc) {
    return 0;
  }

  return desc->keyboard.count > 0 || desc->mouse.count > 0 || desc->gamepad.count > 0;
}

func b32 bindings_eval_desc(const binding_desc* desc) {
  if (!desc || !desc->enabled || !bindings_desc_has_any_combo(desc)) {
    return 0;
  }

  if (!bindings_eval_keyboard(&desc->keyboard)) {
    return 0;
  }

  if (!bindings_eval_mouse(&desc->mouse)) {
    return 0;
  }

  if (!bindings_eval_gamepad(&desc->gamepad)) {
    return 0;
  }

  return 1;
}

func void bindings_refresh_all_transitions(void) {
  for (sz slot_index = 0; slot_index < BINDINGS_MAX_COUNT; slot_index += 1) {
    if (!bindings_entries[slot_index].used) {
      continue;
    }

    b32 new_down = bindings_eval_desc(&bindings_entries[slot_index].desc);
    b32 old_down = bindings_entries[slot_index].is_down;

    if (new_down && !old_down) {
      bindings_entries[slot_index].pressed_generation =
        bindings_next_generation(bindings_entries[slot_index].pressed_generation);
    } else if (!new_down && old_down) {
      bindings_entries[slot_index].released_generation =
        bindings_next_generation(bindings_entries[slot_index].released_generation);
    }

    bindings_entries[slot_index].is_down = new_down;
  }
}

func b32 bindings_msg_affects_state(u32 type) {
  switch (type) {
    case MSG_TYPE_KEY_DOWN:
    case MSG_TYPE_KEY_UP:
    case MSG_TYPE_KEYMAP_CHANGED:
    case MSG_TYPE_MOUSE_BUTTON_DOWN:
    case MSG_TYPE_MOUSE_BUTTON_UP:
    case MSG_TYPE_GAMEPAD_BUTTON_DOWN:
    case MSG_TYPE_GAMEPAD_BUTTON_UP:
    case MSG_TYPE_GAMEPAD_ADDED:
    case MSG_TYPE_GAMEPAD_REMOVED:
    case MSG_TYPE_GAMEPAD_REMAPPED:
    case MSG_TYPE_GAMEPAD_UPDATE_COMPLETE:
    case MSG_TYPE_WINDOW_FOCUS_LOST:
      return 1;

    default:
      break;
  }

  return 0;
}

func b32 bindings_read_pressed_for_key_slot(sz binding_slot_index, sz key_slot_index) {
  u32 slot_epoch = input_capture_get_slot_epoch(key_slot_index);

  if (bindings_pressed_seen_epoch[key_slot_index][binding_slot_index] != slot_epoch) {
    bindings_pressed_seen_epoch[key_slot_index][binding_slot_index] = slot_epoch;
    bindings_pressed_seen[key_slot_index][binding_slot_index] = bindings_entries[binding_slot_index].pressed_generation;
    return 0;
  }

  u32 generation = bindings_entries[binding_slot_index].pressed_generation;
  if (generation == 0 || bindings_pressed_seen[key_slot_index][binding_slot_index] == generation) {
    return 0;
  }

  bindings_pressed_seen[key_slot_index][binding_slot_index] = generation;
  return 1;
}

func b32 bindings_read_released_for_key_slot(sz binding_slot_index, sz key_slot_index) {
  u32 slot_epoch = input_capture_get_slot_epoch(key_slot_index);

  if (bindings_released_seen_epoch[key_slot_index][binding_slot_index] != slot_epoch) {
    bindings_released_seen_epoch[key_slot_index][binding_slot_index] = slot_epoch;
    bindings_released_seen[key_slot_index][binding_slot_index] = bindings_entries[binding_slot_index].released_generation;
    return 0;
  }

  u32 generation = bindings_entries[binding_slot_index].released_generation;
  if (generation == 0 || bindings_released_seen[key_slot_index][binding_slot_index] == generation) {
    return 0;
  }

  bindings_released_seen[key_slot_index][binding_slot_index] = generation;
  return 1;
}

func void bindings_clear(void) {
  for (sz slot_index = 0; slot_index < BINDINGS_MAX_COUNT; slot_index += 1) {
    bindings_entries[slot_index] = (binding_entry) {0};
  }
}

func b32 bindings_add(const binding_desc* desc) {
  if (!desc || desc->binding_id == 0 || !bindings_desc_has_any_combo(desc)) {
    return 0;
  }

  if (desc->keyboard.count > BINDING_MAX_KEYS || desc->mouse.count > BINDING_MAX_MOUSE_BUTTONS ||
      desc->gamepad.count > BINDING_MAX_GAMEPAD_BUTTONS) {
    return 0;
  }

  sz slot_index = bindings_find_slot_by_id(desc->binding_id);
  if (slot_index >= BINDINGS_MAX_COUNT) {
    slot_index = bindings_find_free_slot();
    if (slot_index >= BINDINGS_MAX_COUNT) {
      return 0;
    }
  }

  bindings_entries[slot_index] = (binding_entry) {
      .used = 1,
      .desc = *desc,
      .is_down = 0,
      .pressed_generation = 0,
      .released_generation = 0,
  };

  bindings_entries[slot_index].is_down = bindings_eval_desc(&bindings_entries[slot_index].desc);
  return 1;
}

func b32 bindings_remove(u32 binding_id) {
  if (binding_id == 0) {
    return 0;
  }

  sz slot_index = bindings_find_slot_by_id(binding_id);
  if (slot_index >= BINDINGS_MAX_COUNT) {
    return 0;
  }

  bindings_entries[slot_index] = (binding_entry) {0};
  return 1;
}

func b32 bindings_has(u32 binding_id) {
  return bindings_find_slot_by_id(binding_id) < BINDINGS_MAX_COUNT;
}

func b32 bindings_set_enabled(u32 binding_id, b32 enabled) {
  sz slot_index = bindings_find_slot_by_id(binding_id);
  if (slot_index >= BINDINGS_MAX_COUNT) {
    return 0;
  }

  bindings_entries[slot_index].desc.enabled = enabled ? 1 : 0;
  bindings_refresh_all_transitions();
  return 1;
}

func b32 bindings_is_down(u32 binding_id) {
  sz slot_index = bindings_find_slot_by_id(binding_id);
  if (slot_index >= BINDINGS_MAX_COUNT) {
    return 0;
  }

  return bindings_entries[slot_index].is_down;
}

func b32 bindings_is_pressed(input_key key, u32 binding_id) {
  sz binding_slot_index = bindings_find_slot_by_id(binding_id);
  if (binding_slot_index >= BINDINGS_MAX_COUNT) {
    return 0;
  }

  sz key_slot_index = input_capture_get_slot(key);
  if (key_slot_index >= INPUT_CAPTURE_MAX_KEYS) {
    return 0;
  }

  return bindings_read_pressed_for_key_slot(binding_slot_index, key_slot_index);
}

func b32 bindings_is_released(input_key key, u32 binding_id) {
  sz binding_slot_index = bindings_find_slot_by_id(binding_id);
  if (binding_slot_index >= BINDINGS_MAX_COUNT) {
    return 0;
  }

  sz key_slot_index = input_capture_get_slot(key);
  if (key_slot_index >= INPUT_CAPTURE_MAX_KEYS) {
    return 0;
  }

  return bindings_read_released_for_key_slot(binding_slot_index, key_slot_index);
}

func void bindings_on_msg(const msg* src) {
  if (!src || !bindings_msg_affects_state(src->type)) {
    return;
  }

  bindings_refresh_all_transitions();
}

func void bindings_internal_on_msg(const msg* src) {
  bindings_on_msg(src);
}
