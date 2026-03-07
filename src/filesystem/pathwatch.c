// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/pathwatch.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"

#include <efsw/efsw.h>

typedef struct pathwatch_binding {
  pathwatch_id pathwatch_id;
  void* native_handle;
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
  local_persist pathwatch_binding bindings[PATHWATCH_BINDING_CAP] = {0};
  return bindings;
}

func pathwatch_watch_binding* pathwatch_watch_bindings(void) {
  local_persist pathwatch_watch_binding bindings[PATHWATCH_WATCH_BINDING_CAP] = {0};
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
  pathwatch_binding* bindings = pathwatch_bindings();

  for (sz item_idx = 0; item_idx < PATHWATCH_BINDING_CAP; item_idx += 1) {
    if (bindings[item_idx].native_handle == native_handle) {
      return &bindings[item_idx];
    }
  }
  return NULL;
}

func b32 pathwatch_bind_create(pathwatch_id pathwatch_id, void* native_handle) {
  pathwatch_binding* bindings = pathwatch_bindings();

  for (sz item_idx = 0; item_idx < PATHWATCH_BINDING_CAP; item_idx += 1) {
    if (bindings[item_idx].native_handle == NULL) {
      bindings[item_idx].native_handle = native_handle;
      bindings[item_idx].pathwatch_id = pathwatch_id;
      return 1;
    }
  }
  return 0;
}

func void pathwatch_bind_remove(void* native_handle) {
  pathwatch_binding* binding = pathwatch_find_binding(native_handle);
  if (binding != NULL) {
    binding->native_handle = NULL;
    binding->pathwatch_id = 0;
  }
}

func pathwatch_watch_binding* pathwatch_find_watch_binding_by_public_id(pathwatch_watch_id watch_id) {
  pathwatch_watch_binding* bindings = pathwatch_watch_bindings();

  for (sz item_idx = 0; item_idx < PATHWATCH_WATCH_BINDING_CAP; item_idx += 1) {
    if (bindings[item_idx].watch_id == watch_id) {
      return &bindings[item_idx];
    }
  }
  return NULL;
}

func pathwatch_watch_binding* pathwatch_find_watch_binding_by_native(
    void* native_handle,
    i64 native_watch_id) {
  pathwatch_watch_binding* bindings = pathwatch_watch_bindings();

  for (sz item_idx = 0; item_idx < PATHWATCH_WATCH_BINDING_CAP; item_idx += 1) {
    if (bindings[item_idx].native_handle == native_handle &&
        bindings[item_idx].native_watch_id == native_watch_id) {
      return &bindings[item_idx];
    }
  }
  return NULL;
}

func pathwatch_watch_id pathwatch_watch_bind_create(
    pathwatch_id pathwatch_id,
    void* native_handle,
    i64 native_watch_id,
    const path* watch_path) {
  pathwatch_watch_binding* bindings = pathwatch_watch_bindings();

  for (sz item_idx = 0; item_idx < PATHWATCH_WATCH_BINDING_CAP; item_idx += 1) {
    if (bindings[item_idx].watch_id == 0) {
      pathwatch_watch_id watch_id = pathwatch_next_watch_id();
      bindings[item_idx].watch_id = watch_id;
      bindings[item_idx].native_watch_id = native_watch_id;
      bindings[item_idx].pathwatch_id = pathwatch_id;
      bindings[item_idx].native_handle = native_handle;
      bindings[item_idx].watch_path = watch_path != NULL ? *watch_path : path_from_cstr("");
      return watch_id;
    }
  }
  return 0;
}

func void pathwatch_watch_bind_remove(pathwatch_watch_id watch_id) {
  pathwatch_watch_binding* binding = pathwatch_find_watch_binding_by_public_id(watch_id);
  if (binding != NULL) {
    *binding = (pathwatch_watch_binding) {0};
  }
}

func void pathwatch_watch_bind_remove_by_native(void* native_handle, i64 native_watch_id) {
  pathwatch_watch_binding* binding =
      pathwatch_find_watch_binding_by_native(native_handle, native_watch_id);
  if (binding != NULL) {
    *binding = (pathwatch_watch_binding) {0};
  }
}

