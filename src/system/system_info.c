// MIT License
// Copyright (c) 2026 Christian Luppi

#include "system/system_info.h"
#include "basic/env_defines.h"

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

func void system_copy_string(c8* dst_ptr, sz dst_cap, const c8* src_ptr) {
  if (dst_ptr == NULL || dst_cap == 0) {
    return;
  }

  dst_ptr[0] = '\0';
  if (src_ptr == NULL) {
    return;
  }

  sz src_len = strlen(src_ptr);
  if (src_len >= dst_cap) {
    src_len = dst_cap - 1;
  }

  memcpy(dst_ptr, src_ptr, src_len);
  dst_ptr[src_len] = '\0';
}

func const c8* system_architecture_name(void) {
#if defined(ARCH_X86_64)
  return "x86_64";
#elif defined(ARCH_X86)
  return "x86";
#elif defined(ARCH_ARM64)
  return "arm64";
#elif defined(ARCH_ARM)
  return "arm";
#elif defined(ARCH_RISCV)
  return "riscv";
#elif defined(ARCH_POWERPC)
  return "powerpc";
#elif defined(ARCH_MIPS)
  return "mips";
#elif defined(ARCH_SPARC)
  return "sparc";
#elif defined(ARCH_WASM)
  return "wasm";
#else
  return "unknown";
#endif
}

#if defined(PLATFORM_WINDOWS)
func void system_query_windows_version(system_info* out_info) {
  HMODULE module_handle = GetModuleHandleA("ntdll.dll");
  if (module_handle == NULL) {
    system_copy_string(out_info->os_name, SYSTEM_INFO_OS_NAME_CAP, "Windows");
    return;
  }

  rtl_get_version_fn get_version =
      (rtl_get_version_fn)GetProcAddress(module_handle, "RtlGetVersion");
  if (get_version == NULL) {
    system_copy_string(out_info->os_name, SYSTEM_INFO_OS_NAME_CAP, "Windows");
    return;
  }

  OSVERSIONINFOW version_info;
  memset(&version_info, 0, sizeof(version_info));
  version_info.dwOSVersionInfoSize = sizeof(version_info);

  if (get_version(&version_info) != 0) {
    system_copy_string(out_info->os_name, SYSTEM_INFO_OS_NAME_CAP, "Windows");
    return;
  }

  system_copy_string(out_info->os_name, SYSTEM_INFO_OS_NAME_CAP, "Windows");
  snprintf(out_info->os_version,
           SYSTEM_INFO_OS_VERSION_CAP,
           "%lu.%lu build %lu",
           (unsigned long)version_info.dwMajorVersion,
           (unsigned long)version_info.dwMinorVersion,
           (unsigned long)version_info.dwBuildNumber);
}
#endif

func b32 system_info_query(system_info* out_info) {
  if (out_info == NULL) {
    return 0;
  }

  memset(out_info, 0, sizeof(*out_info));
  system_copy_string(out_info->architecture_name,
                     SYSTEM_INFO_ARCH_NAME_CAP,
                     system_architecture_name());

#if defined(PLATFORM_WINDOWS)
  SYSTEM_INFO native_info;
  GetNativeSystemInfo(&native_info);
  out_info->page_size = (sz)native_info.dwPageSize;
  out_info->allocation_granularity = (sz)native_info.dwAllocationGranularity;

  system_query_windows_version(out_info);

  DWORD computer_size = (DWORD)SYSTEM_INFO_COMPUTER_NAME_CAP;
  if (GetComputerNameA(out_info->computer_name, &computer_size) == 0) {
    out_info->computer_name[0] = '\0';
  }

  DWORD user_size = (DWORD)SYSTEM_INFO_USER_NAME_CAP;
  if (GetUserNameA(out_info->user_name, &user_size) == 0) {
    out_info->user_name[0] = '\0';
  } else if (user_size > 0) {
    out_info->user_name[user_size - 1] = '\0';
  }

  const c8* home_path = getenv("USERPROFILE");
  if (home_path == NULL) {
    const c8* home_drive = getenv("HOMEDRIVE");
    const c8* home_part = getenv("HOMEPATH");
    if (home_drive != NULL && home_part != NULL) {
      snprintf(out_info->user_home, SYSTEM_INFO_HOME_PATH_CAP, "%s%s", home_drive, home_part);
    }
  } else {
    system_copy_string(out_info->user_home, SYSTEM_INFO_HOME_PATH_CAP, home_path);
  }

  return 1;
#elif defined(PLATFORM_UNIX) || defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
  struct utsname uname_info;
  if (uname(&uname_info) == 0) {
    system_copy_string(out_info->os_name, SYSTEM_INFO_OS_NAME_CAP, uname_info.sysname);
    snprintf(out_info->os_version,
             SYSTEM_INFO_OS_VERSION_CAP,
             "%s %s",
             uname_info.release,
             uname_info.version);
    system_copy_string(out_info->computer_name, SYSTEM_INFO_COMPUTER_NAME_CAP, uname_info.nodename);
  }

  sp page_size = (sp)sysconf(_SC_PAGESIZE);
  if (page_size > 0) {
    out_info->page_size = (sz)page_size;
    out_info->allocation_granularity = (sz)page_size;
  }

  const c8* user_name = getenv("USER");
  const c8* home_path = getenv("HOME");
  if (user_name != NULL) {
    system_copy_string(out_info->user_name, SYSTEM_INFO_USER_NAME_CAP, user_name);
  }
  if (home_path != NULL) {
    system_copy_string(out_info->user_home, SYSTEM_INFO_HOME_PATH_CAP, home_path);
  }

  if (out_info->user_name[0] == '\0' || out_info->user_home[0] == '\0') {
    struct passwd* pass_info = getpwuid(geteuid());
    if (pass_info != NULL) {
      if (out_info->user_name[0] == '\0') {
        system_copy_string(out_info->user_name, SYSTEM_INFO_USER_NAME_CAP, pass_info->pw_name);
      }
      if (out_info->user_home[0] == '\0') {
        system_copy_string(out_info->user_home, SYSTEM_INFO_HOME_PATH_CAP, pass_info->pw_dir);
      }
    }
  }

  return 1;
#else
  system_copy_string(out_info->os_name, SYSTEM_INFO_OS_NAME_CAP, "unknown");
  system_copy_string(out_info->os_version, SYSTEM_INFO_OS_VERSION_CAP, "unknown");
  return 0;
#endif
}
