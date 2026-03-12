// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/thread_group.h"
#include "basic/assert.h"
#include "basic/profiler.h"
#include "context/global_ctx.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "memory/memops.h"
#include "basic/safe.h"

typedef struct thread_group_payload {
  thread_group_func entry;
  void* arg;
  u32 idx;
} thread_group_payload;

typedef struct thread_group_data {
  thread* threads;
  u32 count;
} thread_group_data;

func thread_group_data* thread_group_data_from_handle(thread_group group) {
  return (thread_group_data*)group;
}

func void thread_group_destroy_storage(heap* hp, thread_group_data* group) {
  if (hp == NULL || group == NULL) {
    return;
  }

  heap_dealloc(hp, group->threads);
  heap_dealloc(hp, group);
}

func void thread_group_force_wait(thread thd) {
  if (!thread_is_valid(thd)) {
    return;
  }

  SDL_WaitThread((SDL_Thread*)thd, NULL);
}

func void thread_group_cleanup_failed_create(thread_group_data* group, u32 created_count, callsite site) {
  if (group == NULL) {
    return;
  }

  safe_for (u32 idx = 0; idx < created_count; idx += 1) {
    thread thd = group->threads[idx];
    if (!thread_is_valid(thd)) {
      continue;
    }

    if (!_thread_join(thd, NULL, site)) {
      thread_log_warn("Forcing thread group worker cleanup idx=%u handle=%p", idx, thd);
      thread_group_force_wait(thd);
    }

    group->threads[idx] = NULL;
  }
}

