// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/cmdline.h"
#include "basic/assert.h"
#include "strings/cstrings.h"
#include "basic/profiler.h"

func cmdline cmdline_build(sz count, c8** args) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (count <= 0 || args == NULL) {
    cmdline empty = {.count = 0, .args = NULL};
    TracyCZoneEnd(__tracy_zone_ctx);
    return empty;
  }
  assert(count > 0);
  assert(args != NULL);

  cmdline cmdl = {.count = (sz)count, .args = args};
  TracyCZoneEnd(__tracy_zone_ctx);
  return cmdl;
}

func sz cmdline_get_count(cmdline cmdl) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return cmdl.count;
}

func b32 cmdline_is_empty(cmdline cmdl) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return cmdl.count == 0 ? 1 : 0;
}

func cstr8 cmdline_get_arg(cmdline cmdl, sz idx) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (idx >= cmdl.count || cmdl.args == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  assert(idx < cmdl.count);
  TracyCZoneEnd(__tracy_zone_ctx);
  return cmdl.args[idx];
}

func cstr8 cmdline_get_program(cmdline cmdl) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return cmdline_get_arg(cmdl, 0);
}

func b32 cmdline_find(cmdline cmdl, cstr8 arg, sz* out_idx) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (arg == NULL || cmdl.args == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(arg[0] != '\0');

  for (sz idx = 0; idx < cmdl.count; idx++) {
    cstr8 value = cmdline_get_arg(cmdl, idx);
    if (value != NULL && cstr8_cmp(value, arg) == 0) {
      if (out_idx != NULL) {
        *out_idx = idx;
      }
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;
    }
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
}

func b32 cmdline_has(cmdline cmdl, cstr8 arg) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return cmdline_find(cmdl, arg, NULL);
}

func cstr8 cmdline_get_option(cmdline cmdl, cstr8 name) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (name == NULL || cmdl.count <= 1 || cmdl.args == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }

  sz name_len = cstr8_len(name);
  if (name_len == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }

  for (sz idx = 1; idx < cmdl.count; idx++) {
    cstr8 value = cmdline_get_arg(cmdl, idx);
    if (value == NULL) {
      continue;
    }

    if (cstr8_cmp(value, name) == 0) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return cmdline_get_arg(cmdl, idx + 1);
    }

    if (cstr8_cmp_n(value, name, name_len) == 0 && value[name_len] == '=') {
      TracyCZoneEnd(__tracy_zone_ctx);
      return value + name_len + 1;
    }
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return NULL;
}

func b32 cmdline_get_option_i64(cmdline cmdl, cstr8 name, i64* out) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (out == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(out != NULL);

  cstr8 value = cmdline_get_option(cmdl, name);
  if (value == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return cstr8_to_i64(value, out);
}

func b32 cmdline_get_option_f64(cmdline cmdl, cstr8 name, f64* out) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (out == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(out != NULL);

  cstr8 value = cmdline_get_option(cmdl, name);
  if (value == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return cstr8_to_f64(value, out);
}
