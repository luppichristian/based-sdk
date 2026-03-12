// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/cmdline.h"
#include "basic/assert.h"
#include "strings/cstrings.h"
#include "basic/profiler.h"
#include "basic/safe.h"

func cmdline cmdline_build(sz count, c8** args) {
  profile_func_begin;
  if (count <= 0 || args == NULL) {
    cmdline empty = {.count = 0, .args = NULL};
    profile_func_end;
    return empty;
  }
  assert(count > 0);
  assert(args != NULL);
  cmdline cmdl = {.count = count, .args = args};
  profile_func_end;
  return cmdl;
}

func sz cmdline_get_count(cmdline cmdl) {
  return cmdl.count;
}

func b32 cmdline_is_empty(cmdline cmdl) {
  return cmdl.count == 0 ? true : false;
}

func cstr8 cmdline_get_arg(cmdline cmdl, sz idx) {
  profile_func_begin;
  if (idx >= cmdl.count || cmdl.args == NULL) {
    profile_func_end;
    return NULL;
  }
  assert(idx < cmdl.count);
  profile_func_end;
  return cmdl.args[idx];
}

func cstr8 cmdline_get_program(cmdline cmdl) {
  return cmdline_get_arg(cmdl, 0);
}

func b32 cmdline_find(cmdline cmdl, cstr8 arg, sz* out_idx) {
  profile_func_begin;
  if (arg == NULL || cmdl.args == NULL) {
    profile_func_end;
    return false;
  }
  assert(arg[0] != '\0');

  safe_for (sz idx = 0; idx < cmdl.count; idx++) {
    cstr8 value = cmdline_get_arg(cmdl, idx);
    if (value != NULL && cstr8_cmp(value, arg)) {
      if (out_idx != NULL) {
        *out_idx = idx;
      }
      profile_func_end;
      return true;
    }
  }

  profile_func_end;
  return false;
}

func b32 cmdline_has(cmdline cmdl, cstr8 arg) {
  return cmdline_find(cmdl, arg, NULL);
}

func cstr8 cmdline_get_option(cmdline cmdl, cstr8 name) {
  profile_func_begin;
  if (name == NULL || cmdl.count <= 1 || cmdl.args == NULL) {
    profile_func_end;
    return NULL;
  }

  sz name_len = cstr8_len(name);
  if (name_len == 0) {
    profile_func_end;
    return NULL;
  }

  c8 normd_name[128] = {0};
  cstr8 lookup_name = name;
  sz lookup_len = name_len;
  if (name[0] != '-') {
    if (!cstr8_cpy(normd_name, size_of(normd_name), "--")) {
      profile_func_end;
      return NULL;
    }
    if (!cstr8_cat(normd_name, size_of(normd_name), name)) {
      profile_func_end;
      return NULL;
    }
    lookup_name = normd_name;
    lookup_len = cstr8_len(lookup_name);
  }

  safe_for (sz idx = 1; idx < cmdl.count; idx++) {
    cstr8 value = cmdline_get_arg(cmdl, idx);
    if (value == NULL) {
      continue;
    }

    if (cstr8_cmp(value, lookup_name)) {
      profile_func_end;
      return cmdline_get_arg(cmdl, idx + 1);
    }

    if (cstr8_cmp_n(value, lookup_name, lookup_len) && value[lookup_len] == '=') {
      profile_func_end;
      return value + lookup_len + 1;
    }
  }

  profile_func_end;
  return NULL;
}

func b32 cmdline_get_option_i64(cmdline cmdl, cstr8 name, i64* out) {
  profile_func_begin;
  if (out == NULL) {
    profile_func_end;
    return false;
  }
  assert(out != NULL);

  cstr8 value = cmdline_get_option(cmdl, name);
  if (value == NULL) {
    profile_func_end;
    return false;
  }

  profile_func_end;
  return cstr8_to_i64(value, out);
}

func b32 cmdline_get_option_f64(cmdline cmdl, cstr8 name, f64* out) {
  profile_func_begin;
  if (out == NULL) {
    profile_func_end;
    return false;
  }
  assert(out != NULL);

  cstr8 value = cmdline_get_option(cmdl, name);
  if (value == NULL) {
    profile_func_end;
    return false;
  }

  profile_func_end;
  return cstr8_to_f64(value, out);
}
