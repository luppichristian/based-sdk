// MIT License
// Copyright (c) 2026 Christian Luppi

#include "threads/thread_group.h"
#include "basic/assert.h"
#include "context/global_ctx.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

// SDL thread wrapper that bridges thread_func into thread_group_func.
func i32 thread_group_wrapper(void* raw) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  thread_group_slot* slot = (thread_group_slot*)raw;
  if (slot == NULL || slot->entry == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(slot->idx < U32_MAX);
  TracyCZoneEnd(__tracy_zone_ctx);
  return slot->entry(slot->idx, slot->arg);
}

func allocator thread_group_allocator_resolve(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator alloc = thread_get_allocator();
  if (alloc.alloc_fn != NULL && alloc.dealloc_fn != NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return alloc;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return global_get_allocator();
}

// Shared creation path. base_name may be NULL for unnamed threads.
func thread_group create_impl(u32 count, thread_group_func entry, void* arg, cstr8 base_name) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator alloc = thread_group_allocator_resolve();
  thread_group empty = {0};
  if (!count || !entry) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return empty;
  }
  if (alloc.alloc_fn == NULL || alloc.dealloc_fn == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return empty;
  }
  assert(alloc.alloc_fn != NULL);
  assert(alloc.dealloc_fn != NULL);

  allocator main_allocator = {0};
  ctx* context = thread_ctx_get();
  if (context != NULL) {
    main_allocator = context->main_allocator;
  }

  thread_group group = {0};
  group.threads = (thread*)allocator_alloc(&alloc, (sz)count * size_of(thread));
  group.slots = (thread_group_slot*)allocator_alloc(&alloc, (sz)count * size_of(thread_group_slot));

  if (!group.threads || !group.slots) {
    allocator_dealloc(&alloc, group.threads, (sz)count * size_of(thread));
    allocator_dealloc(&alloc, group.slots, (sz)count * size_of(thread_group_slot));
    TracyCZoneEnd(__tracy_zone_ctx);
    return empty;
  }

  for (u32 idx = 0; idx < count; idx += 1) {
    group.slots[idx].entry = entry;
    group.slots[idx].arg = arg;
    group.slots[idx].idx = idx;

    if (base_name != NULL) {
      str8_medium name_buf = {0};
      cstr8_format(name_buf, size_of(name_buf), "%s[%u]", base_name, idx);
      group.threads[idx] = thread_create_named(thread_group_wrapper, &group.slots[idx], name_buf, main_allocator);
    } else {
      group.threads[idx] = thread_create(thread_group_wrapper, &group.slots[idx], main_allocator);
    }

    if (!group.threads[idx]) {
      for (u32 join_idx = 0; join_idx < idx; join_idx += 1) {
        thread_detach(group.threads[join_idx]);
      }
      allocator_dealloc(&alloc, group.threads, (sz)count * size_of(thread));
      allocator_dealloc(&alloc, group.slots, (sz)count * size_of(thread_group_slot));
      TracyCZoneEnd(__tracy_zone_ctx);
      return empty;
    }
  }

  group.count = count;
  thread_log_trace("thread_group_create: count=%u base_name=%s", count, base_name != NULL ? base_name : "<null>");
  TracyCZoneEnd(__tracy_zone_ctx);
  return group;
}

func thread_group _thread_group_create(u32 count, thread_group_func entry, void* arg, callsite site) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  (void)site;
  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_THREAD_GROUP,
                                                     .object_ptr = NULL,
                                                 });
  if (!msg_post(&lifecycle_msg)) {
    thread_group empty = {0};
    TracyCZoneEnd(__tracy_zone_ctx);
    return empty;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return create_impl(count, entry, arg, NULL);
}

func thread_group _thread_group_create_named(
    u32 count,
    thread_group_func entry,
    void* arg,
    cstr8 base_name,
    callsite site) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  (void)site;
  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_THREAD_GROUP,
                                                     .object_ptr = NULL,
                                                 });
  if (!msg_post(&lifecycle_msg)) {
    thread_group empty = {0};
    TracyCZoneEnd(__tracy_zone_ctx);
    return empty;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return create_impl(count, entry, arg, base_name);
}

func void _thread_group_destroy(thread_group* group, callsite site) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator alloc = thread_group_allocator_resolve();
  (void)site;
  if (!group) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  if (alloc.dealloc_fn == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(alloc.dealloc_fn != NULL);

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_THREAD_GROUP,
                                                     .object_ptr = group,
                                                 });
  if (!msg_post(&lifecycle_msg)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  allocator_dealloc(&alloc, group->threads, (sz)group->count * size_of(thread));
  allocator_dealloc(&alloc, group->slots, (sz)group->count * size_of(thread_group_slot));
  group->threads = NULL;
  group->slots = NULL;
  group->count = 0;
  thread_log_trace("thread_group_destroy: group=%p", (void*)group);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func b32 thread_group_is_valid(thread_group* group) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return group && group->threads != NULL;
}

func u32 thread_group_get_count(thread_group* group) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return group ? group->count : 0;
}

func thread thread_group_get(thread_group* group, u32 idx) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!group || idx >= group->count) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return group->threads[idx];
}

func b32 thread_group_join_all(thread_group* group, i32* out_exit_codes) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!group || !group->threads) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(group->count > 0);

  b32 success = 1;
  for (u32 idx = 0; idx < group->count; idx += 1) {
    i32 exit_code = 0;
    if (!thread_join(group->threads[idx], &exit_code)) {
      success = 0;
    }
    if (out_exit_codes) {
      out_exit_codes[idx] = exit_code;
    }
    group->threads[idx] = NULL;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return success;
}

func void thread_group_detach_all(thread_group* group) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!group || !group->threads) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  for (u32 idx = 0; idx < group->count; idx += 1) {
    thread_detach(group->threads[idx]);
    group->threads[idx] = NULL;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}
