// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/codespace.h"
#include "../basic/utility_defines.h"

// =========================================================================
// Input Messages
// =========================================================================
//
// Threading model:
// - SDL's event queue is process-global, not per-thread.
// - msg_post enqueues into that global queue.
// - msg_pump/msg_poll/msg_wait/msg_wait_timeout all consume from the same queue.
// - In practice these pump/poll/wait calls should be centralized on one thread
//   (typically the main thread) to avoid event-consumption races.
// - Message handlers run on the posting thread (inside msg_post), ordered by
//   descending priority.

typedef enum msg_handler_stage {
  MSG_HANDLER_STAGE_BEFORE_POST = 1,
  MSG_HANDLER_STAGE_AFTER_POST = 2,
  MSG_HANDLER_STAGE_POST_FAILED = 3,
} msg_handler_stage;

typedef enum msg_handler_result {
  MSG_HANDLER_RESULT_CONTINUE = 0,
  MSG_HANDLER_RESULT_STOP_DISPATCH = 1,
  MSG_HANDLER_RESULT_CANCEL_POST = 2,
} msg_handler_result;

typedef enum msg_handler_option {
  MSG_HANDLER_OPTION_NONE = 0,
  MSG_HANDLER_OPTION_ONCE = (1u << 0),
  MSG_HANDLER_OPTION_STAGE_BEFORE_POST = (1u << 1),
  MSG_HANDLER_OPTION_STAGE_AFTER_POST = (1u << 2),
  MSG_HANDLER_OPTION_STAGE_POST_FAILED = (1u << 3),
  MSG_HANDLER_OPTION_DISABLED = (1u << 4),
} msg_handler_option;

typedef enum msg_category {
  MSG_CATEGORY_CORE,
  MSG_CATEGORY_GRAPHICS,
  MSG_CATEGORY_AUDIO,
  MSG_CATEGORY_USER0,
  MSG_CATEGORY_USER1,
  MSG_CATEGORY_USER2,
  MSG_CATEGORY_USER3,
  MSG_CATEGORY_MAX,
} msg_category;

const_var sz MSG_DATA_SIZE = kb(2);

// Normalized event record used by the input message queue.
typedef struct msg {
  u32 type;
  u64 timestamp;
  callsite post_site;
  u8 data[MSG_DATA_SIZE];
  msg_category category;
} msg;

typedef msg_handler_result (*msg_handler_fn)(msg* src, msg_handler_stage stage, void* user_data);
typedef b32 (*msg_filter_fn)(const msg* src, void* user_data);

typedef struct msg_handler_desc {
  // Required.
  msg_handler_fn handler_fn;
  void* user_data;
  // Higher values run first.
  i32 priority;
  // Bitmask of msg_handler_option.
  u32 options;
  // Inclusive type filter. Set both to 0 to match all message types.
  u32 type_min;
  u32 type_max;
} msg_handler_desc;

// Processes backend events and refreshes the process-global SDL queue.
// Prefer calling from the main thread.
func void msg_pump(void);

// Pops the next message from the process-global queue into out_msg.
// Returns 1 on success, 0 otherwise.
func b32 msg_poll(msg* out_msg);

// Waits indefinitely on the process-global queue and writes the next message to out_msg.
func b32 msg_wait(msg* out_msg);

// Waits up to timeout_ms on the process-global queue and writes the next message to out_msg.
func b32 msg_wait_timeout(msg* out_msg, i32 timeout_ms);
// Queue inspection helpers.
func b32 msg_peek(msg* out_msg);
func i32 msg_count(u32 type_min, u32 type_max);
func void msg_flush(u32 type_min, u32 type_max);

// Posts src into the process-global queue. Returns 1 on success, 0 otherwise.
// Suitable for cross-thread producers that need to signal the consumer thread.
func b32 _msg_post(const msg* src, callsite site);
#define msg_post(src) _msg_post((src), CALLSITE_HERE)

// Registers one message handler and returns a non-zero handler id on success.
// Handlers are dispatched by descending priority.
// If no stage flags are set in desc->options, BEFORE_POST is implied.
func u64 msg_add_handler(const msg_handler_desc* desc);

// Removes a previously registered message handler.
func b32 msg_remove_handler(u64 handler_id);

// Removes all registered message handlers.
func void msg_clear_handlers(void);
// Installs or clears a process-global event filter callback.
func void msg_set_filter(msg_filter_fn filter_fn, void* user_data);

// Converts a backend-native event object into a msg. Returns 1 on success, 0 otherwise.
func b32 msg_from_native(const void* native_event, msg* out_msg);

// Converts src into a backend-native event object. Returns 1 on success, 0 otherwise.
func b32 msg_to_native(const msg* src, void* native_event);
