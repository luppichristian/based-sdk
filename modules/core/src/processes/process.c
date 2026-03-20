// MIT License
// Copyright (c) 2026 Christian Luppi

#include "processes/process.h"
#include "basic/assert.h"
#include "memory/memops.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "strings/cstrings.h"
#include "../platform_includes.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"
#include "basic/safe.h"

#if defined(PLATFORM_LINUX)
#  include <dirent.h>
#endif

func b32 process_snapshot_reserve(
    process_snapshot_entry** entries,
    sz required,
    sz* capacity) {
  profile_func_begin;
  if (entries == NULL || capacity == NULL) {
    profile_func_end;
    return false;
  }

  if (required <= *capacity) {
    profile_func_end;
    return true;
  }

  sz new_capacity = *capacity > 0 ? *capacity : 64;
  safe_while (new_capacity < required) {
    if (new_capacity > (SZ_MAX / 2)) {
      new_capacity = required;
      break;
    }
    new_capacity *= 2;
  }

  if (new_capacity > (SZ_MAX / size_of(process_snapshot_entry))) {
    profile_func_end;
    return false;
  }

  sz alloc_size = new_capacity * size_of(process_snapshot_entry);
  process_snapshot_entry* resized = (process_snapshot_entry*)SDL_realloc(*entries, (size_t)alloc_size);
  if (resized == NULL) {
    thread_log_error("Failed to resize process snapshot capacity=%zu", (size_t)new_capacity);
    profile_func_end;
    return false;
  }

  *entries = resized;
  *capacity = new_capacity;
  profile_func_end;
  return true;
}

func b32 process_snapshot_push(
    process_snapshot_entry** entries,
    sz* count,
    sz* capacity,
    u64 proc_id,
    cstr8 proc_name) {
  profile_func_begin;
  if (entries == NULL || count == NULL || capacity == NULL || proc_id == 0) {
    profile_func_end;
    return false;
  }

  if (!process_snapshot_reserve(entries, *count + 1, capacity)) {
    profile_func_end;
    return false;
  }

  process_snapshot_entry* entry = &(*entries)[*count];
  entry->id = proc_id;
  mem_zero(entry->name, size_of(entry->name));
  cstr8_cpy(entry->name, size_of(entry->name), (proc_name != NULL && proc_name[0] != '\0') ? proc_name : "<unknown>");

  *count += 1;
  profile_func_end;
  return true;
}

func b32 process_snapshot_parse_pid(cstr8 pid_name, u64* out_pid) {
  profile_func_begin;
  if (pid_name == NULL || out_pid == NULL || pid_name[0] == '\0') {
    profile_func_end;
    return false;
  }

  u64 parsed_pid = 0;
  if (!cstr8_to_u64(pid_name, U64_MAX, &parsed_pid) || parsed_pid == 0) {
    profile_func_end;
    return false;
  }

  *out_pid = parsed_pid;
  profile_func_end;
  return true;
}

// Returns true if options matches process_options_default().
func b32 process_options_is_default(process_options options) {
  return !options.pipe_stdin && !options.pipe_stdout && !options.pipe_stderr &&
         !options.stderr_to_stdout && !options.background && options.cwd == NULL &&
         options.envp == NULL && options.timeout_ms == 0;
}

// Returns true if options matches process_options_captured().
func b32 process_options_is_captured(process_options options) {
  return !options.pipe_stdin && options.pipe_stdout && !options.pipe_stderr &&
         options.stderr_to_stdout && !options.background && options.cwd == NULL &&
         options.envp == NULL && options.timeout_ms == 0;
}

func process_options process_options_default(void) {
  profile_func_begin;
  process_options options = {0};
  profile_func_end;
  return options;
}

func process_options process_options_captured(void) {
  profile_func_begin;
  process_options options = {0};
  options.pipe_stdout = 1;
  options.stderr_to_stdout = 1;
  profile_func_end;
  return options;
}

func process _process_create(cstr8 const* args, callsite site) {
  profile_func_begin;
  process res = _process_create_with(args, process_options_default(), site);
  profile_func_end;
  return res;
}

