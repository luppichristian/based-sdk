// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/pathwatch.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "basic/profiler.h"

#include <efsw/efsw.h>
#include "basic/safe.h"

typedef struct pathwatch_binding {
  pathwatch_id pathwatch_id;
  void* native_handle;
  b32 started;
  b32 paused;
} pathwatch_binding;

typedef struct pathwatch_watch_binding {
  pathwatch_watch_id watch_id;
  i64 native_watch_id;
  pathwatch_id pathwatch_id;
  void* native_handle;
  path watch_path;
} pathwatch_watch_binding;

const_var sz PATHWATCH_BINDING_CAP = 64;
const_var sz PATHWATCH_WATCH_BINDING_CAP = 1024;

func pathwatch_binding* pathwatch_bindings(void) {
  profile_func_begin;
  local_persist pathwatch_binding bindings[PATHWATCH_BINDING_CAP] = {0};
  profile_func_end;
  return bindings;
}

func pathwatch_watch_binding* pathwatch_watch_bindings(void) {
  profile_func_begin;
  local_persist pathwatch_watch_binding bindings[PATHWATCH_WATCH_BINDING_CAP] = {0};
  profile_func_end;
  return bindings;
}

func pathwatch_id pathwatch_next_id(void) {
  local_persist pathwatch_id next_id = 1;
  pathwatch_id value = next_id;
  next_id += 1;
  return value;
}

func pathwatch_watch_id pathwatch_next_watch_id(void) {
  local_persist pathwatch_watch_id next_id = 1;
  pathwatch_watch_id value = next_id;
  next_id += 1;
  return value;
}

func pathwatch_binding* pathwatch_find_binding(void* native_handle) {
  profile_func_begin;
  pathwatch_binding* bindings = pathwatch_bindings();

  safe_for (sz item_idx = 0; item_idx < PATHWATCH_BINDING_CAP; item_idx += 1) {
    if (bindings[item_idx].native_handle == native_handle) {
      profile_func_end;
      return &bindings[item_idx];
    }
  }
  profile_func_end;
  return NULL;
}

func b32 pathwatch_bind_create(pathwatch_id pathwatch_id, void* native_handle) {
  profile_func_begin;
  pathwatch_binding* bindings = pathwatch_bindings();

  safe_for (sz item_idx = 0; item_idx < PATHWATCH_BINDING_CAP; item_idx += 1) {
    if (bindings[item_idx].native_handle == NULL) {
      bindings[item_idx].native_handle = native_handle;
      bindings[item_idx].pathwatch_id = pathwatch_id;
      bindings[item_idx].started = 0;
      bindings[item_idx].paused = 0;
      profile_func_end;
      return true;
    }
  }
  profile_func_end;
  return false;
}

func void pathwatch_bind_remove(void* native_handle) {
  profile_func_begin;
  pathwatch_binding* binding = pathwatch_find_binding(native_handle);
  if (binding != NULL) {
    binding->native_handle = NULL;
    binding->pathwatch_id = 0;
    binding->started = 0;
    binding->paused = 0;
  }
  profile_func_end;
}

func pathwatch_watch_binding* pathwatch_find_watch_binding_by_public_id(pathwatch_watch_id watch_id) {
  profile_func_begin;
  pathwatch_watch_binding* bindings = pathwatch_watch_bindings();

  safe_for (sz item_idx = 0; item_idx < PATHWATCH_WATCH_BINDING_CAP; item_idx += 1) {
    if (bindings[item_idx].watch_id == watch_id) {
      profile_func_end;
      return &bindings[item_idx];
    }
  }
  profile_func_end;
  return NULL;
}

func pathwatch_watch_binding* pathwatch_find_watch_binding_by_native(
    void* native_handle,
    i64 native_watch_id) {
  profile_func_begin;
  pathwatch_watch_binding* bindings = pathwatch_watch_bindings();

  safe_for (sz item_idx = 0; item_idx < PATHWATCH_WATCH_BINDING_CAP; item_idx += 1) {
    if (bindings[item_idx].native_handle == native_handle &&
        bindings[item_idx].native_watch_id == native_watch_id) {
      profile_func_end;
      return &bindings[item_idx];
    }
  }
  profile_func_end;
  return NULL;
}

