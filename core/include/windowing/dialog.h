// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "window.h"

// =========================================================================
c_begin;
// =========================================================================

typedef enum dialog_message_kind {
  DIALOG_MESSAGE_KIND_INFORMATION = 1,
  DIALOG_MESSAGE_KIND_WARNING = 2,
  DIALOG_MESSAGE_KIND_ERROR = 3,
} dialog_message_kind;

typedef enum dialog_message_box_flag {
  DIALOG_MESSAGE_BOX_FLAG_ERROR = (1u << 0),
  DIALOG_MESSAGE_BOX_FLAG_WARNING = (1u << 1),
  DIALOG_MESSAGE_BOX_FLAG_INFORMATION = (1u << 2),
  DIALOG_MESSAGE_BOX_FLAG_BUTTONS_LEFT_TO_RIGHT = (1u << 3),
  DIALOG_MESSAGE_BOX_FLAG_BUTTONS_RIGHT_TO_LEFT = (1u << 4),
} dialog_message_box_flag;

typedef enum dialog_message_box_button_flag {
  DIALOG_MESSAGE_BOX_BUTTON_FLAG_RETURNKEY_DEFAULT = (1u << 0),
  DIALOG_MESSAGE_BOX_BUTTON_FLAG_ESCAPEKEY_DEFAULT = (1u << 1),
} dialog_message_box_button_flag;

typedef struct dialog_message_box_button {
  u32 flags;
  i32 button_id;
  cstr8 text;
} dialog_message_box_button;

typedef struct dialog_message_box_color {
  u8 red;
  u8 green;
  u8 blue;
} dialog_message_box_color;

typedef struct dialog_message_box_color_scheme {
  dialog_message_box_color background;
  dialog_message_box_color text;
  dialog_message_box_color button_border;
  dialog_message_box_color button_background;
  dialog_message_box_color button_selected;
} dialog_message_box_color_scheme;

typedef struct dialog_message_box {
  u32 flags;
  cstr8 title;
  cstr8 message;
  const dialog_message_box_button* buttons;
  sz button_count;
  const dialog_message_box_color_scheme* color_scheme;
} dialog_message_box;

typedef struct dialog_file_filter {
  cstr8 name;
  cstr8 pattern;
} dialog_file_filter;

typedef void dialog_file_callback(
    void* user_data,
    cstr8 const* file_list,
    i32 filter_idx,
    b32 is_cancelled,
    b32 has_error);

// Shows a simple native message dialog.
func b32 dialog_show_message(
    window owner,
    dialog_message_kind message_kind,
    cstr8 title,
    cstr8 message);

// Shows a customizable native message box with optional button output.
func b32 dialog_show_message_box(window owner, const dialog_message_box* message_box, i32* out_button_id);

// Opens a native file-picker dialog.
func b32 dialog_open_file(
    window owner,
    const dialog_file_filter* filters,
    sz filter_count,
    cstr8 default_location,
    b32 allow_many,
    dialog_file_callback* callback,
    void* user_data);

// Opens a native save-file dialog.
func b32 dialog_save_file(
    window owner,
    const dialog_file_filter* filters,
    sz filter_count,
    cstr8 default_location,
    dialog_file_callback* callback,
    void* user_data);

// Opens a native folder-picker dialog.
func b32 dialog_open_folder(
    window owner,
    cstr8 default_location,
    b32 allow_many,
    dialog_file_callback* callback,
    void* user_data);

// =========================================================================
c_end;
// =========================================================================