func process _process_create_with(
    cstr8 const* args,
    process_options options,
    callsite site) {
  profile_func_begin;

  if (!args || !args[0]) {
    thread_log_error("Rejected process create without arguments");
    profile_func_end;
    return NULL;
  }
  assert(args[0][0] != '\0');

  if (process_options_is_default(options)) {
    process prc = (process)SDL_CreateProcess(args, false);
    if (prc == NULL) {
      thread_log_warn("Default process create failed command=%s error=%s", args[0], SDL_GetError());
      thread_log_verbose("Retrying process create with explicit null stdio");

      SDL_PropertiesID fallback_props = SDL_CreateProperties();
      b32 fallback_ok = fallback_props != 0;
      if (fallback_ok) {
        fallback_ok = SDL_SetPointerProperty(fallback_props, SDL_PROP_PROCESS_CREATE_ARGS_POINTER, (void*)args);
      }
      if (fallback_ok) {
        fallback_ok = SDL_SetNumberProperty(
            fallback_props, SDL_PROP_PROCESS_CREATE_STDIN_NUMBER, SDL_PROCESS_STDIO_NULL);
      }
      if (fallback_ok) {
        fallback_ok = SDL_SetNumberProperty(
            fallback_props, SDL_PROP_PROCESS_CREATE_STDOUT_NUMBER, SDL_PROCESS_STDIO_NULL);
      }
      if (fallback_ok) {
        fallback_ok = SDL_SetNumberProperty(
            fallback_props, SDL_PROP_PROCESS_CREATE_STDERR_NUMBER, SDL_PROCESS_STDIO_NULL);
      }

      prc = fallback_ok ? (process)SDL_CreateProcessWithProperties(fallback_props) : NULL;
      if (fallback_props != 0) {
        SDL_DestroyProperties(fallback_props);
      }
    }

    if (prc == NULL) {
      thread_log_error("Failed to create process command=%s error=%s", args[0], SDL_GetError());
      profile_func_end;
      return NULL;
    }

    msg_core_object_lifecycle_data msg_data = {
        .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
        .object_type = MSG_CORE_OBJECT_TYPE_PROCESS,
        .object_ptr = prc,
        .site = site,
    };

    msg lifecycle_msg = {0};
    msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
    if (!msg_post(&lifecycle_msg)) {
      SDL_DestroyProcess((SDL_Process*)prc);
      thread_log_trace("Process creation was suspended command=%s", args[0]);
      profile_func_end;
      return NULL;
    }

    thread_log_trace("Created process command=%s options=default", args[0]);
    profile_func_end;
    return prc;
  }

  if (process_options_is_captured(options)) {
    process prc = (process)SDL_CreateProcess(args, true);
    if (prc == NULL) {
      thread_log_error("Failed to create captured process command=%s error=%s", args[0], SDL_GetError());
      profile_func_end;
      return NULL;
    }

    msg_core_object_lifecycle_data msg_data = {
        .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
        .object_type = MSG_CORE_OBJECT_TYPE_PROCESS,
        .object_ptr = prc,
        .site = site,
    };

    msg lifecycle_msg = {0};
    msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
    if (!msg_post(&lifecycle_msg)) {
      SDL_DestroyProcess((SDL_Process*)prc);
      thread_log_trace("Captured process creation was suspended command=%s", args[0]);
      profile_func_end;
      return NULL;
    }

    thread_log_trace("Created process command=%s options=captured", args[0]);
    profile_func_end;
    return prc;
  }

  SDL_PropertiesID props = SDL_CreateProperties();
  if (!props) {
    thread_log_error("Failed to create process properties command=%s error=%s", args[0], SDL_GetError());
    profile_func_end;
    return NULL;
  }

  b32 ok = SDL_SetPointerProperty(props, SDL_PROP_PROCESS_CREATE_ARGS_POINTER, (void*)args);
  if (ok && options.pipe_stdin) {
    ok = SDL_SetNumberProperty(props, SDL_PROP_PROCESS_CREATE_STDIN_NUMBER, SDL_PROCESS_STDIO_APP);
  }
  if (ok && options.pipe_stdout) {
    ok = SDL_SetNumberProperty(props, SDL_PROP_PROCESS_CREATE_STDOUT_NUMBER, SDL_PROCESS_STDIO_APP);
  }
  if (ok && options.pipe_stderr) {
    ok = SDL_SetNumberProperty(props, SDL_PROP_PROCESS_CREATE_STDERR_NUMBER, SDL_PROCESS_STDIO_APP);
  }
  if (ok && options.stderr_to_stdout) {
    ok = SDL_SetBooleanProperty(props, SDL_PROP_PROCESS_CREATE_STDERR_TO_STDOUT_BOOLEAN, true);
  }
  if (ok && options.background) {
    ok = SDL_SetBooleanProperty(props, SDL_PROP_PROCESS_CREATE_BACKGROUND_BOOLEAN, true);
  }

  process prc = ok ? (process)SDL_CreateProcessWithProperties(props) : NULL;
  SDL_DestroyProperties(props);
  if (!ok) {
    thread_log_error("Failed to configure process properties command=%s", args[0]);
  }
  if (prc == NULL) {
    thread_log_error("Failed to create configured process command=%s error=%s", args[0], SDL_GetError());
  } else {
    msg_core_object_lifecycle_data msg_data = {
        .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
        .object_type = MSG_CORE_OBJECT_TYPE_PROCESS,
        .object_ptr = prc,
        .site = site,
    };

    msg lifecycle_msg = {0};
    msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
    if (!msg_post(&lifecycle_msg)) {
      SDL_DestroyProcess((SDL_Process*)prc);
      thread_log_trace("Process creation was suspended command=%s", args[0]);
      profile_func_end;
      return NULL;
    }

    thread_log_trace("Created configured process command=%s", args[0]);
  }
  profile_func_end;
  return prc;
}

