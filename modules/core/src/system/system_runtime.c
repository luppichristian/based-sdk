// MIT License
// Copyright (c) 2026 Christian Luppi

#include "system/system_runtime.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "basic/env_defines.h"
#include "basic/profiler.h"
#include "memory/memops.h"
#include "platform_includes.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#if defined(PLATFORM_WINDOWS)
typedef struct runtime_cpu_sample {
  u64 idle_ticks;
  u64 kernel_ticks;
  u64 user_ticks;
} runtime_cpu_sample;

typedef BOOL(WINAPI* get_process_memory_info_fn)(
    HANDLE process_handle,
    PPROCESS_MEMORY_COUNTERS counters,
    DWORD counter_size);

func u64 runtime_filetime_to_u64(FILETIME value) {
  profile_func_begin;
  ULARGE_INTEGER wide_value;
  wide_value.LowPart = value.dwLowDateTime;
  wide_value.HighPart = value.dwHighDateTime;
  profile_func_end;
  return wide_value.QuadPart;
}

func b32 runtime_query_windows_cpu(runtime_cpu_sample* out_sample) {
  profile_func_begin;
  if (out_sample == NULL) {
    profile_func_end;
    return false;
  }
  assert(out_sample != NULL);
  FILETIME idle_time;
  FILETIME kernel_time;
  FILETIME user_time;
  if (GetSystemTimes(&idle_time, &kernel_time, &user_time) == 0) {
    thread_log_warn("Failed to query Windows CPU times");
    profile_func_end;
    return false;
  }

  out_sample->idle_ticks = runtime_filetime_to_u64(idle_time);
  out_sample->kernel_ticks = runtime_filetime_to_u64(kernel_time);
  out_sample->user_ticks = runtime_filetime_to_u64(user_time);
  profile_func_end;
  return true;
}
#elif defined(PLATFORM_LINUX)
typedef struct runtime_cpu_sample {
  u64 idle_ticks;
  u64 total_ticks;
} runtime_cpu_sample;

func b32 runtime_query_linux_cpu(runtime_cpu_sample* out_sample) {
  profile_func_begin;
  if (out_sample == NULL) {
    profile_func_end;
    return false;
  }
  assert(out_sample != NULL);
  FILE* stat_file = fopen("/proc/stat", "r");
  if (stat_file == NULL) {
    thread_log_warn("Failed to open Linux CPU stats at /proc/stat");
    profile_func_end;
    return false;
  }

  c8 line_buffer[256];
  if (fgets(line_buffer, size_of(line_buffer), stat_file) == NULL) {
    thread_log_warn("Failed to read Linux CPU stats at /proc/stat");
    fclose(stat_file);
    profile_func_end;
    return false;
  }
  fclose(stat_file);

  unsigned long long user_ticks = 0;
  unsigned long long nice_ticks = 0;
  unsigned long long system_ticks = 0;
  unsigned long long idle_ticks = 0;
  unsigned long long io_wait_ticks = 0;
  unsigned long long irq_ticks = 0;
  unsigned long long soft_irq_ticks = 0;
  unsigned long long steal_ticks = 0;
  i32 parsed_count = sscanf(line_buffer,
                            "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
                            &user_ticks,
                            &nice_ticks,
                            &system_ticks,
                            &idle_ticks,
                            &io_wait_ticks,
                            &irq_ticks,
                            &soft_irq_ticks,
                            &steal_ticks);
  if (parsed_count < 4) {
    thread_log_warn("Failed to parse Linux CPU stats line");
    profile_func_end;
    return false;
  }

  out_sample->idle_ticks = (u64)idle_ticks + (u64)io_wait_ticks;
  out_sample->total_ticks = (u64)user_ticks + (u64)nice_ticks + (u64)system_ticks +
                            (u64)idle_ticks + (u64)io_wait_ticks + (u64)irq_ticks +
                            (u64)soft_irq_ticks + (u64)steal_ticks;
  profile_func_end;
  return true;
}
#elif defined(PLATFORM_MACOS)
typedef struct runtime_cpu_sample {
  u64 idle_ticks;
  u64 total_ticks;
} runtime_cpu_sample;

func b32 runtime_query_apple_cpu(runtime_cpu_sample* out_sample) {
  profile_func_begin;
  if (out_sample == NULL) {
    profile_func_end;
    return false;
  }
  assert(out_sample != NULL);
  host_cpu_load_info_data_t load_info;
  mach_msg_type_number_t load_count = HOST_CPU_LOAD_INFO_COUNT;
  kern_return_t query_result =
      host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&load_info, &load_count);
  if (query_result != KERN_SUCCESS) {
    thread_log_warn("Failed to query Apple CPU stats result=%d", (i32)query_result);
    profile_func_end;
    return false;
  }

  out_sample->idle_ticks = (u64)load_info.cpu_ticks[CPU_STATE_IDLE];
  out_sample->total_ticks = (u64)load_info.cpu_ticks[CPU_STATE_USER] +
                            (u64)load_info.cpu_ticks[CPU_STATE_SYSTEM] +
                            (u64)load_info.cpu_ticks[CPU_STATE_IDLE] +
                            (u64)load_info.cpu_ticks[CPU_STATE_NICE];
  profile_func_end;
  return true;
}
#endif

