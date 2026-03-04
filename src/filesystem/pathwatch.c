// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/pathwatch.h"

#include <efsw/efsw.h>

typedef struct pathwatch_binding {
  void* native_handle;
  pathwatch_event_callback* event_fn;
  pathwatch_missed_callback* missed_fn;
  void* user_data;
} pathwatch_binding;

const_var sz PATHWATCH_BINDING_CAP = 64;

func pathwatch_binding* pathwatch_bindings(void) {
  local_persist pathwatch_binding bindings[PATHWATCH_BINDING_CAP] = {0};
  return bindings;
}

func pathwatch_binding* pathwatch_find_binding(void* native_handle) {
  pathwatch_binding* bindings = pathwatch_bindings();
  sz item_idx = 0;

  for (item_idx = 0; item_idx < PATHWATCH_BINDING_CAP; item_idx += 1) {
    if (bindings[item_idx].native_handle == native_handle) {
      return &bindings[item_idx];
    }
  }

  return NULL;
}

func pathwatch_binding* pathwatch_bind_create(
    void* native_handle,
    pathwatch_event_callback* event_fn,
    pathwatch_missed_callback* missed_fn,
    void* user_data) {
  pathwatch_binding* binding = pathwatch_find_binding(native_handle);
  pathwatch_binding* bindings = pathwatch_bindings();
  sz item_idx = 0;

  if (binding != NULL) {
    binding->event_fn = event_fn;
    binding->missed_fn = missed_fn;
    binding->user_data = user_data;
    return binding;
  }

  for (item_idx = 0; item_idx < PATHWATCH_BINDING_CAP; item_idx += 1) {
    if (bindings[item_idx].native_handle == NULL) {
      bindings[item_idx].native_handle = native_handle;
      bindings[item_idx].event_fn = event_fn;
      bindings[item_idx].missed_fn = missed_fn;
      bindings[item_idx].user_data = user_data;
      return &bindings[item_idx];
    }
  }

  return NULL;
}

func void pathwatch_bind_remove(void* native_handle) {
  pathwatch_binding* binding = pathwatch_find_binding(native_handle);
  if (binding != NULL) {
    binding->native_handle = NULL;
    binding->event_fn = NULL;
    binding->missed_fn = NULL;
    binding->user_data = NULL;
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
    efsw_watchid watch_id,
    const char* dir_ptr,
    const char* file_ptr,
    enum efsw_action action,
    const char* old_file_ptr,
    void* user_data) {
  pathwatch_event event;
  pathwatch_binding* binding = NULL;
  path dir_path;
  path file_path;

  (void)user_data;

  binding = pathwatch_find_binding(native_handle);
  if (binding == NULL || binding->event_fn == NULL) {
    return;
  }

  dir_path = path_from_cstr(dir_ptr != NULL ? dir_ptr : "");
  file_path = path_from_cstr(file_ptr != NULL ? file_ptr : "");

  event.watch_id = (i64)watch_id;
  event.action = pathwatch_map_action(action);
  event.watch_path = dir_path;
  event.item_path = path_join(&dir_path, &file_path);
  event.old_item_path = path_from_cstr("");

  if (old_file_ptr != NULL && old_file_ptr[0] != '\0') {
    path old_file_path = path_from_cstr(old_file_ptr);
    event.old_item_path = path_join(&dir_path, &old_file_path);
  }

  binding->event_fn(&event, binding->user_data);
}

func void pathwatch_dispatch_missed(
    efsw_watcher native_handle,
    efsw_watchid watch_id,
    const char* dir_ptr) {
  pathwatch_binding* binding = pathwatch_find_binding(native_handle);
  path dir_path;

  (void)watch_id;

  if (binding == NULL || binding->missed_fn == NULL) {
    return;
  }

  dir_path = path_from_cstr(dir_ptr != NULL ? dir_ptr : "");
  binding->missed_fn(&dir_path, binding->user_data);
}

func pathwatch pathwatch_create(
    pathwatch_event_callback* event_fn,
    pathwatch_missed_callback* missed_fn,
    void* user_data,
    b32 use_generic_mode) {
  pathwatch watcher;

  watcher.native_handle = efsw_create(use_generic_mode ? 1 : 0);
  watcher.event_fn = event_fn;
  watcher.missed_fn = missed_fn;
  watcher.user_data = user_data;

  if (watcher.native_handle == NULL ||
      pathwatch_bind_create(watcher.native_handle, event_fn, missed_fn, user_data) == NULL) {
    if (watcher.native_handle != NULL) {
      efsw_release((efsw_watcher)watcher.native_handle);
    }
    watcher.event_fn = NULL;
    watcher.missed_fn = NULL;
    watcher.user_data = NULL;
    watcher.native_handle = NULL;
  }

  return watcher;
}

func void pathwatch_destroy(pathwatch* watcher) {
  if (watcher == NULL || watcher->native_handle == NULL) {
    return;
  }

  pathwatch_bind_remove(watcher->native_handle);
  efsw_release((efsw_watcher)watcher->native_handle);
  watcher->native_handle = NULL;
  watcher->event_fn = NULL;
  watcher->missed_fn = NULL;
  watcher->user_data = NULL;
}

func b32 pathwatch_start(pathwatch* watcher) {
  if (watcher == NULL || watcher->native_handle == NULL) {
    return 0;
  }

  efsw_watch((efsw_watcher)watcher->native_handle);
  return 1;
}

func i64 pathwatch_add(pathwatch* watcher, const path* src, b32 recursive) {
  efsw_watchid watch_id = 0;

  if (watcher == NULL || watcher->native_handle == NULL || src == NULL) {
    return 0;
  }

  watch_id = efsw_addwatch_withoptions(
      (efsw_watcher)watcher->native_handle,
      src->buf,
      pathwatch_dispatch,
      recursive ? 1 : 0,
      NULL,
      0,
      watcher,
      pathwatch_dispatch_missed);
  return (i64)watch_id;
}

func b32 pathwatch_remove(pathwatch* watcher, i64 watch_id) {
  if (watcher == NULL || watcher->native_handle == NULL) {
    return 0;
  }

  efsw_removewatch_byid((efsw_watcher)watcher->native_handle, (efsw_watchid)watch_id);
  return 1;
}

func b32 pathwatch_remove_path(pathwatch* watcher, const path* src) {
  if (watcher == NULL || watcher->native_handle == NULL || src == NULL) {
    return 0;
  }

  efsw_removewatch((efsw_watcher)watcher->native_handle, src->buf);
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

func const c8* pathwatch_get_last_error(void) {
  return efsw_getlasterror();
}
