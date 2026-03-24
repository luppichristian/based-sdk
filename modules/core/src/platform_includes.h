// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../include/basic/env_defines.h"

#if defined(PLATFORM_WINDOWS)
#  define NOMINMAX
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#  include <dbghelp.h>
#  include <direct.h>
#  include <psapi.h>
#  include <hidsdi.h>
#elif defined(PLATFORM_LINUX)
#  include <dlfcn.h>
#  include <execinfo.h>
#  include <fcntl.h>
#  include <pwd.h>
#  include <sys/file.h>
#  include <sys/mman.h>
#  include <sys/resource.h>
#  include <sys/stat.h>
#  include <sys/sysinfo.h>
#  include <sys/types.h>
#  include <sys/utsname.h>
#  include <unistd.h>
#  include <libinput.h>
#  include <libudev.h>
#  include <poll.h>
#  include <fcntl.h>
#elif defined(PLATFORM_MACOS)
#  include <dlfcn.h>
#  include <execinfo.h>
#  include <fcntl.h>
#  include <mach/mach.h>
#  include <mach/task.h>
#  include <pwd.h>
#  include <sys/file.h>
#  include <sys/mman.h>
#  include <sys/resource.h>
#  include <sys/stat.h>
#  include <sys/sysctl.h>
#  include <sys/types.h>
#  include <sys/utsname.h>
#  include <unistd.h>
#  include <CoreFoundation/CoreFoundation.h>
#  include <IOKit/hid/IOHIDManager.h>
#  include <IOKit/hid/IOHIDKeys.h>
#  include <IOKit/hidsystem/IOHIDUsageTables.h>
#endif
