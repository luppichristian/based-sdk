// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/cmdline.h"
#include "strings/cstrings.h"

func cmdline cmdline_make(sz count, c8** args) {
  if (count <= 0 || args == NULL) {
    cmdline empty = {.count = 0, .args = NULL};
    return empty;
  }

  cmdline cmdl = {.count = (sz)count, .args = args};
  return cmdl;
}

func sz cmdline_count(cmdline cmdl) {
  return cmdl.count;
}

func b32 cmdline_is_empty(cmdline cmdl) {
  return cmdl.count == 0 ? 1 : 0;
}

func const c8* cmdline_arg(cmdline cmdl, sz index) {
  if (index >= cmdl.count || cmdl.args == NULL) {
    return NULL;
  }
  return cmdl.args[index];
}

func const c8* cmdline_program(cmdline cmdl) {
  return cmdline_arg(cmdl, 0);
}

func b32 cmdline_find(cmdline cmdl, const c8* arg, sz* out_index) {
  if (arg == NULL || cmdl.args == NULL) {
    return 0;
  }

  for (sz index = 0; index < cmdl.count; index++) {
    const c8* value = cmdline_arg(cmdl, index);
    if (value != NULL && cstr8_cmp(value, arg) == 0) {
      if (out_index != NULL) {
        *out_index = index;
      }
      return 1;
    }
  }

  return 0;
}

func b32 cmdline_has(cmdline cmdl, const c8* arg) {
  return cmdline_find(cmdl, arg, NULL);
}

func const c8* cmdline_get_option(cmdline cmdl, const c8* name) {
  if (name == NULL || cmdl.count <= 1 || cmdl.args == NULL) {
    return NULL;
  }

  sz name_len = cstr8_len(name);
  if (name_len == 0) {
    return NULL;
  }

  for (sz index = 1; index < cmdl.count; index++) {
    const c8* value = cmdline_arg(cmdl, index);
    if (value == NULL) {
      continue;
    }

    if (cstr8_cmp(value, name) == 0) {
      return cmdline_arg(cmdl, index + 1);
    }

    if (cstr8_cmp_n(value, name, name_len) == 0 && value[name_len] == '=') {
      return value + name_len + 1;
    }
  }

  return NULL;
}

func b32 cmdline_get_option_i64(cmdline cmdl, const c8* name, i64* out) {
  if (out == NULL) {
    return 0;
  }

  const c8* value = cmdline_get_option(cmdl, name);
  if (value == NULL) {
    return 0;
  }

  return cstr8_to_i64(value, out);
}

func b32 cmdline_get_option_f64(cmdline cmdl, const c8* name, f64* out) {
  if (out == NULL) {
    return 0;
  }

  const c8* value = cmdline_get_option(cmdl, name);
  if (value == NULL) {
    return 0;
  }

  return cstr8_to_f64(value, out);
}
