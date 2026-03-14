// MIT License
// Copyright (c) 2026 Christian Luppi

#include "system/system_info.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "basic/env_defines.h"
#include "basic/profiler.h"
#include "memory/memops.h"
#include "platform_includes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(PLATFORM_WINDOWS)
typedef LONG(WINAPI* rtl_get_version_fn)(void* version_info);
#endif

func cstr8 system_architecture_name(void) {
#if defined(ARCH_X86_64)
  return "x86_64";
#elif defined(ARCH_ARM64)
  return "arm64";
#else
  return "unknown";
#endif
}

#if defined(PLATFORM_WINDOWS)
func void system_query_windows_version(system_info* out_info) {
  profile_func_begin;
  HMODULE module_handle = GetModuleHandleA("ntdll.dll");
  if (module_handle == NULL) {
    thread_log_debug("Falling back to generic Windows version name");
    cstr8_cpy(out_info->os_name, size_of(out_info->os_name), "Windows");
    profile_func_end;
    return;
  }

  rtl_get_version_fn get_version =
      (rtl_get_version_fn)GetProcAddress(module_handle, "RtlGetVersion");
  if (get_version == NULL) {
    thread_log_debug("RtlGetVersion unavailable, using generic Windows version name");
    cstr8_cpy(out_info->os_name, size_of(out_info->os_name), "Windows");
    profile_func_end;
    return;
  }

  OSVERSIONINFOW version_info;
  mem_zero(&version_info, size_of(version_info));
  version_info.dwOSVersionInfoSize = size_of(version_info);

  if (get_version(&version_info) != 0) {
    thread_log_warn("Failed to query Windows version through RtlGetVersion");
    cstr8_cpy(out_info->os_name, size_of(out_info->os_name), "Windows");
    profile_func_end;
    return;
  }

  cstr8_cpy(out_info->os_name, size_of(out_info->os_name), "Windows");
  cstr8_format(out_info->os_version,
               size_of(out_info->os_version),
               "%lu.%lu build %lu",
               (unsigned long)version_info.dwMajorVersion,
               (unsigned long)version_info.dwMinorVersion,
               (unsigned long)version_info.dwBuildNumber);
  profile_func_end;
}
#endif

func b32 system_info_query(system_info* out_info) {
  profile_func_begin;
  if (out_info == NULL) {
    profile_func_end;
    return false;
  }
  assert(out_info != NULL);

  mem_zero(out_info, size_of(*out_info));
  cstr8_cpy(out_info->architecture_name,
            size_of(out_info->architecture_name),
            system_architecture_name());

#if defined(PLATFORM_WINDOWS)
  SYSTEM_INFO native_info;
  GetNativeSystemInfo(&native_info);
  out_info->page_size = (sz)native_info.dwPageSize;
  out_info->allocation_granularity = (sz)native_info.dwAllocationGranularity;

  system_query_windows_version(out_info);

  DWORD computer_size = (DWORD)size_of(out_info->computer_name);
  if (GetComputerNameA(out_info->computer_name, &computer_size) == 0) {
    thread_log_warn("Failed to query Windows computer name");
    out_info->computer_name[0] = '\0';
  }

  DWORD user_size = (DWORD)size_of(out_info->user_name);
  if (GetUserNameA(out_info->user_name, &user_size) == 0) {
    thread_log_warn("Failed to query Windows user name");
    out_info->user_name[0] = '\0';
  } else if (user_size > 0) {
    out_info->user_name[user_size - 1] = '\0';
  }

  cstr8 home_path = getenv("USERPROFILE");
  if (home_path == NULL) {
    thread_log_debug("USERPROFILE unavailable, trying HOMEDRIVE and HOMEPATH");
    cstr8 home_drive = getenv("HOMEDRIVE");
    cstr8 home_part = getenv("HOMEPATH");
    if (home_drive != NULL && home_part != NULL) {
      cstr8_format(out_info->user_home, size_of(out_info->user_home), "%s%s", home_drive, home_part);
    }
  } else {
    cstr8_cpy(out_info->user_home, size_of(out_info->user_home), home_path);
  }

  thread_log_trace("Queried system info platform=windows arch=%s", out_info->architecture_name);
  profile_func_end;
  return true;
#elif defined(PLATFORM_UNIX)
  struct utsname uname_info;
  if (uname(&uname_info) == 0) {
    cstr8_cpy(out_info->os_name, size_of(out_info->os_name), uname_info.sysname);
    cstr8_format(out_info->os_version,
                 size_of(out_info->os_version),
                 "%s %s",
                 uname_info.release,
                 uname_info.version);
    cstr8_cpy(out_info->computer_name, size_of(out_info->computer_name), uname_info.nodename);
  } else {
    thread_log_warn("Failed to query Unix uname information");
  }

  sp page_size = (sp)sysconf(_SC_PAGESIZE);
  if (page_size > 0) {
    out_info->page_size = (sz)page_size;
    out_info->allocation_granularity = (sz)page_size;
  } else {
    thread_log_warn("Failed to query Unix page size");
  }

  cstr8 user_name = getenv("USER");
  cstr8 home_path = getenv("HOME");
  if (user_name != NULL) {
    cstr8_cpy(out_info->user_name, size_of(out_info->user_name), user_name);
  } else {
    thread_log_debug("USER environment variable unavailable");
  }
  if (home_path != NULL) {
    cstr8_cpy(out_info->user_home, size_of(out_info->user_home), home_path);
  } else {
    thread_log_debug("HOME environment variable unavailable");
  }

  if (out_info->user_name[0] == '\0' || out_info->user_home[0] == '\0') {
    struct passwd* pass_info = getpwuid(geteuid());
    if (pass_info != NULL) {
      if (out_info->user_name[0] == '\0') {
        cstr8_cpy(out_info->user_name, size_of(out_info->user_name), pass_info->pw_name);
      }
      if (out_info->user_home[0] == '\0') {
        cstr8_cpy(out_info->user_home, size_of(out_info->user_home), pass_info->pw_dir);
      }
    } else {
      thread_log_warn("Failed to query Unix passwd fallback information");
    }
  }

  thread_log_trace("Queried system info platform=unix arch=%s", out_info->architecture_name);
  profile_func_end;
  return true;
#else
  cstr8_cpy(out_info->os_name, size_of(out_info->os_name), "unknown");
  cstr8_cpy(out_info->os_version, size_of(out_info->os_version), "unknown");
  invalid_code_path;
  profile_func_end;
  return false;
#endif
}
