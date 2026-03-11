// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/bindings.h"
#include "basic/assert.h"
#include "input/msg_core.h"
#include "basic/profiler.h"

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
  profile_func_begin;
  u32 result = value + 1;
  if (result == 0) {
    result = 1;
  }
  profile_func_end;
  return result;
}

func sz bindings_find_slot_by_id(u32 binding_id) {
  profile_func_begin;
  for (sz slot_idx = 0; slot_idx < BINDINGS_MAX_COUNT; slot_idx += 1) {
    if (bindings_entries[slot_idx].used && bindings_entries[slot_idx].desc.binding_id == binding_id) {
      profile_func_end;
      return slot_idx;
    }
  }

  profile_func_end;
  return BINDINGS_MAX_COUNT;
}

func sz bindings_find_free_slot(void) {
  profile_func_begin;
  for (sz slot_idx = 0; slot_idx < BINDINGS_MAX_COUNT; slot_idx += 1) {
    if (!bindings_entries[slot_idx].used) {
      profile_func_end;
      return slot_idx;
    }
  }

  profile_func_end;
  return BINDINGS_MAX_COUNT;
}

func b32 bindings_eval_keyboard(const binding_keyboard_combo* combo) {
  profile_func_begin;
  if (!combo || combo->count == 0) {
    profile_func_end;
    return true;
  }

  if (combo->count > BINDING_MAX_KEYS) {
    profile_func_end;
    return false;
  }

  if (!keyboard_has_mods_exact(combo->required_mods, combo->forbidden_mods)) {
    profile_func_end;
    return false;
  }

  for (sz item_idx = 0; item_idx < combo->count; item_idx += 1) {
    if (!keyboard_is_key_down(INPUT_KEY_DEFAULT, combo->scancodes[item_idx])) {
      profile_func_end;
      return false;
    }
  }

  profile_func_end;
  return true;
}

func b32 bindings_eval_mouse(const binding_mouse_combo* combo) {
  profile_func_begin;
  if (!combo || combo->count == 0) {
    profile_func_end;
    return true;
  }

  if (combo->count > BINDING_MAX_MOUSE_BUTTONS) {
    profile_func_end;
    return false;
  }

  for (sz item_idx = 0; item_idx < combo->count; item_idx += 1) {
    if (!mouse_is_button_down(INPUT_KEY_DEFAULT, combo->buttons[item_idx])) {
      profile_func_end;
      return false;
    }
  }

  profile_func_end;
  return true;
}

func b32 bindings_eval_gamepad_slot(const binding_gamepad_combo* combo, sz slot_idx) {
  profile_func_begin;
  if (!gamepads_is_connected(slot_idx)) {
    profile_func_end;
    return false;
  }

  for (sz item_idx = 0; item_idx < combo->count; item_idx += 1) {
    if (!gamepads_get_button(INPUT_KEY_DEFAULT, slot_idx, combo->buttons[item_idx])) {
      profile_func_end;
      return false;
    }
  }

  profile_func_end;
  return true;
}

func b32 bindings_eval_gamepad(const binding_gamepad_combo* combo) {
  profile_func_begin;
  if (!combo || combo->count == 0) {
    profile_func_end;
    return true;
  }

  if (combo->count > BINDING_MAX_GAMEPAD_BUTTONS) {
    profile_func_end;
    return false;
  }

  if (combo->any_slot) {
    for (sz slot_idx = 0; slot_idx < GAMEPADS_MAX_COUNT; slot_idx += 1) {
      if (bindings_eval_gamepad_slot(combo, slot_idx)) {
        profile_func_end;
        return true;
      }
    }

    profile_func_end;
    return false;
  }

  if (combo->slot_idx >= GAMEPADS_MAX_COUNT) {
    profile_func_end;
    return false;
  }

  profile_func_end;
  return bindings_eval_gamepad_slot(combo, combo->slot_idx);
}

func b32 bindings_desc_has_any_combo(const binding_desc* desc) {
  if (!desc) {
      return false;
  }

  return desc->keyboard.count > 0 || desc->mouse.count > 0 || desc->gamepad.count > 0;
}

func b32 bindings_eval_desc(const binding_desc* desc) {
  profile_func_begin;
  if (!desc || !desc->enabled || !bindings_desc_has_any_combo(desc)) {
    profile_func_end;
    return false;
  }

  if (!bindings_eval_keyboard(&desc->keyboard)) {
    profile_func_end;
    return false;
  }

  if (!bindings_eval_mouse(&desc->mouse)) {
    profile_func_end;
    return false;
  }

  if (!bindings_eval_gamepad(&desc->gamepad)) {
    profile_func_end;
    return false;
  }

  profile_func_end;
  return true;
}

