// MIT License
// Copyright (c) 2026 Christian Luppi

#include "system/system_runtime.h"
#include "basic/env_defines.h"

#include <stddef.h>
#include <string.h>

#if defined(PLATFORM_WINDOWS)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <psapi.h>
#elif defined(PLATFORM_LINUX)
#  include <stdio.h>
#  include <sys/resource.h>
#  include <sys/sysinfo.h>
#  include <unistd.h>
#elif defined(PLATFORM_MACOS) || defined(PLATFORM_IOS)
#  include <mach/mach.h>
#  include <mach/task.h>
#  include <sys/resource.h>
#  include <sys/sysctl.h>
#  include <unistd.h>
#endif

#if defined(PLATFORM_WINDOWS)
typedef struct runtime_cpu_sample {
  u64 idle_ticks;
  u64 kernel_ticks;
  u64 user_ticks;
} runtime_cpu_sample;

typedef BOOL(WINAPI* get_process_memory_info_fn)(HANDLE process_handle,
                                                 PPROCESS_MEMORY_COUNTERS counters,
                                                 DWORD counter_size);

func u64 runtime_filetime_to_u64(FILETIME value) {
  ULARGE_INTEGER wide_value;
  wide_value.LowPart = value.dwLowDateTime;
  wide_value.HighPart = value.dwHighDateTime;
  return wide_value.QuadPart;
}

func b32 runtime_query_windows_cpu(runtime_cpu_sample* out_sample) {
  FILETIME idle_time;
  FILETIME kernel_time;
  FILETIME user_time;
  if (GetSystemTimes(&idle_time, &kernel_time, &user_time) == 0) {
    return 0;
  }

  out_sample->idle_ticks = runtime_filetime_to_u64(idle_time);
  out_sample->kernel_ticks = runtime_filetime_to_u64(kernel_time);
  out_sample->user_ticks = runtime_filetime_to_u64(user_time);
  return 1;
}
#elif defined(PLATFORM_LINUX)
typedef struct runtime_cpu_sample {
  u64 idle_ticks;
  u64 total_ticks;
} runtime_cpu_sample;

func b32 runtime_query_linux_cpu(runtime_cpu_sample* out_sample) {
  FILE* stat_file = fopen("/proc/stat", "r");
  if (stat_file == NULL) {
    return 0;
  }

  c8 line_buffer[256];
  if (fgets(line_buffer, sizeof(line_buffer), stat_file) == NULL) {
    fclose(stat_file);
    return 0;
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
    return 0;
  }

  out_sample->idle_ticks = (u64)idle_ticks + (u64)io_wait_ticks;
  out_sample->total_ticks = (u64)user_ticks + (u64)nice_ticks + (u64)system_ticks +
                            (u64)idle_ticks + (u64)io_wait_ticks + (u64)irq_ticks +
                            (u64)soft_irq_ticks + (u64)steal_ticks;
  return 1;
}
#elif defined(PLATFORM_MACOS) || defined(PLATFORM_IOS)
typedef struct runtime_cpu_sample {
  u64 idle_ticks;
  u64 total_ticks;
} runtime_cpu_sample;

func b32 runtime_query_apple_cpu(runtime_cpu_sample* out_sample) {
  host_cpu_load_info_data_t load_info;
  mach_msg_type_number_t load_count = HOST_CPU_LOAD_INFO_COUNT;
  kern_return_t query_result =
      host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&load_info, &load_count);
  if (query_result != KERN_SUCCESS) {
    return 0;
  }

  out_sample->idle_ticks = (u64)load_info.cpu_ticks[CPU_STATE_IDLE];
  out_sample->total_ticks = (u64)load_info.cpu_ticks[CPU_STATE_USER] +
                            (u64)load_info.cpu_ticks[CPU_STATE_SYSTEM] +
                            (u64)load_info.cpu_ticks[CPU_STATE_IDLE] +
                            (u64)load_info.cpu_ticks[CPU_STATE_NICE];
  return 1;
}
#endif

func b32 system_runtime_query(system_runtime_info* out_info) {
  if (out_info == NULL) {
    return 0;
  }

  memset(out_info, 0, sizeof(*out_info));

#if defined(PLATFORM_WINDOWS)
  MEMORYSTATUSEX memory_status;
  memset(&memory_status, 0, sizeof(memory_status));
  memory_status.dwLength = sizeof(memory_status);
  if (GlobalMemoryStatusEx(&memory_status) == 0) {
    return 0;
  }

  out_info->memory_total = (sz)memory_status.ullTotalPhys;
  out_info->memory_available = (sz)memory_status.ullAvailPhys;
  out_info->memory_used = out_info->memory_total - out_info->memory_available;

  PROCESS_MEMORY_COUNTERS memory_counters;
  memset(&memory_counters, 0, sizeof(memory_counters));
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
      get_memory_info(GetCurrentProcess(), &memory_counters, sizeof(memory_counters)) != 0) {
    out_info->process_memory_used = (sz)memory_counters.WorkingSetSize;
    out_info->process_memory_peak = (sz)memory_counters.PeakWorkingSetSize;
  }
  if (psapi_module != NULL) {
    FreeLibrary(psapi_module);
  }

  runtime_cpu_sample cpu_sample;
  if (runtime_query_windows_cpu(&cpu_sample)) {
    local_persist b32 has_prev_sample = 0;
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
    has_prev_sample = 1;
  }

  return 1;
#elif defined(PLATFORM_LINUX)
  struct sysinfo sys_info;
  if (sysinfo(&sys_info) != 0) {
    return 0;
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
    }
  }

  struct rusage usage_info;
  memset(&usage_info, 0, sizeof(usage_info));
  if (getrusage(RUSAGE_SELF, &usage_info) == 0) {
    out_info->process_memory_peak = (sz)((u64)usage_info.ru_maxrss * 1024ULL);
  }

  runtime_cpu_sample cpu_sample;
  if (runtime_query_linux_cpu(&cpu_sample)) {
    local_persist b32 has_prev_sample = 0;
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
    has_prev_sample = 1;
  }

  return 1;
#elif defined(PLATFORM_MACOS) || defined(PLATFORM_IOS)
  u64 total_memory = 0;
  sz total_size = sizeof(total_memory);
  if (sysctlbyname("hw.memsize", &total_memory, &total_size, NULL, 0) == 0) {
    out_info->memory_total = (sz)total_memory;
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
  }

  mach_task_basic_info_data_t task_data;
  mach_msg_type_number_t task_count = MACH_TASK_BASIC_INFO_COUNT;
  if (task_info(mach_task_self(),
                MACH_TASK_BASIC_INFO,
                (task_info_t)&task_data,
                &task_count) == KERN_SUCCESS) {
    out_info->process_memory_used = (sz)task_data.resident_size;
    out_info->process_memory_peak = (sz)task_data.resident_size_max;
  }

  runtime_cpu_sample cpu_sample;
  if (runtime_query_apple_cpu(&cpu_sample)) {
    local_persist b32 has_prev_sample = 0;
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
    has_prev_sample = 1;
  }

  return 1;
#else
  return 0;
#endif
}
