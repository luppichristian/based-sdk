// MIT License
// Copyright (c) 2026 Christian Luppi

#define ENTRY_TYPE_APP
#include <based_core.h>

typedef struct app_entry_state {
  window window_id;
  u64 close_handler_id;
  b32 should_quit;
} app_entry_state;

func b32 app_window_close_handler(msg* src, void* user_data) {
  if (src == NULL || user_data == NULL) {
    return 1;
  }

  app_entry_state* state_ptr = (app_entry_state*)user_data;
  msg_core_window_data* win_data = msg_core_get_window(src);
  if (win_data == NULL || win_data->window != state_ptr->window_id) {
    return 1;
  }

  thread_log_info("Standalone app window close requested");
  state_ptr->should_quit = 1;
  if (window_is_valid(state_ptr->window_id)) {
    if (!window_destroy(state_ptr->window_id)) {
      thread_log_warn("Failed destroying standalone app window after close request");
    }
  }
  state_ptr->window_id = NULL;
  return 1;
}

func app_result app_init(cmdline cmdl, void** state) {
  (void)cmdl;

  if (state == NULL) {
    thread_log_error("Standalone app received NULL state pointer");
    return APP_RESULT_FAIL;
  }

  allocator alloc = entry_get_allocator();
  app_entry_state* state_ptr = (app_entry_state*)allocator_calloc(alloc, 1, size_of(*state_ptr));
  if (state_ptr == NULL) {
    thread_log_error("Failed allocating standalone app state");
    return APP_RESULT_FAIL;
  }

  state_ptr->window_id = window_create("based core app entry standalone", 960, 540, WINDOW_MODE_WINDOWED, 0);
  if (state_ptr->window_id == NULL) {
    thread_log_error("Failed creating standalone app window");
    allocator_dealloc(alloc, state_ptr);
    return APP_RESULT_FAIL;
  }

  msg_handler_desc close_desc = {0};
  close_desc.handler_fn = app_window_close_handler;
  close_desc.user_data = state_ptr;
  close_desc.category = MSG_CATEGORY_CORE;
  close_desc.type = MSG_CORE_TYPE_WINDOW_CLOSE_REQUESTED;
  state_ptr->close_handler_id = msg_add_handler(&close_desc);
  if (state_ptr->close_handler_id == 0) {
    thread_log_error("Failed registering standalone app close handler");
    window_destroy(state_ptr->window_id);
    allocator_dealloc(alloc, state_ptr);
    return APP_RESULT_FAIL;
  }

  thread_log_info("Standalone app window created");
  *state = state_ptr;
  return APP_RESULT_CONTINUE;
}

func app_result app_update(void* state) {
  app_entry_state* state_ptr = (app_entry_state*)state;
  if (state_ptr == NULL) {
    thread_log_error("Standalone app state is NULL during update");
    return APP_RESULT_FAIL;
  }

  if (state_ptr->should_quit || !window_is_valid(state_ptr->window_id)) {
    return APP_RESULT_SUCCESS;
  }

  return APP_RESULT_CONTINUE;
}

func void app_quit(void* state, app_result result) {
  app_entry_state* state_ptr = (app_entry_state*)state;
  if (state_ptr == NULL) {
    return;
  }

  thread_log_info("Standalone app quitting result=%u", (u32)result);
  if (state_ptr->close_handler_id != 0) {
    if (!msg_remove_handler(state_ptr->close_handler_id)) {
      thread_log_warn("Failed removing standalone app close handler");
    }
  }
  if (window_is_valid(state_ptr->window_id)) {
    if (!window_destroy(state_ptr->window_id)) {
      thread_log_warn("Failed destroying standalone app window during quit");
    }
  }

  allocator_dealloc(entry_get_allocator(), state_ptr);
}