func b32 process_is_valid(process prc) {
  return prc != NULL;
}

func u64 process_get_id(process prc) {
  if (!prc) {
    return 0;
  }

  SDL_PropertiesID props = SDL_GetProcessProperties((SDL_Process*)prc);
  if (!props) {
    return 0;
  }

  return (u64)SDL_GetNumberProperty(props, SDL_PROP_PROCESS_PID_NUMBER, 0);
}

func process_snapshot_entry* process_snapshot_get(sz* out_count) {
  profile_func_begin;

  process_snapshot_entry* entries = NULL;
  sz count = 0;
  sz capacity = 0;

#if defined(PLATFORM_WINDOWS)
  DWORD proc_ids[4096] = {0};
  DWORD bytes_needed = 0;
  if (!EnumProcesses(proc_ids, size_of(proc_ids), &bytes_needed)) {
    thread_log_error("Failed to enumerate process ids error=%lu", (unsigned long)GetLastError());
    if (out_count != NULL) {
      *out_count = 0;
    }
    profile_func_end;
    return NULL;
  }

  sz pid_count = (sz)(bytes_needed / size_of(DWORD));
  safe_for (sz pid_idx = 0; pid_idx < pid_count; ++pid_idx) {
    u64 proc_id = (u64)proc_ids[pid_idx];
    if (proc_id == 0) {
      continue;
    }

    c8 proc_name[PROCESS_NAME_CAP] = {0};
    HANDLE proc_handle = OpenProcess(
        PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ,
        FALSE,
        (DWORD)proc_id);
    if (proc_handle != NULL) {
      DWORD name_size = (DWORD)size_of(proc_name);
      if (!QueryFullProcessImageNameA(proc_handle, 0, proc_name, &name_size) || proc_name[0] == '\0') {
        if (GetModuleBaseNameA(proc_handle, NULL, proc_name, (DWORD)size_of(proc_name)) == 0) {
          cstr8_cpy(proc_name, size_of(proc_name), "<unknown>");
        }
      }
      CloseHandle(proc_handle);
    }
    if (proc_name[0] == '\0') {
      cstr8_cpy(proc_name, size_of(proc_name), "<unknown>");
    } else {
      cstr8 last_backslash = cstr8_find_last_char(proc_name, '\\');
      cstr8 last_forward_slash = cstr8_find_last_char(proc_name, '/');
      cstr8 base_name = proc_name;
      if (last_backslash != NULL) {
        base_name = last_backslash + 1;
      }
      if (last_forward_slash != NULL && last_forward_slash + 1 > base_name) {
        base_name = last_forward_slash + 1;
      }
      if (base_name != proc_name) {
        c8 normalized_name[PROCESS_NAME_CAP] = {0};
        cstr8_cpy(normalized_name, size_of(normalized_name), base_name);
        cstr8_cpy(proc_name, size_of(proc_name), normalized_name);
      }
    }

    if (!process_snapshot_push(&entries, &count, &capacity, proc_id, proc_name)) {
      thread_log_error("Failed to append process snapshot entry on Windows");
      SDL_free(entries);
      if (out_count != NULL) {
        *out_count = 0;
      }
      profile_func_end;
      return NULL;
    }
  }
#elif defined(PLATFORM_LINUX)
  DIR* proc_dir = opendir("/proc");
  if (proc_dir == NULL) {
    thread_log_error("Failed to open proc filesystem path=/proc");
    if (out_count != NULL) {
      *out_count = 0;
    }
    profile_func_end;
    return NULL;
  }

  struct dirent* dir_entry = NULL;
  safe_while ((dir_entry = readdir(proc_dir)) != NULL) {
    u64 proc_id = 0;
    if (!process_snapshot_parse_pid((cstr8)dir_entry->d_name, &proc_id)) {
      continue;
    }

    c8 comm_path[64] = {0};
    if (!cstr8_format(comm_path, size_of(comm_path), "/proc/%s/comm", dir_entry->d_name)) {
      continue;
    }

    i32 comm_fd = open(comm_path, O_RDONLY);
    if (comm_fd < 0) {
      continue;
    }

    c8 proc_name[PROCESS_NAME_CAP] = {0};
    ssize_t read_size = read(comm_fd, proc_name, size_of(proc_name) - 1);
    close(comm_fd);
    if (read_size <= 0) {
      continue;
    }

    safe_for (sz char_idx = 0; char_idx < (sz)read_size; ++char_idx) {
      if (proc_name[char_idx] == '\n' || proc_name[char_idx] == '\r') {
        proc_name[char_idx] = '\0';
        break;
      }
    }

    if (!process_snapshot_push(&entries, &count, &capacity, proc_id, proc_name)) {
      thread_log_error("Failed to append process snapshot entry on Linux");
      SDL_free(entries);
      closedir(proc_dir);
      if (out_count != NULL) {
        *out_count = 0;
      }
      profile_func_end;
      return NULL;
    }
  }

  closedir(proc_dir);
#elif defined(PLATFORM_MACOS)
  i32 mib[] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};
  size_t proc_bytes = 0;
  if (sysctl(mib, count_of(mib), NULL, &proc_bytes, NULL, 0) != 0 || proc_bytes == 0) {
    thread_log_error("Failed to query process table on macOS");
    if (out_count != NULL) {
      *out_count = 0;
    }
    profile_func_end;
    return NULL;
  }

  struct kinfo_proc* proc_list = (struct kinfo_proc*)SDL_malloc(proc_bytes);
  if (proc_list == NULL) {
    thread_log_error("Failed to allocate process list bytes=%zu", proc_bytes);
    if (out_count != NULL) {
      *out_count = 0;
    }
    profile_func_end;
    return NULL;
  }

  if (sysctl(mib, count_of(mib), proc_list, &proc_bytes, NULL, 0) != 0) {
    thread_log_error("Failed to read process table on macOS");
    SDL_free(proc_list);
    if (out_count != NULL) {
      *out_count = 0;
    }
    profile_func_end;
    return NULL;
  }

  sz proc_count = (sz)(proc_bytes / size_of(struct kinfo_proc));
  safe_for (sz idx = 0; idx < proc_count; ++idx) {
    u64 proc_id = (u64)proc_list[idx].kp_proc.p_pid;
    if (proc_id == 0) {
      continue;
    }

    if (!process_snapshot_push(&entries, &count, &capacity, proc_id, proc_list[idx].kp_proc.p_comm)) {
      thread_log_error("Failed to append process snapshot entry on macOS");
      SDL_free(proc_list);
      SDL_free(entries);
      if (out_count != NULL) {
        *out_count = 0;
      }
      profile_func_end;
      return NULL;
    }
  }

  SDL_free(proc_list);
