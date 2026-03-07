// MIT License
// Copyright (c) 2026 Christian Luppi

#include "input/sensor.h"
#include "../sdl3_include.h"

func b32 sensor_id_is_valid(sensor_id src) {
  return src.opaque != 0;
}

func sensor_id sensor_from_native_id(u64 native_id) {
  sensor_id result = {0};
  result.opaque = native_id;
  return result;
}

func u64 sensor_to_native_id(sensor_id src) {
  return src.opaque;
}

func sz sensor_get_count(void) {
  int count = 0;
  SDL_SensorID* ids = SDL_GetSensors(&count);

  if (ids) {
    SDL_free(ids);
  }

  return count > 0 ? (sz)count : 0;
}

func b32 sensor_get_id(sz idx, sensor_id* out_id) {
  int count = 0;
  SDL_SensorID* ids = SDL_GetSensors(&count);
  b32 found = ids != NULL && idx < (sz)count;

  if (out_id) {
    *out_id = (sensor_id) {0};
  }

  if (found && out_id) {
    *out_id = sensor_from_native_id((u64)ids[idx]);
  }

  if (ids) {
    SDL_free(ids);
  }

  return found;
}

func cstr8 sensor_get_name(sensor_id id) {
  if (!sensor_id_is_valid(id)) {
    return NULL;
  }

  return SDL_GetSensorNameForID((SDL_SensorID)sensor_to_native_id(id));
}

func sensor_kind sensor_get_kind(sensor_id id) {
  if (!sensor_id_is_valid(id)) {
    return SENSOR_KIND_INVALID;
  }

  return (sensor_kind)SDL_GetSensorTypeForID((SDL_SensorID)sensor_to_native_id(id));
}

func i32 sensor_get_non_portable_kind(sensor_id id) {
  if (!sensor_id_is_valid(id)) {
    return -1;
  }

  return (i32)SDL_GetSensorNonPortableTypeForID((SDL_SensorID)sensor_to_native_id(id));
}
