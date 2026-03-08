// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/sensor.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

typedef struct sensor_state_entry {
  SDL_SensorID sensor_id;
  b32 is_open;
  b32 is_started;
} sensor_state_entry;

const_var sz SENSOR_STATE_CAP = 128;
global_var sensor_state_entry sensor_states[SENSOR_STATE_CAP] = {0};

func sensor_state_entry* sensor_find_state(SDL_SensorID sensor_id, b32 create_if_missing) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  for (sz item_idx = 0; item_idx < SENSOR_STATE_CAP; item_idx += 1) {
    if (sensor_states[item_idx].sensor_id == sensor_id) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return &sensor_states[item_idx];
    }
  }
  if (!create_if_missing) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  for (sz item_idx = 0; item_idx < SENSOR_STATE_CAP; item_idx += 1) {
    if (sensor_states[item_idx].sensor_id == 0) {
      sensor_states[item_idx].sensor_id = sensor_id;
      TracyCZoneEnd(__tracy_zone_ctx);
      return &sensor_states[item_idx];
    }
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return NULL;
}

func b32 sensor_id_is_valid(sensor src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return src != NULL;
}

func sensor sensor_from_native_id(up native_id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (sensor)(up)native_id;
}

func up sensor_to_native_id(sensor src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return (up)src;
}

func sz sensor_get_count(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  int count = 0;
  SDL_SensorID* ids = SDL_GetSensors(&count);

  if (ids) {
    SDL_free(ids);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return count > 0 ? (sz)count : 0;
}

func b32 sensor_get_id(sz idx, sensor* out_id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  int count = 0;
  SDL_SensorID* ids = SDL_GetSensors(&count);
  b32 found = ids != NULL && idx < (sz)count;

  if (out_id) {
    *out_id = NULL;
  }

  if (found && out_id) {
    *out_id = sensor_from_native_id((up)ids[idx]);
  }

  if (ids) {
    SDL_free(ids);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return found;
}

func cstr8 sensor_get_name(sensor id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!sensor_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_GetSensorNameForID((SDL_SensorID)sensor_to_native_id(id));
}

func sensor_kind sensor_get_kind(sensor id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!sensor_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return SENSOR_KIND_INVALID;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return (sensor_kind)SDL_GetSensorTypeForID((SDL_SensorID)sensor_to_native_id(id));
}

func i32 sensor_get_non_portable_kind(sensor id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!sensor_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return -1;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return (i32)SDL_GetSensorNonPortableTypeForID((SDL_SensorID)sensor_to_native_id(id));
}

func b32 sensor_open(sensor id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!sensor_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  sensor_state_entry* state = sensor_find_state((SDL_SensorID)sensor_to_native_id(id), 1);
  if (state == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  state->is_open = 1;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 sensor_close(sensor id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!sensor_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  sensor_state_entry* state = sensor_find_state((SDL_SensorID)sensor_to_native_id(id), 0);
  if (state == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  *state = (sensor_state_entry) {0};
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 sensor_start(sensor id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!sensor_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  sensor_state_entry* state = sensor_find_state((SDL_SensorID)sensor_to_native_id(id), 1);
  if (state == NULL || !state->is_open) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  state->is_started = 1;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 sensor_stop(sensor id) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!sensor_id_is_valid(id)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  sensor_state_entry* state = sensor_find_state((SDL_SensorID)sensor_to_native_id(id), 0);
  if (state == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  state->is_started = 0;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 sensor_read(sensor id, buffer* out_samples) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (out_samples != NULL) {
    *out_samples = (buffer) {0};
  }
  if (!sensor_id_is_valid(id) || out_samples == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  sensor_state_entry* state = sensor_find_state((SDL_SensorID)sensor_to_native_id(id), 0);
  if (state == NULL || !state->is_open || !state->is_started) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
}