#else
  invalid_code_path;
  if (out_count != NULL) {
    *out_count = 0;
  }
  profile_func_end;
  return NULL;
#endif

  if (out_count != NULL) {
    *out_count = count;
  }
  thread_log_trace("Enumerated process snapshot count=%zu", (size_t)count);
  profile_func_end;
  return entries;
}

func void process_snapshot_free(process_snapshot_entry* ptr) {
  profile_func_begin;
  SDL_free(ptr);
  profile_func_end;
}

func void* process_read(process prc, sz* out_size, i32* out_exit_code) {
  profile_func_begin;
  if (!prc) {
    thread_log_error("Cannot read invalid process handle");
    profile_func_end;
    return NULL;
  }
  assert(process_is_valid(prc));

  sz read_size = 0;
  i32 exit_code = 0;
  void* data = SDL_ReadProcess(
      (SDL_Process*)prc,
      out_size ? (size_t*)&read_size : NULL,
      out_exit_code ? (int*)&exit_code : NULL);

  if (out_size) {
    *out_size = (sz)read_size;
  }
  if (out_exit_code) {
    *out_exit_code = (i32)exit_code;
  }

  if (data == NULL) {
    thread_log_warn("Process read returned no data handle=%p", prc);
  } else {
    thread_log_trace("Read process output handle=%p size=%zu", prc, (size_t)read_size);
  }

  profile_func_end;
  return data;
}