func b32 thread_group_post_lifecycle(
    msg_core_object_event_kind event_kind,
    thread_group_data* group,
    callsite site) {
  msg_core_object_lifecycle_data msg_data = {
      .event_kind = event_kind,
      .object_type = MSG_CORE_OBJECT_TYPE_THREAD_GROUP,
      .object_ptr = group,
      .site = site,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  return msg_post(&lifecycle_msg);
}

func i32 thread_group_wrapper(void* raw) {
  profile_func_begin;

  thread_group_payload* payload = (thread_group_payload*)raw;
  if (payload == NULL || payload->entry == NULL) {
    thread_log_error("Rejected thread group worker payload=%p", raw);
    profile_func_end;
    return 1;
  }

  thread_group_func entry = payload->entry;
  void* arg = payload->arg;
  u32 idx = payload->idx;

  heap* hp = global_get_perm_heap();
  if (hp == NULL) {
    global_log_error("Failed to acquire global heap");
    profile_func_end;
    return 1;
  }

  heap_dealloc(hp, payload);
  assert(idx < U32_MAX);

  profile_func_end;
  return entry(idx, arg);
}

func thread_group thread_group_create_impl(
    u32 count,
    thread_group_func entry,
    void* arg,
    ctx_setup setup,
    cstr8 base_name,
    callsite site) {
  profile_func_begin;

  if (count == 0 || entry == NULL) {
    thread_log_error("Rejected thread group creation count=%u has_entry=%u", count, (u32)(entry != NULL));
    profile_func_end;
    return NULL;
  }

  heap* hp = thread_get_perm_heap();
  if (hp == NULL) {
    thread_log_error("Thread ctx heap allocator is not available");
    profile_func_end;
    return NULL;
  }

  heap* payload_hp = global_get_perm_heap();
  if (payload_hp == NULL) {
    global_log_error("Failed to acquire global heap");
    profile_func_end;
    return NULL;
  }

  thread_group_data* group = heap_alloc_type(hp, thread_group_data);
  if (group == NULL) {
    thread_log_error("Failed to allocate thread group handle count=%u", count);
    profile_func_end;
    return NULL;
  }

  mem_zero(group, size_of(*group));
  group->threads = heap_alloc_array(hp, thread, count);
  if (group->threads == NULL) {
    thread_log_error("Failed to allocate thread group storage count=%u", count);
    thread_group_destroy_storage(hp, group);
    profile_func_end;
    return NULL;
  }

  group->count = count;
  safe_for (u32 idx = 0; idx < count; idx += 1) {
    thread_group_payload* payload = heap_alloc_type(payload_hp, thread_group_payload);
    if (payload == NULL) {
      thread_log_error("Failed to allocate thread group worker payload idx=%u", idx);
      thread_group_cleanup_failed_create(group, idx, site);
      thread_group_destroy_storage(hp, group);
      profile_func_end;
      return NULL;
    }

    payload->entry = entry;
    payload->arg = arg;
    payload->idx = idx;

    if (base_name != NULL) {
      str8_medium name_buf = {0};
      cstr8_format(name_buf, size_of(name_buf), "%s[%u]", base_name, idx);
      group->threads[idx] = thread_create_named(thread_group_wrapper, payload, name_buf, setup);
    } else {
      group->threads[idx] = thread_create(thread_group_wrapper, payload, setup);
    }

    if (!thread_is_valid(group->threads[idx])) {
      heap_dealloc(payload_hp, payload);
      thread_log_error("Failed to create thread group worker idx=%u base_name=%s",
                       idx,
                       base_name != NULL ? base_name : "<null>");
      thread_group_cleanup_failed_create(group, idx, site);
      thread_group_destroy_storage(hp, group);
      profile_func_end;
      return NULL;
    }
  }

  if (!thread_group_post_lifecycle(MSG_CORE_OBJECT_EVENT_CREATE, group, site)) {
    thread_log_trace("Thread group creation cancelled handle=%p", group);
    safe_for (u32 idx = 0; idx < count; idx += 1) {
      thread thd = group->threads[idx];
      if (!thread_is_valid(thd)) {
        continue;
      }

      if (_thread_detach(thd, site)) {
        group->threads[idx] = NULL;
        continue;
      }

      thread_log_warn("Forcing thread group worker detach idx=%u handle=%p", idx, thd);
      SDL_DetachThread((SDL_Thread*)thd);
      group->threads[idx] = NULL;
    }

    thread_group_destroy_storage(hp, group);
    profile_func_end;
    return NULL;
  }

  thread_log_info("Created thread group handle=%p count=%u base_name=%s",
                  group,
                  count,
                  base_name != NULL ? base_name : "<null>");
  profile_func_end;
  return group;
}

func thread_group _thread_group_create(
    u32 count,
    thread_group_func entry,
    void* arg,
    ctx_setup setup,
    callsite site) {
  profile_func_begin;
  thread_group group = thread_group_create_impl(count, entry, arg, setup, NULL, site);
  profile_func_end;
  return group;
}

func thread_group _thread_group_create_named(
    u32 count,
    thread_group_func entry,
    void* arg,
    ctx_setup setup,
    cstr8 base_name,
    callsite site) {
  profile_func_begin;
  thread_group group = thread_group_create_impl(count, entry, arg, setup, base_name, site);
  profile_func_end;
  return group;
}

func b32 _thread_group_destroy(thread_group group, callsite site) {
  profile_func_begin;

  thread_group_data* data = thread_group_data_from_handle(group);
  if (data == NULL) {
    thread_log_warn("Skipping thread group destroy for invalid handle");
    profile_func_end;
    return false;
  }

  heap* hp = thread_get_perm_heap();
  if (hp == NULL) {
    thread_log_error("Thread ctx heap allocator is not available");
    profile_func_end;
    return false;
  }

  if (!thread_group_post_lifecycle(MSG_CORE_OBJECT_EVENT_DESTROY, data, site)) {
    thread_log_trace("Thread group destruction cancelled handle=%p", group);
    profile_func_end;
    return false;
  }

  safe_for (u32 idx = 0; idx < data->count; idx += 1) {
    thread thd = data->threads[idx];
    if (!thread_is_valid(thd)) {
      continue;
    }

    if (_thread_detach(thd, site)) {
      data->threads[idx] = NULL;
      continue;
    }

    thread_log_warn("Forcing thread group worker detach idx=%u handle=%p", idx, thd);
    SDL_DetachThread((SDL_Thread*)thd);
    data->threads[idx] = NULL;
  }

  thread_log_info("Destroyed thread group handle=%p", group);
  thread_group_destroy_storage(hp, data);
  profile_func_end;
  return true;
}

func b32 thread_group_is_valid(thread_group group) {
  return group != NULL;
}

func u32 thread_group_get_count(thread_group group) {
  thread_group_data* data = thread_group_data_from_handle(group);
  return data != NULL ? data->count : 0;
}

func thread thread_group_get(thread_group group, u32 idx) {
  profile_func_begin;

  thread_group_data* data = thread_group_data_from_handle(group);
  if (data == NULL || idx >= data->count) {
    profile_func_end;
    return NULL;
  }

  profile_func_end;
  return data->threads[idx];
}

func b32 thread_group_join_all(thread_group group, i32* out_exit_codes) {
  profile_func_begin;

  thread_group_data* data = thread_group_data_from_handle(group);
  if (data == NULL) {
    thread_log_error("Rejected thread group join for invalid handle");
    profile_func_end;
    return false;
  }

  b32 success = true;
  safe_for (u32 idx = 0; idx < data->count; idx += 1) {
    thread thd = data->threads[idx];
    if (!thread_is_valid(thd)) {
      if (out_exit_codes != NULL) {
        out_exit_codes[idx] = 0;
      }

      continue;
    }

    i32 exit_code = 0;
    if (!_thread_join(thd, &exit_code, CALLSITE_HERE)) {
      thread_log_error("Failed to join thread group worker idx=%u handle=%p", idx, group);
      success = false;
      if (out_exit_codes != NULL) {
        out_exit_codes[idx] = 0;
      }

      continue;
    }

    if (out_exit_codes != NULL) {
      out_exit_codes[idx] = exit_code;
    }

    data->threads[idx] = NULL;
  }

  thread_log_info("Joined thread group handle=%p count=%u success=%u", group, data->count, (u32)success);
  profile_func_end;
  return success;
}

func b32 thread_group_detach_all(thread_group group) {
  profile_func_begin;

  thread_group_data* data = thread_group_data_from_handle(group);
  if (data == NULL) {
    thread_log_warn("Skipping thread group detach for invalid handle");
    profile_func_end;
    return false;
  }

  b32 success = true;
  safe_for (u32 idx = 0; idx < data->count; idx += 1) {
    thread thd = data->threads[idx];
    if (!thread_is_valid(thd)) {
      continue;
    }

    if (!_thread_detach(thd, CALLSITE_HERE)) {
      thread_log_error("Failed to detach thread group worker idx=%u handle=%p", idx, group);
      success = false;
      continue;
    }

    data->threads[idx] = NULL;
  }

  thread_log_info("Detached thread group handle=%p count=%u success=%u", group, data->count, (u32)success);
  profile_func_end;
  return success;
}
