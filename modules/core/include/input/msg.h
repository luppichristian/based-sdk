// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/codespace.h"
#include "../basic/utility_defines.h"

// =========================================================================
c_begin;
// =========================================================================

// =========================================================================
// Input Messages
// =========================================================================
//
// Threading model:
// - msg_post dispatches immediately on the calling thread.
// - Registered handlers run in descending priority order.
// - A handler may cancel delivery by returning 0, causing msg_post to return 0.
// - msg_poll and msg_post_native are SDL helpers. They translate native SDL
//   events to msg records and forward them through msg_post.
// - In practice SDL polling should still be centralized on one thread
//   (typically the main thread) to avoid native event-consumption races.

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

#define MSG_DATA_SIZE kb(2)

// Normalized event record used by the input messaging system.
typedef struct msg {
  u32 type;
  u64 timestamp;
  callsite post_site;
  u8 data[MSG_DATA_SIZE];
  msg_category category;
} msg;

typedef b32 (*msg_handler_fn)(msg* src, void* user_data);
typedef b32 (*msg_filter_fn)(const msg* src, void* user_data);

typedef struct msg_handler_desc {
  // Required.
  msg_handler_fn handler_fn;
  void* user_data;
  // Higher values run first.
  i32 priority;
  // Filter by one category and one type.
  // Set category to MSG_CATEGORY_MAX to match all categories.
  // Set type to 0 to match all message types within the selected category.
  msg_category category;
  u32 type;
} msg_handler_desc;

// Polls SDL once, translates the next accepted SDL event into out_msg, and
// dispatches it immediately through msg_post. Returns 1 when a message was
// accepted and posted, 0 otherwise.
func b32 msg_poll(msg* out_msg);

// Posts src immediately to registered handlers. Returns 1 when the message was
// accepted, 0 when it was filtered or canceled.
func b32 _msg_post(const msg* src, callsite site);
#define msg_post(src) _msg_post((src), CALLSITE_HERE)

// Registers one message handler and returns a non-zero handler id on success.
// Handlers are dispatched by descending priority.
func u64 msg_add_handler(const msg_handler_desc* desc);

// Removes a previously registered message handler.
func b32 msg_remove_handler(u64 handler_id);

// Installs or clears a process-global event filter callback.
func void msg_set_filter(msg_filter_fn filter_fn, void* user_data);

// =========================================================================
c_end;
// =========================================================================
