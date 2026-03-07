// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/cmdline.h"
#include "basic/assert.h"
#include "strings/cstrings.h"

func cmdline cmdline_build(sz count, c8** args) {
  if (count <= 0 || args == NULL) {
    cmdline empty = {.count = 0, .args = NULL};
    return empty;
  }
  assert(count > 0);
  assert(args != NULL);

  cmdline cmdl = {.count = (sz)count, .args = args};
  return cmdl;
}

func sz cmdline_get_count(cmdline cmdl) {
  return cmdl.count;
}

func b32 cmdline_is_empty(cmdline cmdl) {
  return cmdl.count == 0 ? 1 : 0;
}

func cstr8 cmdline_get_arg(cmdline cmdl, sz idx) {
  if (idx >= cmdl.count || cmdl.args == NULL) {
    return NULL;
  }
  assert(idx < cmdl.count);
  return cmdl.args[idx];
}

func cstr8 cmdline_get_program(cmdline cmdl) {
  return cmdline_get_arg(cmdl, 0);
}

func b32 cmdline_find(cmdline cmdl, cstr8 arg, sz* out_idx) {
  if (arg == NULL || cmdl.args == NULL) {
    return 0;
  }
  assert(arg[0] != '\0');

  for (sz idx = 0; idx < cmdl.count; idx++) {
    cstr8 value = cmdline_get_arg(cmdl, idx);
    if (value != NULL && cstr8_cmp(value, arg) == 0) {
      if (out_idx != NULL) {
        *out_idx = idx;
      }
      return 1;
    }
  }

  return 0;
}

func b32 cmdline_has(cmdline cmdl, cstr8 arg) {
  return cmdline_find(cmdl, arg, NULL);
}

func cstr8 cmdline_get_option(cmdline cmdl, cstr8 name) {
  if (name == NULL || cmdl.count <= 1 || cmdl.args == NULL) {
    return NULL;
  }

  sz name_len = cstr8_len(name);
  if (name_len == 0) {
    return NULL;
  }

  for (sz idx = 1; idx < cmdl.count; idx++) {
    cstr8 value = cmdline_get_arg(cmdl, idx);
    if (value == NULL) {
      continue;
    }

    if (cstr8_cmp(value, name) == 0) {
      return cmdline_get_arg(cmdl, idx + 1);
    }

    if (cstr8_cmp_n(value, name, name_len) == 0 && value[name_len] == '=') {
      return value + name_len + 1;
    }
  }

  return NULL;
}

func b32 cmdline_get_option_i64(cmdline cmdl, cstr8 name, i64* out) {
  if (out == NULL) {
    return 0;
  }
  assert(out != NULL);

  cstr8 value = cmdline_get_option(cmdl, name);
  if (value == NULL) {
    return 0;
  }

  return cstr8_to_i64(value, out);
}

func b32 cmdline_get_option_f64(cmdline cmdl, cstr8 name, f64* out) {
  if (out == NULL) {
    return 0;
  }
  assert(out != NULL);

  cstr8 value = cmdline_get_option(cmdl, name);
  if (value == NULL) {
    return 0;
  }

  return cstr8_to_f64(value, out);
}