func void pathwatch_watch_bind_remove_all_for_watcher(pathwatch_id pathwatch_id, void* native_handle) {
  pathwatch_watch_binding* bindings = pathwatch_watch_bindings();

  for (sz item_idx = 0; item_idx < PATHWATCH_WATCH_BINDING_CAP; item_idx += 1) {
    if (bindings[item_idx].watch_id != 0 && bindings[item_idx].pathwatch_id == pathwatch_id &&
        bindings[item_idx].native_handle == native_handle) {
      bindings[item_idx] = (pathwatch_watch_binding) {0};
    }
  }
}

func void pathwatch_watch_bind_remove_by_path(pathwatch_id pathwatch_id, void* native_handle, const path* src) {
  pathwatch_watch_binding* bindings = pathwatch_watch_bindings();
  path normalized_src = src != NULL ? *src : path_from_cstr("");
  path_normalize(&normalized_src);
  path_remove_trailing_slash(&normalized_src);

  for (sz item_idx = 0; item_idx < PATHWATCH_WATCH_BINDING_CAP; item_idx += 1) {
    if (bindings[item_idx].watch_id != 0 && bindings[item_idx].pathwatch_id == pathwatch_id &&
        bindings[item_idx].native_handle == native_handle) {
      path normalized_watch = bindings[item_idx].watch_path;
      path_normalize(&normalized_watch);
      path_remove_trailing_slash(&normalized_watch);
      if (cstr8_cmp(normalized_watch.buf, normalized_src.buf) == 0) {
        bindings[item_idx] = (pathwatch_watch_binding) {0};
      }
    }
  }
}