func pathwatch_watch_id pathwatch_watch_bind_create(
    pathwatch_id pathwatch_id,
    void* native_handle,
    i64 native_watch_id,
    const path* watch_path) {
  profile_func_begin;
  pathwatch_watch_binding* bindings = pathwatch_watch_bindings();

  safe_for (sz item_idx = 0; item_idx < PATHWATCH_WATCH_BINDING_CAP; item_idx += 1) {
    if (bindings[item_idx].watch_id == 0) {
      pathwatch_watch_id watch_id = pathwatch_next_watch_id();
      bindings[item_idx].watch_id = watch_id;
      bindings[item_idx].native_watch_id = native_watch_id;
      bindings[item_idx].pathwatch_id = pathwatch_id;
      bindings[item_idx].native_handle = native_handle;
      bindings[item_idx].watch_path = watch_path != NULL ? *watch_path : path_from_cstr("");
      profile_func_end;
      return watch_id;
    }
  }
  profile_func_end;
  return 0;
}

func void pathwatch_watch_bind_remove(pathwatch_watch_id watch_id) {
  profile_func_begin;
  pathwatch_watch_binding* binding = pathwatch_find_watch_binding_by_public_id(watch_id);
  if (binding != NULL) {
    *binding = (pathwatch_watch_binding) {0};
  }
  profile_func_end;
}

func void pathwatch_watch_bind_remove_by_native(void* native_handle, i64 native_watch_id) {
  profile_func_begin;
  pathwatch_watch_binding* binding =
      pathwatch_find_watch_binding_by_native(native_handle, native_watch_id);
  if (binding != NULL) {
    *binding = (pathwatch_watch_binding) {0};
  }
  profile_func_end;
}

func void pathwatch_watch_bind_remove_all_for_watcher(pathwatch_id pathwatch_id, void* native_handle) {
  profile_func_begin;
  pathwatch_watch_binding* bindings = pathwatch_watch_bindings();

  safe_for (sz item_idx = 0; item_idx < PATHWATCH_WATCH_BINDING_CAP; item_idx += 1) {
    if (bindings[item_idx].watch_id != 0 && bindings[item_idx].pathwatch_id == pathwatch_id &&
        bindings[item_idx].native_handle == native_handle) {
      bindings[item_idx] = (pathwatch_watch_binding) {0};
    }
  }
  profile_func_end;
}

func void pathwatch_watch_bind_remove_by_path(pathwatch_id pathwatch_id, void* native_handle, const path* src) {
  profile_func_begin;
  pathwatch_watch_binding* bindings = pathwatch_watch_bindings();
  path normd_src = path_norm_trimmed_cpy(src);

  safe_for (sz item_idx = 0; item_idx < PATHWATCH_WATCH_BINDING_CAP; item_idx += 1) {
    if (bindings[item_idx].watch_id != 0 && bindings[item_idx].pathwatch_id == pathwatch_id &&
        bindings[item_idx].native_handle == native_handle) {
      path normd_watch = path_norm_trimmed_cpy(&bindings[item_idx].watch_path);
      if (cstr8_cmp(normd_watch.buf, normd_src.buf)) {
        bindings[item_idx] = (pathwatch_watch_binding) {0};
      }
    }
  }
  profile_func_end;
}

func pathwatch_action pathwatch_map_action(enum efsw_action action) {
  profile_func_begin;
  switch (action) {
    case EFSW_ADD:
      profile_func_end;
      return PATHWATCH_ACTION_CREATE;
    case EFSW_DELETE:
      profile_func_end;
      return PATHWATCH_ACTION_DELETE;
    case EFSW_MODIFIED:
      profile_func_end;
      return PATHWATCH_ACTION_MODIFY;
    case EFSW_MOVED:
      profile_func_end;
      return PATHWATCH_ACTION_MOVE;
    default:
      profile_func_end;
      return PATHWATCH_ACTION_MODIFY;
  }
}

