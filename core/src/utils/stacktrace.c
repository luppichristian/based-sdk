// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/stacktrace.h"
#include "basic/profiler.h"

#if defined(_WIN32)
#  include <windows.h>
#  include <dbghelp.h>
#else
#  include <dlfcn.h>
#  include <execinfo.h>
#endif

func sz stacktrace_capture(stacktrace_frame* out_frames, sz out_capacity, sz skip_frames) {
  profile_func_begin;
  if (out_frames == NULL || out_capacity == 0) {
    profile_func_end;
    return 0;
  }

  sz frame_idx = 0;
  sz capture_cap = min(out_capacity + skip_frames + 1, STACKTRACE_CAPTURE_CAP);

  void* raw_frames[STACKTRACE_CAPTURE_CAP] = {0};

#if defined(_WIN32)
  HANDLE process = GetCurrentProcess();
  SymInitialize(process, NULL, TRUE);

  USHORT captured = CaptureStackBackTrace(
      (ULONG)(skip_frames + 1),
      (ULONG)capture_cap,
      raw_frames,
      NULL);

  for (frame_idx = 0; frame_idx < (sz)captured && frame_idx < out_capacity; ++frame_idx) {
    out_frames[frame_idx].address = (up)raw_frames[frame_idx];
    out_frames[frame_idx].symbol[0] = '\0';

    u8 sym_storage[size_of(SYMBOL_INFO) + STR_CAP_MEDIUM] = {0};
    SYMBOL_INFO* symbol_info = (SYMBOL_INFO*)sym_storage;
    symbol_info->SizeOfStruct = size_of(SYMBOL_INFO);
    symbol_info->MaxNameLen = STR_CAP_MEDIUM - 1;

    DWORD64 displacement = 0;
    if (SymFromAddr(process, (DWORD64)raw_frames[frame_idx], &displacement, symbol_info)) {
      cstr8_format(
          out_frames[frame_idx].symbol,
          STR_CAP_MEDIUM,
          "%s+0x%llx",
          symbol_info->Name,
          (unsigned long long)displacement);
    } else {
      cstr8_format(
          out_frames[frame_idx].symbol,
          STR_CAP_MEDIUM,
          "0x%p",
          raw_frames[frame_idx]);
    }
  }
#else
  i32 captured = backtrace(raw_frames, (i32)capture_cap);
  if (captured <= 0) {
    profile_func_end;
    return 0;
  }

  i32 begin_idx = (i32)(skip_frames + 1);
  if (begin_idx > captured) {
    begin_idx = captured;
  }

  for (i32 idx = begin_idx; idx < captured && frame_idx < out_capacity; ++idx, ++frame_idx) {
    out_frames[frame_idx].address = (up)raw_frames[idx];
    out_frames[frame_idx].symbol[0] = '\0';

    Dl_info info = {0};
    if (dladdr(raw_frames[idx], &info) != 0) {
      cstr8 symbol_name = info.dli_sname != NULL ? info.dli_sname : "<unknown>";
      cstr8 module_name = info.dli_fname != NULL ? info.dli_fname : "<module>";

      up sym_addr = (up)info.dli_saddr;
      up frame_addr = (up)raw_frames[idx];
      up displacement = 0;
      if (sym_addr != 0 && frame_addr >= sym_addr) {
        displacement = frame_addr - sym_addr;
      }

      cstr8_format(
          out_frames[frame_idx].symbol,
          STR_CAP_MEDIUM,
          "%s!%s+0x%llx",
          module_name,
          symbol_name,
          (unsigned long long)displacement);
    } else {
      cstr8_format(out_frames[frame_idx].symbol, STR_CAP_MEDIUM, "0x%p", raw_frames[idx]);
    }
  }
#endif

  profile_func_end;
  return frame_idx;
}
