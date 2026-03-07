// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "path.h"

typedef i64 pathwatch_id;
typedef i64 pathwatch_watch_id;

typedef enum pathwatch_action {
  PATHWATCH_ACTION_CREATE = 1,
  PATHWATCH_ACTION_DELETE = 2,
  PATHWATCH_ACTION_MODIFY = 3,
  PATHWATCH_ACTION_MOVE = 4,
} pathwatch_action;

typedef struct pathwatch_event {
  pathwatch_id pathwatch_id;
  pathwatch_watch_id watch_id;
  pathwatch_action action;
  path watch_path;
  path item_path;
  path old_item_path;
} pathwatch_event;

typedef struct pathwatch {
  pathwatch_id id;
  void* native_handle;
} pathwatch;

// Creates a pathwatch wrapper around efsw.
// Changes are emitted through MSG_CORE_TYPE_PATHWATCH messages.
func pathwatch pathwatch_create(b32 use_generic_mode);

// Releases the watcher and every active watch.
func void pathwatch_destroy(pathwatch* watcher);

// Starts the background watch thread. Returns 1 on success, 0 otherwise.
func b32 pathwatch_start(pathwatch* watcher);
// Stops, pauses, resumes, or drains the watcher event stream.
func b32 pathwatch_stop(pathwatch* watcher);
func b32 pathwatch_pause(pathwatch* watcher);
func b32 pathwatch_resume(pathwatch* watcher);
func i32 pathwatch_drain(void);

// Adds a watched directory. Returns a positive watch id on success, or a negative error code.
func pathwatch_watch_id pathwatch_add(pathwatch* watcher, const path* src, b32 recursive);

// Removes the watch identified by watch_id. Returns 1 on success, 0 otherwise.
func b32 pathwatch_remove(pathwatch* watcher, pathwatch_watch_id watch_id);

// Removes the watch rooted at src. Returns 1 on success, 0 otherwise.
func b32 pathwatch_remove_path(pathwatch* watcher, const path* src);

// Returns the watched path associated with watch_id.
func b32 pathwatch_get_path(pathwatch_watch_id watch_id, path* out_watch_path);

// Mirrors efsw follow-symlink behaviour. Returns 1 on success, 0 otherwise.
func b32 pathwatch_follow_symlinks(pathwatch* watcher, b32 enabled);

// Mirrors efsw out-of-scope link behaviour. Returns 1 on success, 0 otherwise.
func b32 pathwatch_allow_out_of_scope_links(pathwatch* watcher, b32 enabled);

// Returns the last efsw error string.
func cstr8 pathwatch_get_last_error(void);