func void pathwatch_dispatch(
    efsw_watcher native_handle,
    efsw_watchid native_watch_id,
    cstr8 dir_ptr,
    cstr8 file_ptr,
    enum efsw_action action,
    cstr8 old_file_ptr,
    void* user_data) {
  profile_func_begin;
  (void)dir_ptr;
  (void)file_ptr;
  (void)old_file_ptr;
  (void)user_data;

  pathwatch_binding* binding = pathwatch_find_binding(native_handle);
  if (binding == NULL) {
    thread_log_warn("Dropping pathwatch event for unknown watcher native=%p", native_handle);
    profile_func_end;
    return;
  }
  if (binding->paused || !binding->started) {
    thread_log_verbose("Dropping pathwatch event watcher=%lld started=%u paused=%u",
                       (long long)binding->pathwatch_id,
                       (u32)binding->started,
                       (u32)binding->paused);
    profile_func_end;
    return;
  }

  pathwatch_watch_binding* watch_binding =
      pathwatch_find_watch_binding_by_native(native_handle, (i64)native_watch_id);
  if (watch_binding == NULL) {
    thread_log_warn("Dropping pathwatch event for unknown watch native=%lld", (long long)native_watch_id);
    profile_func_end;
    return;
  }

  msg event_msg = {0};
  event_msg.type = MSG_CORE_TYPE_PATHWATCH;
  msg_core_pathwatch_data pathwatch_data = {
      .event_kind = MSG_CORE_PATHWATCH_EVENT_ITEM,
      .pathwatch_id = binding->pathwatch_id,
      .watch_id = watch_binding->watch_id,
      .action = pathwatch_map_action(action),
  };
  msg_core_fill_pathwatch(&event_msg, &pathwatch_data);
  if (!msg_post(&event_msg)) {
    thread_log_error("Failed to post pathwatch item event watcher=%lld watch=%lld",
                     (long long)binding->pathwatch_id,
                     (long long)watch_binding->watch_id);
  }
  profile_func_end;
}

func void pathwatch_dispatch_missed(
    efsw_watcher native_handle,
    efsw_watchid native_watch_id,
    cstr8 dir_ptr) {
  profile_func_begin;
  (void)dir_ptr;

  pathwatch_binding* binding = pathwatch_find_binding(native_handle);
  if (binding == NULL) {
    thread_log_warn("Dropping missed pathwatch event for unknown watcher native=%p", native_handle);
    profile_func_end;
    return;
  }
  if (binding->paused || !binding->started) {
    thread_log_verbose("Dropping missed pathwatch event watcher=%lld started=%u paused=%u",
                       (long long)binding->pathwatch_id,
                       (u32)binding->started,
                       (u32)binding->paused);
    profile_func_end;
    return;
  }

  pathwatch_watch_binding* watch_binding =
      pathwatch_find_watch_binding_by_native(native_handle, (i64)native_watch_id);

  msg event_msg = {0};
  event_msg.type = MSG_CORE_TYPE_PATHWATCH;
  msg_core_pathwatch_data pathwatch_data = {
      .event_kind = MSG_CORE_PATHWATCH_EVENT_MISSED,
      .pathwatch_id = binding->pathwatch_id,
      .watch_id = watch_binding != NULL ? watch_binding->watch_id : 0,
      .action = (pathwatch_action)0,
  };
  msg_core_fill_pathwatch(&event_msg, &pathwatch_data);
  if (!msg_post(&event_msg)) {
    thread_log_error("Failed to post missed pathwatch event watcher=%lld watch=%lld",
                     (long long)binding->pathwatch_id,
                     (long long)pathwatch_data.watch_id);
  }
  profile_func_end;
}

func pathwatch _pathwatch_create(b32 use_generic_mode, callsite site) {
  profile_func_begin;
  pathwatch watcher = {0};
  watcher.id = pathwatch_next_id();
  watcher.native_handle = efsw_create(use_generic_mode ? true : false);

  if (watcher.native_handle == NULL || !pathwatch_bind_create(watcher.id, watcher.native_handle)) {
    if (watcher.native_handle != NULL) {
      efsw_release((efsw_watcher)watcher.native_handle);
    }
    watcher.id = 0;
    watcher.native_handle = NULL;
    thread_log_error("Failed to create pathwatch watcher");
    profile_func_end;
    return watcher;
  }

  msg lifecycle_msg = {0};
  msg_core_object_lifecycle_data msg_data = {
      .object_type = MSG_CORE_OBJECT_TYPE_PATHWATCH,
      .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
      .object_ptr = watcher.native_handle,
      .site = site,
  };
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("Pathwatch creation was suspended watcher=%lld", (long long)watcher.id);
    pathwatch_bind_remove(watcher.native_handle);
    efsw_release((efsw_watcher)watcher.native_handle);
    watcher.id = 0;
    watcher.native_handle = NULL;
  }
  if (watcher.native_handle != NULL) {
    thread_log_trace("Created pathwatch watcher=%lld", (long long)watcher.id);
  }

  profile_func_end;
  return watcher;
}

