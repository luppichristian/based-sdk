// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "path.h"

typedef enum pathwatch_action {
  PATHWATCH_ACTION_CREATE = 1,
  PATHWATCH_ACTION_DELETE = 2,
  PATHWATCH_ACTION_MODIFY = 3,
  PATHWATCH_ACTION_MOVE = 4,
} pathwatch_action;

typedef struct pathwatch_event {
  i64 watch_id;
  pathwatch_action action;
  path watch_path;
  path item_path;
  path old_item_path;
} pathwatch_event;

typedef void pathwatch_event_callback(const pathwatch_event* event, void* user_data);
typedef void pathwatch_missed_callback(const path* watch_path, void* user_data);

typedef struct pathwatch {
  void* native_handle;
  pathwatch_event_callback* event_fn;
  pathwatch_missed_callback* missed_fn;
  void* user_data;
} pathwatch;

// Creates a pathwatch wrapper around efsw.
func pathwatch pathwatch_create(
    pathwatch_event_callback* event_fn,
    pathwatch_missed_callback* missed_fn,
    void* user_data,
    b32 use_generic_mode);

// Releases the watcher and every active watch.
func void pathwatch_destroy(pathwatch* watcher);

// Starts the background watch thread. Returns 1 on success, 0 otherwise.
func b32 pathwatch_start(pathwatch* watcher);

// Adds a watched directory. Returns a positive watch id on success, or a negative error code.
func i64 pathwatch_add(pathwatch* watcher, const path* src, b32 recursive);

// Removes the watch identified by watch_id. Returns 1 on success, 0 otherwise.
func b32 pathwatch_remove(pathwatch* watcher, i64 watch_id);

// Removes the watch rooted at src. Returns 1 on success, 0 otherwise.
func b32 pathwatch_remove_path(pathwatch* watcher, const path* src);

// Mirrors efsw follow-symlink behaviour. Returns 1 on success, 0 otherwise.
func b32 pathwatch_follow_symlinks(pathwatch* watcher, b32 enabled);

// Mirrors efsw out-of-scope link behaviour. Returns 1 on success, 0 otherwise.
func b32 pathwatch_allow_out_of_scope_links(pathwatch* watcher, b32 enabled);

// Returns the last efsw error string.
func const c8* pathwatch_get_last_error(void);