func void bindings_refresh_all_transitions(void) {
  profile_func_begin;
  for (sz slot_idx = 0; slot_idx < BINDINGS_MAX_COUNT; slot_idx += 1) {
    if (!bindings_entries[slot_idx].used) {
      continue;
    }

    b32 new_down = bindings_eval_desc(&bindings_entries[slot_idx].desc);
    b32 old_down = bindings_entries[slot_idx].is_down;

    if (new_down && !old_down) {
      bindings_entries[slot_idx].pressed_generation =
          bindings_next_generation(bindings_entries[slot_idx].pressed_generation);
    } else if (!new_down && old_down) {
      bindings_entries[slot_idx].released_generation =
          bindings_next_generation(bindings_entries[slot_idx].released_generation);
    }

    bindings_entries[slot_idx].is_down = new_down;
  }
  profile_func_end;
}

func b32 bindings_msg_affects_state(u32 type) {
  profile_func_begin;
  switch (type) {
    case MSG_CORE_TYPE_KEY_DOWN:
    case MSG_CORE_TYPE_KEY_UP:
    case MSG_CORE_TYPE_KEYMAP_CHANGED:
    case MSG_CORE_TYPE_MOUSE_BUTTON_DOWN:
    case MSG_CORE_TYPE_MOUSE_BUTTON_UP:
    case MSG_CORE_TYPE_GAMEPAD_BUTTON_DOWN:
    case MSG_CORE_TYPE_GAMEPAD_BUTTON_UP:
    case MSG_CORE_TYPE_GAMEPAD_ADDED:
    case MSG_CORE_TYPE_GAMEPAD_REMOVED:
    case MSG_CORE_TYPE_GAMEPAD_REMAPPED:
    case MSG_CORE_TYPE_GAMEPAD_UPDATE_COMPLETE:
    case MSG_CORE_TYPE_WINDOW_FOCUS_LOST:
      profile_func_end;
      return true;

    default:
      break;
  }

  profile_func_end;
  return false;
}

func b32 bindings_read_pressed_for_key_slot(sz binding_slot_idx, sz key_slot_idx) {
  profile_func_begin;
  u32 slot_epoch = input_capture_get_slot_epoch(key_slot_idx);

  if (bindings_pressed_seen_epoch[key_slot_idx][binding_slot_idx] != slot_epoch) {
    bindings_pressed_seen_epoch[key_slot_idx][binding_slot_idx] = slot_epoch;
    bindings_pressed_seen[key_slot_idx][binding_slot_idx] = bindings_entries[binding_slot_idx].pressed_generation;
    profile_func_end;
    return false;
  }

  u32 generation = bindings_entries[binding_slot_idx].pressed_generation;
  if (generation == 0 || bindings_pressed_seen[key_slot_idx][binding_slot_idx] == generation) {
    profile_func_end;
    return false;
  }

  bindings_pressed_seen[key_slot_idx][binding_slot_idx] = generation;
  profile_func_end;
  return true;
}

func b32 bindings_read_released_for_key_slot(sz binding_slot_idx, sz key_slot_idx) {
  profile_func_begin;
  u32 slot_epoch = input_capture_get_slot_epoch(key_slot_idx);

  if (bindings_released_seen_epoch[key_slot_idx][binding_slot_idx] != slot_epoch) {
    bindings_released_seen_epoch[key_slot_idx][binding_slot_idx] = slot_epoch;
    bindings_released_seen[key_slot_idx][binding_slot_idx] = bindings_entries[binding_slot_idx].released_generation;
    profile_func_end;
    return false;
  }

  u32 generation = bindings_entries[binding_slot_idx].released_generation;
  if (generation == 0 || bindings_released_seen[key_slot_idx][binding_slot_idx] == generation) {
    profile_func_end;
    return false;
  }

  bindings_released_seen[key_slot_idx][binding_slot_idx] = generation;
  profile_func_end;
  return true;
}

func void bindings_clear(void) {
  profile_func_begin;
  for (sz slot_idx = 0; slot_idx < BINDINGS_MAX_COUNT; slot_idx += 1) {
    bindings_entries[slot_idx] = (binding_entry) {0};
  }
  profile_func_end;
}