func b32 system_runtime_query(system_runtime_info* out_info) {
  profile_func_begin;
  if (out_info == NULL) {
    profile_func_end;
    return false;
  }
  assert(out_info != NULL);

  mem_zero(out_info, size_of(*out_info));

#if defined(PLATFORM_WINDOWS)
  MEMORYSTATUSEX memory_status;
  mem_zero(&memory_status, size_of(memory_status));
  memory_status.dwLength = size_of(memory_status);
  if (GlobalMemoryStatusEx(&memory_status) == 0) {
    thread_log_error("Failed to query Windows memory status");
    profile_func_end;
    return false;
  }

  out_info->memory_total = (sz)memory_status.ullTotalPhys;
  out_info->memory_available = (sz)memory_status.ullAvailPhys;
  out_info->memory_used = out_info->memory_total - out_info->memory_available;

  PROCESS_MEMORY_COUNTERS memory_counters;
  mem_zero(&memory_counters, size_of(memory_counters));
  get_process_memory_info_fn get_memory_info = NULL;
  HMODULE psapi_module = NULL;
  HMODULE kernel_module = GetModuleHandleA("kernel32.dll");
  if (kernel_module != NULL) {
    get_memory_info = (get_process_memory_info_fn)GetProcAddress(kernel_module, "K32GetProcessMemoryInfo");
  }

  if (get_memory_info == NULL) {
    psapi_module = LoadLibraryA("psapi.dll");
    if (psapi_module != NULL) {
      get_memory_info =
          (get_process_memory_info_fn)GetProcAddress(psapi_module, "GetProcessMemoryInfo");
    }
  }

  if (get_memory_info != NULL &&
      get_memory_info(GetCurrentProcess(), &memory_counters, size_of(memory_counters)) != 0) {
    out_info->process_memory_used = (sz)memory_counters.WorkingSetSize;
    out_info->process_memory_peak = (sz)memory_counters.PeakWorkingSetSize;
  } else {
    thread_log_warn("Failed to query Windows process memory usage");
  }
  if (psapi_module != NULL) {
    FreeLibrary(psapi_module);
  }

  runtime_cpu_sample cpu_sample;
  if (runtime_query_windows_cpu(&cpu_sample)) {
    local_persist b32 has_prev_sample = false;
    local_persist runtime_cpu_sample prev_sample;

    if (has_prev_sample) {
      u64 idle_delta = cpu_sample.idle_ticks - prev_sample.idle_ticks;
      u64 kernel_delta = cpu_sample.kernel_ticks - prev_sample.kernel_ticks;
      u64 user_delta = cpu_sample.user_ticks - prev_sample.user_ticks;
      u64 total_delta = kernel_delta + user_delta;
      if (total_delta > 0 && total_delta >= idle_delta) {
        out_info->cpu_usage_percent =
            (f32)((f64)(total_delta - idle_delta) * 100.0 / (f64)total_delta);
      }
    }

    prev_sample = cpu_sample;
    has_prev_sample = true;
  } else {
    thread_log_warn("Failed to query Windows CPU sample");
  }

  thread_log_trace("Queried runtime info platform=windows mem_used=%zu", (size_t)out_info->memory_used);
  profile_func_end;
  return true;
#elif defined(PLATFORM_LINUX)
  struct sysinfo sys_info;
  if (sysinfo(&sys_info) != 0) {
    thread_log_error("Failed to query Linux sysinfo");
    profile_func_end;
    return false;
  }

  u64 total_memory = (u64)sys_info.totalram * (u64)sys_info.mem_unit;
  u64 free_memory = (u64)sys_info.freeram * (u64)sys_info.mem_unit;
  u64 buffer_memory = (u64)sys_info.bufferram * (u64)sys_info.mem_unit;
  out_info->memory_total = (sz)total_memory;
  out_info->memory_available = (sz)(free_memory + buffer_memory);
  if (out_info->memory_available > out_info->memory_total) {
    out_info->memory_available = out_info->memory_total;
  }
  out_info->memory_used = out_info->memory_total - out_info->memory_available;

  sp page_size = (sp)sysconf(_SC_PAGESIZE);
  if (page_size > 0) {
    FILE* statm_file = fopen("/proc/self/statm", "r");
    if (statm_file != NULL) {
      unsigned long long total_pages = 0;
      unsigned long long resident_pages = 0;
      if (fscanf(statm_file, "%llu %llu", &total_pages, &resident_pages) == 2) {
        (void)total_pages;
        out_info->process_memory_used = (sz)((u64)resident_pages * (u64)page_size);
      }
      fclose(statm_file);
    } else {
      thread_log_warn("Failed to open Linux process memory stats at /proc/self/statm");
    }
  }

  struct rusage usage_info;
  mem_zero(&usage_info, size_of(usage_info));
  if (getrusage(RUSAGE_SELF, &usage_info) == 0) {
    out_info->process_memory_peak = (sz)((u64)usage_info.ru_maxrss * 1024ULL);
  } else {
    thread_log_warn("Failed to query Linux process peak memory usage");
  }

  if (out_info->process_memory_peak < out_info->process_memory_used) {
    out_info->process_memory_peak = out_info->process_memory_used;
  }

  runtime_cpu_sample cpu_sample;
  if (runtime_query_linux_cpu(&cpu_sample)) {
    local_persist b32 has_prev_sample = false;
    local_persist runtime_cpu_sample prev_sample;

    if (has_prev_sample) {
      u64 idle_delta = cpu_sample.idle_ticks - prev_sample.idle_ticks;
      u64 total_delta = cpu_sample.total_ticks - prev_sample.total_ticks;
      if (total_delta > 0 && total_delta >= idle_delta) {
        out_info->cpu_usage_percent =
            (f32)((f64)(total_delta - idle_delta) * 100.0 / (f64)total_delta);
      }
    }

    prev_sample = cpu_sample;
    has_prev_sample = true;
  } else {
    thread_log_warn("Failed to query Linux CPU sample");
  }

  thread_log_trace("Queried runtime info platform=linux mem_used=%zu", (size_t)out_info->memory_used);
  profile_func_end;
  return true;
#elif defined(PLATFORM_MACOS)
  u64 total_memory = 0;
  sz total_size = size_of(total_memory);
  if (sysctlbyname("hw.memsize", &total_memory, &total_size, NULL, 0) == 0) {
    out_info->memory_total = (sz)total_memory;
  } else {
    thread_log_warn("Failed to query Apple total memory");
  }

  vm_statistics64_data_t vm_info;
  mach_msg_type_number_t vm_count = HOST_VM_INFO64_COUNT;
  if (host_statistics64(mach_host_self(), HOST_VM_INFO64, (host_info64_t)&vm_info, &vm_count) ==
      KERN_SUCCESS) {
    vm_size_t page_size = 0;
    host_page_size(mach_host_self(), &page_size);
    u64 available_memory = ((u64)vm_info.free_count + (u64)vm_info.inactive_count) * (u64)page_size;
    out_info->memory_available = (sz)available_memory;
    if (out_info->memory_total >= out_info->memory_available) {
      out_info->memory_used = out_info->memory_total - out_info->memory_available;
    }
  } else {
    thread_log_warn("Failed to query Apple VM statistics");
  }

  mach_task_basic_info_data_t task_data;
  mach_msg_type_number_t task_count = MACH_TASK_BASIC_INFO_COUNT;
  if (task_info(mach_task_self(),
                MACH_TASK_BASIC_INFO,
                (task_info_t)&task_data,
                &task_count) == KERN_SUCCESS) {
    out_info->process_memory_used = (sz)task_data.resident_size;
    out_info->process_memory_peak = (sz)task_data.resident_size_max;
  } else {
    thread_log_warn("Failed to query Apple task memory info");
  }

  runtime_cpu_sample cpu_sample;
  if (runtime_query_apple_cpu(&cpu_sample)) {
    local_persist b32 has_prev_sample = false;
    local_persist runtime_cpu_sample prev_sample;

    if (has_prev_sample) {
      u64 idle_delta = cpu_sample.idle_ticks - prev_sample.idle_ticks;
      u64 total_delta = cpu_sample.total_ticks - prev_sample.total_ticks;
      if (total_delta > 0 && total_delta >= idle_delta) {
        out_info->cpu_usage_percent =
            (f32)((f64)(total_delta - idle_delta) * 100.0 / (f64)total_delta);
      }
    }

    prev_sample = cpu_sample;
    has_prev_sample = true;
  } else {
    thread_log_warn("Failed to query Apple CPU sample");
  }

  thread_log_trace("Queried runtime info platform=apple mem_used=%zu", (size_t)out_info->memory_used);
  profile_func_end;
  return true;
#else
  invalid_code_path;
  profile_func_end;
  return false;
#endif
}