func pathwatch_action pathwatch_map_action(enum efsw_action action) {
  switch (action) {
    case EFSW_ADD:
      return PATHWATCH_ACTION_CREATE;
    case EFSW_DELETE:
      return PATHWATCH_ACTION_DELETE;
    case EFSW_MODIFIED:
      return PATHWATCH_ACTION_MODIFY;
    case EFSW_MOVED:
      return PATHWATCH_ACTION_MOVE;
    default:
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
  (void)dir_ptr;
  (void)file_ptr;
  (void)old_file_ptr;
  (void)user_data;

  pathwatch_binding* binding = pathwatch_find_binding(native_handle);
  if (binding == NULL) {
    return;
  }

  pathwatch_watch_binding* watch_binding =
      pathwatch_find_watch_binding_by_native(native_handle, (i64)native_watch_id);
  if (watch_binding == NULL) {
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
  (void)msg_post(&event_msg);
}

func void pathwatch_dispatch_missed(
    efsw_watcher native_handle,
    efsw_watchid native_watch_id,
    cstr8 dir_ptr) {
  (void)dir_ptr;

  pathwatch_binding* binding = pathwatch_find_binding(native_handle);
  if (binding == NULL) {
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
  (void)msg_post(&event_msg);
}

func pathwatch pathwatch_create(b32 use_generic_mode) {
  pathwatch watcher = {0};
  watcher.id = pathwatch_next_id();
  watcher.native_handle = efsw_create(use_generic_mode ? 1 : 0);

  if (watcher.native_handle == NULL || !pathwatch_bind_create(watcher.id, watcher.native_handle)) {
    if (watcher.native_handle != NULL) {
      efsw_release((efsw_watcher)watcher.native_handle);
    }
    watcher.id = 0;
    watcher.native_handle = NULL;
    thread_log_error("pathwatch_create: failed");
    return watcher;
  }

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_object_lifecycle_data lifecycle_data = {
      .object_type = MSG_CORE_OBJECT_TYPE_PATHWATCH,
      .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
      .object_ptr = watcher.native_handle,
  };
  msg_core_fill_object_lifecycle(&lifecycle_msg, &lifecycle_data);
  if (!msg_post(&lifecycle_msg)) {
    pathwatch_bind_remove(watcher.native_handle);
    efsw_release((efsw_watcher)watcher.native_handle);
    watcher.id = 0;
    watcher.native_handle = NULL;
  }
  if (watcher.native_handle != NULL) {
    thread_log_trace("pathwatch_create: id=%lld", (long long)watcher.id);
  }

  return watcher;
}

func void pathwatch_destroy(pathwatch* watcher) {
  if (watcher == NULL || watcher->native_handle == NULL) {
    return;
  }
  assert(watcher != NULL);

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_object_lifecycle_data lifecycle_data = {
      .object_type = MSG_CORE_OBJECT_TYPE_PATHWATCH,
      .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
      .object_ptr = watcher->native_handle,
  };
  msg_core_fill_object_lifecycle(&lifecycle_msg, &lifecycle_data);
  if (!msg_post(&lifecycle_msg)) {
    return;
  }

  pathwatch_watch_bind_remove_all_for_watcher(watcher->id, watcher->native_handle);
  pathwatch_bind_remove(watcher->native_handle);
  efsw_release((efsw_watcher)watcher->native_handle);
  thread_log_trace("pathwatch_destroy: id=%lld", (long long)watcher->id);
  watcher->id = 0;
  watcher->native_handle = NULL;
}

func b32 pathwatch_start(pathwatch* watcher) {
  if (watcher == NULL || watcher->native_handle == NULL) {
    return 0;
  }
  assert(watcher->id > 0);

  efsw_watch((efsw_watcher)watcher->native_handle);
  return 1;
}

func pathwatch_watch_id pathwatch_add(pathwatch* watcher, const path* src, b32 recursive) {
  if (watcher == NULL || watcher->native_handle == NULL || src == NULL) {
    return 0;
  }
  assert(src->buf[0] != '\0');

  efsw_watchid native_watch_id = efsw_addwatch_withoptions(
      (efsw_watcher)watcher->native_handle,
      src->buf,
      pathwatch_dispatch,
      recursive ? 1 : 0,
      NULL,
      0,
      watcher,
      pathwatch_dispatch_missed);
  if (native_watch_id <= 0) {
    return 0;
  }

  pathwatch_watch_id watch_id = pathwatch_watch_bind_create(
      watcher->id,
      watcher->native_handle,
      (i64)native_watch_id,
      src);
  if (watch_id > 0) {
    thread_log_trace("pathwatch_add: watcher=%lld watch=%lld path=%s", (long long)watcher->id, (long long)watch_id, src->buf);
  }
  return watch_id;
}

func b32 pathwatch_remove(pathwatch* watcher, pathwatch_watch_id watch_id) {
  if (watcher == NULL || watcher->native_handle == NULL || watch_id <= 0) {
    return 0;
  }

  pathwatch_watch_binding* binding = pathwatch_find_watch_binding_by_public_id(watch_id);
  if (binding == NULL || binding->pathwatch_id != watcher->id ||
      binding->native_handle != watcher->native_handle) {
    return 0;
  }

  efsw_removewatch_byid((efsw_watcher)watcher->native_handle, (efsw_watchid)binding->native_watch_id);
  pathwatch_watch_bind_remove(watch_id);
  thread_log_trace("pathwatch_remove: watcher=%lld watch=%lld", (long long)watcher->id, (long long)watch_id);
  return 1;
}

func b32 pathwatch_remove_path(pathwatch* watcher, const path* src) {
  if (watcher == NULL || watcher->native_handle == NULL || src == NULL) {
    return 0;
  }

  efsw_removewatch((efsw_watcher)watcher->native_handle, src->buf);
  pathwatch_watch_bind_remove_by_path(watcher->id, watcher->native_handle, src);
  return 1;
}

func b32 pathwatch_get_path(pathwatch_watch_id watch_id, path* out_watch_path) {
  if (watch_id <= 0 || out_watch_path == NULL) {
    return 0;
  }

  pathwatch_watch_binding* binding = pathwatch_find_watch_binding_by_public_id(watch_id);
  if (binding == NULL) {
    return 0;
  }

  *out_watch_path = binding->watch_path;
  return 1;
}

func b32 pathwatch_follow_symlinks(pathwatch* watcher, b32 enabled) {
  if (watcher == NULL || watcher->native_handle == NULL) {
    return 0;
  }

  efsw_follow_symlinks((efsw_watcher)watcher->native_handle, enabled ? 1 : 0);
  return 1;
}

func b32 pathwatch_allow_out_of_scope_links(pathwatch* watcher, b32 enabled) {
  if (watcher == NULL || watcher->native_handle == NULL) {
    return 0;
  }

  efsw_allow_outofscopelinks((efsw_watcher)watcher->native_handle, enabled ? 1 : 0);
  return 1;
}

func cstr8 pathwatch_get_last_error(void) {
  return efsw_getlasterror();
}