func void _pathwatch_destroy(pathwatch* watcher, callsite site) {
  profile_func_begin;
  if (watcher == NULL || watcher->native_handle == NULL) {
    profile_func_end;
    return;
  }
  assert(watcher != NULL);

  msg lifecycle_msg = {0};
  msg_core_object_lifecycle_data msg_data = {
      .object_type = MSG_CORE_OBJECT_TYPE_PATHWATCH,
      .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
      .object_ptr = watcher->native_handle,
      .site = site,
  };
  msg_core_fill_object_lifecycle(&lifecycle_msg, &msg_data);
  if (!msg_post(&lifecycle_msg)) {
    thread_log_trace("Pathwatch destruction was suspended watcher=%lld", (long long)watcher->id);
    profile_func_end;
    return;
  }

  pathwatch_watch_bind_remove_all_for_watcher(watcher->id, watcher->native_handle);
  pathwatch_bind_remove(watcher->native_handle);
  efsw_release((efsw_watcher)watcher->native_handle);
  thread_log_trace("Destroyed pathwatch watcher=%lld", (long long)watcher->id);
  watcher->id = 0;
  watcher->native_handle = NULL;
  profile_func_end;
}

func b32 pathwatch_start(pathwatch* watcher) {
  profile_func_begin;
  if (watcher == NULL || watcher->native_handle == NULL) {
    thread_log_error("Rejected pathwatch start for invalid watcher");
    profile_func_end;
    return false;
  }
  assert(watcher->id > 0);

  efsw_watch((efsw_watcher)watcher->native_handle);
  pathwatch_binding* binding = pathwatch_find_binding(watcher->native_handle);
  if (binding != NULL) {
    binding->started = 1;
    binding->paused = 0;
  }
  thread_log_info("Started pathwatch watcher=%lld", (long long)watcher->id);
  profile_func_end;
  return true;
}

func b32 pathwatch_stop(pathwatch* watcher) {
  profile_func_begin;
  if (watcher == NULL || watcher->native_handle == NULL) {
    thread_log_error("Rejected pathwatch stop for invalid watcher");
    profile_func_end;
    return false;
  }

  pathwatch_watch_bind_remove_all_for_watcher(watcher->id, watcher->native_handle);
  pathwatch_binding* binding = pathwatch_find_binding(watcher->native_handle);
  if (binding != NULL) {
    binding->started = 0;
    binding->paused = 0;
  }
  thread_log_info("Stopped pathwatch watcher=%lld", (long long)watcher->id);
  profile_func_end;
  return true;
}

func b32 pathwatch_pause(pathwatch* watcher) {
  profile_func_begin;
  if (watcher == NULL || watcher->native_handle == NULL) {
    thread_log_error("Rejected pathwatch pause for invalid watcher");
    profile_func_end;
    return false;
  }

  pathwatch_binding* binding = pathwatch_find_binding(watcher->native_handle);
  if (binding == NULL) {
    thread_log_warn("Cannot pause unbound pathwatch watcher=%lld", (long long)watcher->id);
    profile_func_end;
    return false;
  }

  binding->paused = 1;
  thread_log_info("Paused pathwatch watcher=%lld", (long long)watcher->id);
  profile_func_end;
  return true;
}

func b32 pathwatch_resume(pathwatch* watcher) {
  profile_func_begin;
  if (watcher == NULL || watcher->native_handle == NULL) {
    thread_log_error("Rejected pathwatch resume for invalid watcher");
    profile_func_end;
    return false;
  }

  pathwatch_binding* binding = pathwatch_find_binding(watcher->native_handle);
  if (binding == NULL) {
    thread_log_warn("Cannot resume unbound pathwatch watcher=%lld", (long long)watcher->id);
    profile_func_end;
    return false;
  }

  binding->paused = 0;
  thread_log_info("Resumed pathwatch watcher=%lld", (long long)watcher->id);
  profile_func_end;
  return true;
}

func i32 pathwatch_drain(void) {
  profile_func_begin;
  i32 count = msg_count(MSG_CORE_TYPE_PATHWATCH, MSG_CORE_TYPE_PATHWATCH);
  msg_flush(MSG_CORE_TYPE_PATHWATCH, MSG_CORE_TYPE_PATHWATCH);
  profile_func_end;
  return count;
}

