// MIT License
// Copyright (c) 2026 Christian Luppi

#include "system/system_info.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "basic/env_defines.h"
#include "basic/profiler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(PLATFORM_WINDOWS)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#elif defined(PLATFORM_UNIX) || defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
#  include <sys/types.h>
#  include <sys/utsname.h>
#  include <unistd.h>
#  if defined(PLATFORM_MACOS) || defined(PLATFORM_IOS)
#    include <pwd.h>
#    include <sys/sysctl.h>
#  else
#    include <pwd.h>
#  endif
#endif

#if defined(PLATFORM_WINDOWS)
typedef LONG(WINAPI* rtl_get_version_fn)(void* version_info);
#endif

func void system_copy_string(c8* dst_ptr, sz dst_cap, cstr8 src_ptr) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (dst_ptr == NULL || dst_cap == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(dst_cap > 0);

  dst_ptr[0] = '\0';
  if (src_ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  sz src_len = cstr8_len(src_ptr);
  if (src_len >= dst_cap) {
    src_len = dst_cap - 1;
  }

  memcpy(dst_ptr, src_ptr, src_len);
  dst_ptr[src_len] = '\0';
  TracyCZoneEnd(__tracy_zone_ctx);
}

func cstr8 system_architecture_name(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
#if defined(ARCH_X86_64)
  TracyCZoneEnd(__tracy_zone_ctx);
  return "x86_64";
#elif defined(ARCH_X86)
  TracyCZoneEnd(__tracy_zone_ctx);
  return "x86";
#elif defined(ARCH_ARM64)
  TracyCZoneEnd(__tracy_zone_ctx);
  return "arm64";
#elif defined(ARCH_ARM)
  TracyCZoneEnd(__tracy_zone_ctx);
  return "arm";
#elif defined(ARCH_RISCV)
  TracyCZoneEnd(__tracy_zone_ctx);
  return "riscv";
#elif defined(ARCH_POWERPC)
  TracyCZoneEnd(__tracy_zone_ctx);
  return "powerpc";
#elif defined(ARCH_MIPS)
  TracyCZoneEnd(__tracy_zone_ctx);
  return "mips";
#elif defined(ARCH_SPARC)
  TracyCZoneEnd(__tracy_zone_ctx);
  return "sparc";
#elif defined(ARCH_WASM)
  TracyCZoneEnd(__tracy_zone_ctx);
  return "wasm";
#else
  TracyCZoneEnd(__tracy_zone_ctx);
  return "unknown";
#endif
}

#if defined(PLATFORM_WINDOWS)
func void system_query_windows_version(system_info* out_info) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  HMODULE module_handle = GetModuleHandleA("ntdll.dll");
  if (module_handle == NULL) {
    system_copy_string(out_info->os_name, size_of(out_info->os_name), "Windows");
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  rtl_get_version_fn get_version =
      (rtl_get_version_fn)GetProcAddress(module_handle, "RtlGetVersion");
  if (get_version == NULL) {
    system_copy_string(out_info->os_name, size_of(out_info->os_name), "Windows");
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  OSVERSIONINFOW version_info;
  memset(&version_info, 0, size_of(version_info));
  version_info.dwOSVersionInfoSize = size_of(version_info);

  if (get_version(&version_info) != 0) {
    system_copy_string(out_info->os_name, size_of(out_info->os_name), "Windows");
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  system_copy_string(out_info->os_name, size_of(out_info->os_name), "Windows");
  cstr8_format(out_info->os_version,
               size_of(out_info->os_version),
               "%lu.%lu build %lu",
               (unsigned long)version_info.dwMajorVersion,
               (unsigned long)version_info.dwMinorVersion,
               (unsigned long)version_info.dwBuildNumber);
  TracyCZoneEnd(__tracy_zone_ctx);
}
#endif

func b32 system_info_query(system_info* out_info) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (out_info == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(out_info != NULL);

  memset(out_info, 0, size_of(*out_info));
  system_copy_string(out_info->architecture_name,
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
    out_info->computer_name[0] = '\0';
  }

  DWORD user_size = (DWORD)size_of(out_info->user_name);
  if (GetUserNameA(out_info->user_name, &user_size) == 0) {
    out_info->user_name[0] = '\0';
  } else if (user_size > 0) {
    out_info->user_name[user_size - 1] = '\0';
  }

  cstr8 home_path = getenv("USERPROFILE");
  if (home_path == NULL) {
    cstr8 home_drive = getenv("HOMEDRIVE");
    cstr8 home_part = getenv("HOMEPATH");
    if (home_drive != NULL && home_part != NULL) {
      cstr8_format(out_info->user_home, size_of(out_info->user_home), "%s%s", home_drive, home_part);
    }
  } else {
    system_copy_string(out_info->user_home, size_of(out_info->user_home), home_path);
  }

  thread_log_trace("system_info_query: platform=windows arch=%s", out_info->architecture_name);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
#elif defined(PLATFORM_UNIX) || defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
  struct utsname uname_info;
  if (uname(&uname_info) == 0) {
    system_copy_string(out_info->os_name, size_of(out_info->os_name), uname_info.sysname);
    cstr8_format(out_info->os_version,
                 size_of(out_info->os_version),
                 "%s %s",
                 uname_info.release,
                 uname_info.version);
    system_copy_string(out_info->computer_name, size_of(out_info->computer_name), uname_info.nodename);
  }

  sp page_size = (sp)sysconf(_SC_PAGESIZE);
  if (page_size > 0) {
    out_info->page_size = (sz)page_size;
    out_info->allocation_granularity = (sz)page_size;
  }

  cstr8 user_name = getenv("USER");
  cstr8 home_path = getenv("HOME");
  if (user_name != NULL) {
    system_copy_string(out_info->user_name, size_of(out_info->user_name), user_name);
  }
  if (home_path != NULL) {
    system_copy_string(out_info->user_home, size_of(out_info->user_home), home_path);
  }

  if (out_info->user_name[0] == '\0' || out_info->user_home[0] == '\0') {
    struct passwd* pass_info = getpwuid(geteuid());
    if (pass_info != NULL) {
      if (out_info->user_name[0] == '\0') {
        system_copy_string(out_info->user_name, size_of(out_info->user_name), pass_info->pw_name);
      }
      if (out_info->user_home[0] == '\0') {
        system_copy_string(out_info->user_home, size_of(out_info->user_home), pass_info->pw_dir);
      }
    }
  }

  thread_log_trace("system_info_query: platform=unix arch=%s", out_info->architecture_name);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
#else
  system_copy_string(out_info->os_name, size_of(out_info->os_name), "unknown");
  system_copy_string(out_info->os_version, size_of(out_info->os_version), "unknown");
  thread_log_warn("system_info_query: unknown platform");
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
#endif
}