func b32 bindings_add(const binding_desc* desc) {
  profile_func_begin;
  if (!desc || desc->binding_id == 0 || !bindings_desc_has_any_combo(desc)) {
    profile_func_end;
    return false;
  }
  assert(desc->binding_id != 0);

  if (desc->keyboard.count > BINDING_MAX_KEYS || desc->mouse.count > BINDING_MAX_MOUSE_BUTTONS ||
      desc->gamepad.count > BINDING_MAX_GAMEPAD_BUTTONS) {
    profile_func_end;
    return false;
  }

  sz slot_idx = bindings_find_slot_by_id(desc->binding_id);
  if (slot_idx >= BINDINGS_MAX_COUNT) {
    slot_idx = bindings_find_free_slot();
    if (slot_idx >= BINDINGS_MAX_COUNT) {
      profile_func_end;
      return false;
    }
  }

  bindings_entries[slot_idx] = (binding_entry) {
      .used = 1,
      .desc = *desc,
      .is_down = 0,
      .pressed_generation = 0,
      .released_generation = 0,
  };

  bindings_entries[slot_idx].is_down = bindings_eval_desc(&bindings_entries[slot_idx].desc);
  profile_func_end;
  return true;
}

func b32 bindings_remove(u32 binding_id) {
  profile_func_begin;
  if (binding_id == 0) {
    profile_func_end;
    return false;
  }

  sz slot_idx = bindings_find_slot_by_id(binding_id);
  if (slot_idx >= BINDINGS_MAX_COUNT) {
    profile_func_end;
    return false;
  }

  bindings_entries[slot_idx] = (binding_entry) {0};
  profile_func_end;
  return true;
}

func b32 bindings_has(u32 binding_id) {
  profile_func_begin;
  b32 res = bindings_find_slot_by_id(binding_id) < BINDINGS_MAX_COUNT;
  profile_func_end;
  return res;
}

func b32 bindings_set_enabled(u32 binding_id, b32 enabled) {
  profile_func_begin;
  sz slot_idx = bindings_find_slot_by_id(binding_id);
  if (slot_idx >= BINDINGS_MAX_COUNT) {
    profile_func_end;
    return false;
  }

  bindings_entries[slot_idx].desc.enabled = enabled ? true : false;
  bindings_refresh_all_transitions();
  profile_func_end;
  return true;
}

func b32 bindings_is_down(u32 binding_id) {
  profile_func_begin;
  sz slot_idx = bindings_find_slot_by_id(binding_id);
  if (slot_idx >= BINDINGS_MAX_COUNT) {
    profile_func_end;
    return false;
  }

  profile_func_end;
  return bindings_entries[slot_idx].is_down;
}

func b32 bindings_is_pressed(input_key key, u32 binding_id) {
  profile_func_begin;
  assert(binding_id != 0);
  sz binding_slot_idx = bindings_find_slot_by_id(binding_id);
  if (binding_slot_idx >= BINDINGS_MAX_COUNT) {
    profile_func_end;
    return false;
  }

  sz key_slot_idx = input_capture_get_slot(key);
  if (key_slot_idx >= INPUT_CAPTURE_MAX_KEYS) {
    profile_func_end;
    return false;
  }

  profile_func_end;
  return bindings_read_pressed_for_key_slot(binding_slot_idx, key_slot_idx);
}

func b32 bindings_is_released(input_key key, u32 binding_id) {
  profile_func_begin;
  assert(binding_id != 0);
  sz binding_slot_idx = bindings_find_slot_by_id(binding_id);
  if (binding_slot_idx >= BINDINGS_MAX_COUNT) {
    profile_func_end;
    return false;
  }

  sz key_slot_idx = input_capture_get_slot(key);
  if (key_slot_idx >= INPUT_CAPTURE_MAX_KEYS) {
    profile_func_end;
    return false;
  }

  profile_func_end;
  return bindings_read_released_for_key_slot(binding_slot_idx, key_slot_idx);
}

func void bindings_on_msg(const msg* src) {
  profile_func_begin;
  if (!src || !bindings_msg_affects_state(src->type)) {
    profile_func_end;
    return;
  }

  bindings_refresh_all_transitions();
  profile_func_end;
}

func void bindings_internal_on_msg(const msg* src) {
  profile_func_begin;
  bindings_on_msg(src);
  profile_func_end;
}
