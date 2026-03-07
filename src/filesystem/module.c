// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/module.h"

#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "basic/env_defines.h"
#include "basic/profiler.h"

#include <string.h>

#if defined(PLATFORM_WINDOWS)
#  include <windows.h>
#elif defined(PLATFORM_UNIX)
#  include <dlfcn.h>
#endif

#define MODULE_INIT_SYMBOL "mod_init"
#define MODULE_QUIT_SYMBOL "mod_quit"

func mod module_empty(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  mod module_value;
  memset(&module_value, 0, size_of(module_value));
  module_value.source_path = path_from_cstr("");
  TracyCZoneEnd(__tracy_zone_ctx);
  return module_value;
}

func b32 mod_is_open(const mod* mod_ptr) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (mod_ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return mod_ptr->native_handle != NULL ? 1 : 0;
}

func void* mod_get_func(const mod* mod_ptr, cstr8 name) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!mod_is_open(mod_ptr) || name == NULL || name[0] == '\0') {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  assert(name[0] != '\0');

#if defined(PLATFORM_WINDOWS)
  FARPROC raw_symbol = GetProcAddress((HMODULE)mod_ptr->native_handle, name);
  union {
    FARPROC raw_symbol;
    void* resolved_func;
  } cast_value;
  cast_value.raw_symbol = raw_symbol;
  TracyCZoneEnd(__tracy_zone_ctx);
  return cast_value.resolved_func;
#elif defined(PLATFORM_UNIX)
  void* raw_symbol = dlsym(mod_ptr->native_handle, name);
  union {
    void* raw_symbol;
    void* resolved_func;
  } cast_value;
  cast_value.raw_symbol = raw_symbol;
  TracyCZoneEnd(__tracy_zone_ctx);
  return cast_value.resolved_func;
#else
  (void)name;
  TracyCZoneEnd(__tracy_zone_ctx);
  return NULL;
#endif
  TracyCZoneEnd(__tracy_zone_ctx);
}

func cstr8 mod_get_extension(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
#if defined(PLATFORM_WINDOWS)
  TracyCZoneEnd(__tracy_zone_ctx);
  return ".dll";
#elif defined(PLATFORM_MACOS)
  TracyCZoneEnd(__tracy_zone_ctx);
  return ".dylib";
#elif defined(PLATFORM_UNIX)
  TracyCZoneEnd(__tracy_zone_ctx);
  return ".so";
#else
  TracyCZoneEnd(__tracy_zone_ctx);
  return "";
#endif
}

func void mod_close(mod* mod_ptr) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!mod_is_open(mod_ptr)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(mod_ptr != NULL);

  if (mod_ptr->initialized && mod_ptr->quit_func != NULL) {
    mod_ptr->quit_func();
    mod_ptr->initialized = 0;
  }

#if defined(PLATFORM_WINDOWS)
  (void)FreeLibrary((HMODULE)mod_ptr->native_handle);
#elif defined(PLATFORM_UNIX)
  (void)dlclose(mod_ptr->native_handle);
#endif

  *mod_ptr = module_empty();
  thread_log_trace("mod_close");
  TracyCZoneEnd(__tracy_zone_ctx);
}

func mod mod_open(const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  mod module_value = module_empty();
  void* init_symbol = NULL;
  void* quit_symbol = NULL;

  if (src == NULL || src->buf[0] == '\0') {
    TracyCZoneEnd(__tracy_zone_ctx);
    return module_value;
  }
  assert(src->buf[0] != '\0');

#if defined(PLATFORM_WINDOWS)
  HMODULE handle = LoadLibraryA(src->buf);
  if (handle == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return module_value;
  }
  module_value.native_handle = (void*)handle;
#elif defined(PLATFORM_UNIX)
  void* handle = dlopen(src->buf, RTLD_NOW);
  if (handle == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return module_value;
  }
  module_value.native_handle = handle;
#else
  TracyCZoneEnd(__tracy_zone_ctx);
  return module_value;
#endif

  module_value.source_path = *src;

  init_symbol = mod_get_func(&module_value, MODULE_INIT_SYMBOL);
  quit_symbol = mod_get_func(&module_value, MODULE_QUIT_SYMBOL);
  union {
    void* generic_func;
    mod_init_func* init_func;
    mod_quit_func* quit_func;
  } cast_value;
  cast_value.generic_func = init_symbol;
  module_value.init_func = cast_value.init_func;
  cast_value.generic_func = quit_symbol;
  module_value.quit_func = cast_value.quit_func;

  if (module_value.init_func == NULL || module_value.quit_func == NULL) {
    thread_log_error("mod_open: missing mod_init/mod_quit symbols path=%s", src->buf);
    mod_close(&module_value);
    TracyCZoneEnd(__tracy_zone_ctx);
    return module_empty();
  }

  if (!module_value.init_func()) {
    thread_log_error("mod_open: mod_init failed path=%s", src->buf);
    mod_close(&module_value);
    TracyCZoneEnd(__tracy_zone_ctx);
    return module_empty();
  }

  module_value.initialized = 1;
  thread_log_trace("mod_open: path=%s", src->buf);
  TracyCZoneEnd(__tracy_zone_ctx);
  return module_value;
}

