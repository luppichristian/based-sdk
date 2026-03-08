// MIT License
// Copyright (c) 2026 Christian Luppi

#include "processes/common_processes.h"
#include "basic/env_defines.h"
#include "basic/profiler.h"
#include "basic/utility_defines.h"
#include "processes/process.h"
#include "../sdl3_include.h"

#include <stdio.h>

func b32 common_processes_spawn_background(cstr8 const* args, cstr8 cwd_path) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (args == NULL || args[0] == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return false;
  }

  process_options options = process_options_default();
  options.background = true;
  options.cwd = cwd_path;
  process prc = process_create_with(args, options);
  if (!process_is_valid(prc)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return false;
  }

  process_destroy(prc);
  TracyCZoneEnd(__tracy_zone_ctx);
  return true;
}

func b32 process_open_weblink(cstr8 url) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (url == NULL || url[0] == '\0') {
    TracyCZoneEnd(__tracy_zone_ctx);
    return false;
  }

  b32 result = SDL_OpenURL(url);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func b32 process_open_file_window(cstr8 location) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
#if defined(PLATFORM_WINDOWS)
  cstr8 const args[] = {
      "explorer.exe",
      location != NULL ? location : ".",
      NULL,
  };
#elif defined(PLATFORM_MACOS)
  cstr8 const args[] = {
      "open",
      location != NULL ? location : ".",
      NULL,
  };
#else
  cstr8 const args[] = {
      "xdg-open",
      location != NULL ? location : ".",
      NULL,
  };
#endif
  b32 result = common_processes_spawn_background(args, NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func b32 process_open_terminal(cstr8 location) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
#if defined(PLATFORM_WINDOWS)
  c8 cmd_text[1024] = {0};
  cstr8 dir_path = location != NULL ? location : ".";
  (void)snprintf(cmd_text, size_of(cmd_text), "cd /d \"%s\"", dir_path);

  cstr8 const args[] = {
      "cmd.exe",
      "/K",
      cmd_text,
      NULL,
  };
  b32 result = common_processes_spawn_background(args, NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
#elif defined(PLATFORM_MACOS)
  cstr8 const args[] = {
      "open",
      "-a",
      "Terminal",
      location != NULL ? location : ".",
      NULL,
  };
  b32 result = common_processes_spawn_background(args, NULL);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
#else
  cstr8 const xterm_args[] = {
      "x-terminal-emulator",
      NULL,
  };
  if (common_processes_spawn_background(xterm_args, location)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return true;
  }

  cstr8 const gnome_args[] = {
      "gnome-terminal",
      "--working-directory",
      location != NULL ? location : ".",
      NULL,
  };
  if (common_processes_spawn_background(gnome_args, NULL)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return true;
  }

  cstr8 const konsole_args[] = {
      "konsole",
      "--workdir",
      location != NULL ? location : ".",
      NULL,
  };
  if (common_processes_spawn_background(konsole_args, NULL)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return true;
  }

  cstr8 const xterm_plain_args[] = {
      "xterm",
      NULL,
  };
  b32 result = common_processes_spawn_background(xterm_plain_args, location);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
#endif
}
