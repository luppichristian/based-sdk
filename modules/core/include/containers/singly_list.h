// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "basic/primitive_types.h"
#include "basic/safe.h"
#include "basic/utility_defines.h"

// =========================================================================
c_begin;
// =========================================================================

/*
SINGLY_LIST_* manages an intrusive forward list with head and tail pointers.
Each node must provide a `next` member.

Example:

  typedef struct packet_node {
    struct packet_node* next;
    u32 size;
  } packet_node;
*/

#define SINGLY_LIST_EMPTY(head, tail) ((head) == NULL)

#define SINGLY_LIST_COUNT(head, tail, count) stmt(                                  \
    (void)(tail);                                                                   \
    (count) = 0;                                                                    \
    safe_for (type_of((head)) _node = (head); _node != NULL; _node = _node->next) { \
      (count)++;                                                                    \
    })

#define SINGLY_LIST_HEAD(head, tail) (head)
#define SINGLY_LIST_TAIL(head, tail) (tail)

#define SINGLY_LIST_PUSH_FRONT(head, tail, node) stmt( \
    (node)->next = (head);                             \
    if ((head) == NULL) {                              \
      (tail) = (node);                                 \
    }(head) = (node);)

#define SINGLY_LIST_PUSH_BACK(head, tail, node) stmt( \
    (node)->next = NULL;                              \
    if ((tail) != NULL) {                             \
      (tail)->next = (node);                          \
    } else {                                          \
      (head) = (node);                                \
    }(tail) = (node);)

#define SINGLY_LIST_POP_FRONT(head, tail, node) stmt( \
    (node) = (head);                                  \
    if ((head) != NULL) {                             \
      (head) = (head)->next;                          \
      if ((head) == NULL) {                           \
        (tail) = NULL;                                \
      }                                               \
      (node)->next = NULL;                            \
    })

#define SINGLY_LIST_FOREACH(head, tail, it) \
  safe_for (type_of((head)) it = (head); (it) != NULL; (it) = (it)->next)

// =========================================================================
c_end;
// =========================================================================