func pathwatch_watch_id pathwatch_add(pathwatch* watcher, const path* src, b32 recursive) {
  profile_func_begin;
  if (watcher == NULL || watcher->native_handle == NULL || src == NULL) {
    thread_log_error("Rejected pathwatch add watcher=%p path=%s",
                     (void*)watcher,
                     src != NULL ? src->buf : "<null>");
    profile_func_end;
    return false;
  }
  assert(src->buf[0] != '\0');

  efsw_watchid native_watch_id = efsw_addwatch_withoptions(
      (efsw_watcher)watcher->native_handle,
      src->buf,
      pathwatch_dispatch,
      recursive ? true : false,
      NULL,
      0,
      watcher,
      pathwatch_dispatch_missed);
  if (native_watch_id <= 0) {
    thread_log_error("Failed to add pathwatch path=%s watcher=%lld", src->buf, (long long)watcher->id);
    profile_func_end;
    return false;
  }

  pathwatch_watch_id watch_id = pathwatch_watch_bind_create(
      watcher->id,
      watcher->native_handle,
      (i64)native_watch_id,
      src);
  if (watch_id > 0) {
    thread_log_trace("Added pathwatch watcher=%lld watch=%lld path=%s", (long long)watcher->id, (long long)watch_id, src->buf);
  } else {
    thread_log_error("Failed to bind added pathwatch watcher=%lld path=%s", (long long)watcher->id, src->buf);
  }
  profile_func_end;
  return watch_id;
}

func b32 pathwatch_remove(pathwatch* watcher, pathwatch_watch_id watch_id) {
  profile_func_begin;
  if (watcher == NULL || watcher->native_handle == NULL || watch_id <= 0) {
    thread_log_error("Rejected pathwatch remove watcher=%p watch=%lld", (void*)watcher, (long long)watch_id);
    profile_func_end;
    return false;
  }

  pathwatch_watch_binding* binding = pathwatch_find_watch_binding_by_public_id(watch_id);
  if (binding == NULL || binding->pathwatch_id != watcher->id ||
      binding->native_handle != watcher->native_handle) {
    thread_log_warn("Cannot remove unknown pathwatch watcher=%lld watch=%lld", (long long)watcher->id, (long long)watch_id);
    profile_func_end;
    return false;
  }

  efsw_removewatch_byid((efsw_watcher)watcher->native_handle, (efsw_watchid)binding->native_watch_id);
  pathwatch_watch_bind_remove(watch_id);
  thread_log_trace("Removed pathwatch watcher=%lld watch=%lld", (long long)watcher->id, (long long)watch_id);
  profile_func_end;
  return true;
}

func b32 pathwatch_remove_path(pathwatch* watcher, const path* src) {
  profile_func_begin;
  if (watcher == NULL || watcher->native_handle == NULL || src == NULL) {
    thread_log_error("Rejected pathwatch remove-by-path watcher=%p path=%s",
                     (void*)watcher,
                     src != NULL ? src->buf : "<null>");
    profile_func_end;
    return false;
  }

  efsw_removewatch((efsw_watcher)watcher->native_handle, src->buf);
  pathwatch_watch_bind_remove_by_path(watcher->id, watcher->native_handle, src);
  thread_log_trace("Removed pathwatch by path watcher=%lld path=%s", (long long)watcher->id, src->buf);
  profile_func_end;
  return true;
}

func b32 pathwatch_get_path(pathwatch_watch_id watch_id, path* out_watch_path) {
  profile_func_begin;
  if (watch_id <= 0 || out_watch_path == NULL) {
    profile_func_end;
    return false;
  }

  pathwatch_watch_binding* binding = pathwatch_find_watch_binding_by_public_id(watch_id);
  if (binding == NULL) {
    profile_func_end;
    return false;
  }

  *out_watch_path = binding->watch_path;
  profile_func_end;
  return true;
}

func b32 pathwatch_follow_symlinks(pathwatch* watcher, b32 enabled) {
  profile_func_begin;
  if (watcher == NULL || watcher->native_handle == NULL) {
    profile_func_end;
    return false;
  }

  efsw_follow_symlinks((efsw_watcher)watcher->native_handle, enabled ? true : false);
  profile_func_end;
  return true;
}

func b32 pathwatch_allow_out_of_scope_links(pathwatch* watcher, b32 enabled) {
  profile_func_begin;
  if (watcher == NULL || watcher->native_handle == NULL) {
    profile_func_end;
    return false;
  }

  efsw_allow_outofscopelinks((efsw_watcher)watcher->native_handle, enabled ? true : false);
  profile_func_end;
  return true;
}

func cstr8 pathwatch_get_last_error(void) {
  return efsw_getlasterror();
}