func void process_read_free(void* ptr) {
  profile_func_begin;
  SDL_free(ptr);
  profile_func_end;
}

func b32 process_wait(process prc, b32 block, i32* out_exit_code) {
  profile_func_begin;
  if (!prc) {
    thread_log_error("Cannot wait on invalid process");
    profile_func_end;
    return false;
  }
  assert(block == 0 || block == 1);

  i32 exit_code = 0;
  b32 finished = SDL_WaitProcess(
      (SDL_Process*)prc,
      block != 0,
      out_exit_code ? (int*)&exit_code : NULL);
  if (finished && out_exit_code) {
    *out_exit_code = (i32)exit_code;
  }
  thread_log_trace("Waited on process handle=%p finished=%u block=%u", prc, (u32)finished, (u32)block);

  profile_func_end;
  return finished;
}

func b32 process_wait_timeout(process prc, i32 timeout_ms, i32* out_exit_code) {
  profile_func_begin;
  if (!prc || timeout_ms < 0) {
    thread_log_error("Rejected process wait timeout handle=%p timeout_ms=%d", prc, timeout_ms);
    profile_func_end;
    return false;
  }

  u64 start_ticks = SDL_GetTicks();
  i32 exit_code = 0;
  safe_while (!process_wait(prc, 0, &exit_code)) {
    if ((i32)(SDL_GetTicks() - start_ticks) >= timeout_ms) {
      thread_log_warn("Process wait timed out handle=%p timeout_ms=%d", prc, timeout_ms);
      profile_func_end;
      return false;
    }
    SDL_Delay(1);
  }

  if (out_exit_code != NULL) {
    *out_exit_code = exit_code;
  }
  thread_log_trace("Process wait completed within timeout handle=%p timeout_ms=%d", prc, timeout_ms);
  profile_func_end;
  return true;
}

func b32 process_kill(process prc, b32 force) {
  profile_func_begin;
  if (!prc) {
    thread_log_error("Cannot kill invalid process");
    profile_func_end;
    return false;
  }
  assert(force == 0 || force == 1);

  b32 success = SDL_KillProcess((SDL_Process*)prc, force != 0);
  if (!success) {
    thread_log_error("Failed to kill process handle=%p force=%u error=%s", prc, (u32)force, SDL_GetError());
  } else {
    thread_log_info("Killed process handle=%p force=%u", prc, (u32)force);
  }
  profile_func_end;
  return success;
}

func void _process_destroy(process prc, callsite site) {
  profile_func_begin;
  if (!prc) {
    thread_log_warn("Skipping process destroy for invalid handle");
    profile_func_end;
    return;
  }

  msg_core_object_lifecycle_data msg_data = {
      .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
      .object_type = MSG_CORE_OBJECT_TYPE_PROCESS,
      .object_ptr = prc,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("Process destruction was suspended handle=%p", prc);
    profile_func_end;
    return;
  }

  thread_log_trace("Destroyed process handle=%p", prc);
  SDL_DestroyProcess((SDL_Process*)prc);
  profile_func_end;
}
