// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/sensor.h"
#include "context/thread_ctx.h"
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
  profile_func_begin;
  for (sz item_idx = 0; item_idx < SENSOR_STATE_CAP; item_idx += 1) {
    if (sensor_states[item_idx].sensor_id == sensor_id) {
      profile_func_end;
      return &sensor_states[item_idx];
    }
  }
  if (!create_if_missing) {
    profile_func_end;
    return NULL;
  }
  for (sz item_idx = 0; item_idx < SENSOR_STATE_CAP; item_idx += 1) {
    if (sensor_states[item_idx].sensor_id == 0) {
      sensor_states[item_idx].sensor_id = sensor_id;
      profile_func_end;
      return &sensor_states[item_idx];
    }
  }
  if (create_if_missing) {
    thread_log_error("Sensor state table is full sensor_id=%lld", (long long)sensor_id);
  }
  profile_func_end;
  return NULL;
}

func b32 sensor_id_is_valid(sensor src) {
  return src != NULL;
}

func sensor sensor_from_native_id(up native_id) {
  return (sensor)(up)native_id;
}

func up sensor_to_native_id(sensor src) {
  return (up)src;
}

func sz sensor_get_count(void) {
  profile_func_begin;
  int count = 0;
  SDL_SensorID* ids = SDL_GetSensors(&count);

  if (ids == NULL && count != 0) {
    thread_log_warn("Failed to enumerate sensors error=%s", SDL_GetError());
  }

  if (ids) {
    SDL_free(ids);
  }

  sz result = count > 0 ? (sz)count : 0;
  profile_func_end;
  return result;
}

func b32 sensor_get_id(sz idx, sensor* out_id) {
  profile_func_begin;
  int count = 0;
  SDL_SensorID* ids = SDL_GetSensors(&count);
  b32 found = ids != NULL && idx < (sz)count;

  if (out_id) {
    *out_id = NULL;
  }

  if (ids == NULL && count != 0) {
    thread_log_error("Failed to enumerate sensors for id lookup idx=%zu error=%s", (size_t)idx, SDL_GetError());
  } else if (!found) {
    thread_log_warn("Sensor id lookup missed idx=%zu count=%d", (size_t)idx, count);
  }

  if (found && out_id) {
    *out_id = sensor_from_native_id((up)ids[idx]);
  }

  if (ids) {
    SDL_free(ids);
  }

  profile_func_end;
  return found;
}

func cstr8 sensor_get_name(sensor id) {
  profile_func_begin;
  if (!sensor_id_is_valid(id)) {
    thread_log_warn("Rejected sensor name query for invalid sensor");
    profile_func_end;
    return NULL;
  }

  cstr8 result = SDL_GetSensorNameForID((SDL_SensorID)sensor_to_native_id(id));
  if (result == NULL) {
    thread_log_warn("Sensor name is unavailable sensor=%lld", (long long)sensor_to_native_id(id));
  }
  profile_func_end;
  return result;
}

func sensor_kind sensor_get_kind(sensor id) {
  profile_func_begin;
  if (!sensor_id_is_valid(id)) {
    thread_log_warn("Rejected sensor kind query for invalid sensor");
    profile_func_end;
    return SENSOR_KIND_INVALID;
  }

  sensor_kind result = (sensor_kind)SDL_GetSensorTypeForID((SDL_SensorID)sensor_to_native_id(id));
  profile_func_end;
  return result;
}

func i32 sensor_get_non_portable_kind(sensor id) {
  profile_func_begin;
  if (!sensor_id_is_valid(id)) {
    thread_log_warn("Rejected sensor non portable kind query for invalid sensor");
    profile_func_end;
    return -1;
  }

  profile_func_end;
  return (i32)SDL_GetSensorNonPortableTypeForID((SDL_SensorID)sensor_to_native_id(id));
}

func b32 sensor_open(sensor id) {
  profile_func_begin;
  if (!sensor_id_is_valid(id)) {
    thread_log_error("Rejected sensor open for invalid sensor");
    profile_func_end;
    return false;
  }
  sensor_state_entry* state = sensor_find_state((SDL_SensorID)sensor_to_native_id(id), 1);
  if (state == NULL) {
    thread_log_error("Failed to create sensor state sensor=%lld", (long long)sensor_to_native_id(id));
    profile_func_end;
    return false;
  }
  state->is_open = 1;
  thread_log_info("Opened sensor=%lld", (long long)sensor_to_native_id(id));
  profile_func_end;
  return true;
}

func b32 sensor_close(sensor id) {
  profile_func_begin;
  if (!sensor_id_is_valid(id)) {
    thread_log_error("Rejected sensor close for invalid sensor");
    profile_func_end;
    return false;
  }
  sensor_state_entry* state = sensor_find_state((SDL_SensorID)sensor_to_native_id(id), 0);
  if (state == NULL) {
    thread_log_warn("Cannot close unknown sensor=%lld", (long long)sensor_to_native_id(id));
    profile_func_end;
    return false;
  }
  *state = (sensor_state_entry) {0};
  thread_log_info("Closed sensor=%lld", (long long)sensor_to_native_id(id));
  profile_func_end;
  return true;
}

func b32 sensor_start(sensor id) {
  profile_func_begin;
  if (!sensor_id_is_valid(id)) {
    thread_log_error("Rejected sensor start for invalid sensor");
    profile_func_end;
    return false;
  }
  sensor_state_entry* state = sensor_find_state((SDL_SensorID)sensor_to_native_id(id), 1);
  if (state == NULL || !state->is_open) {
    thread_log_error("Cannot start sensor=%lld because it is not open", (long long)sensor_to_native_id(id));
    profile_func_end;
    return false;
  }
  state->is_started = 1;
  thread_log_info("Started sensor=%lld", (long long)sensor_to_native_id(id));
  profile_func_end;
  return true;
}

func b32 sensor_stop(sensor id) {
  profile_func_begin;
  if (!sensor_id_is_valid(id)) {
    thread_log_error("Rejected sensor stop for invalid sensor");
    profile_func_end;
    return false;
  }
  sensor_state_entry* state = sensor_find_state((SDL_SensorID)sensor_to_native_id(id), 0);
  if (state == NULL) {
    thread_log_warn("Cannot stop unknown sensor=%lld", (long long)sensor_to_native_id(id));
    profile_func_end;
    return false;
  }
  state->is_started = 0;
  thread_log_info("Stopped sensor=%lld", (long long)sensor_to_native_id(id));
  profile_func_end;
  return true;
}

func b32 sensor_read(sensor id, buffer* out_samples) {
  profile_func_begin;
  if (out_samples != NULL) {
    *out_samples = (buffer) {0};
  }
  if (!sensor_id_is_valid(id) || out_samples == NULL) {
    thread_log_error("Rejected sensor read sensor=%lld out_samples=%p",
                     (long long)sensor_to_native_id(id),
                     (void*)out_samples);
    profile_func_end;
    return false;
  }
  sensor_state_entry* state = sensor_find_state((SDL_SensorID)sensor_to_native_id(id), 0);
  if (state == NULL || !state->is_open || !state->is_started) {
    thread_log_error("Cannot read sensor=%lld open=%u started=%u",
                     (long long)sensor_to_native_id(id),
                     (u32)(state != NULL && state->is_open),
                     (u32)(state != NULL && state->is_started));
    profile_func_end;
    return false;
  }
  thread_log_warn("Sensor read is not implemented sensor=%lld", (long long)sensor_to_native_id(id));
  profile_func_end;
  return false;
}
