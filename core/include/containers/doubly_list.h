// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "basic/primitive_types.h"
#include "basic/utility_defines.h"
#include "basic/safe.h"

// =========================================================================
c_begin;
// =========================================================================

/*
DOUBLY_LIST_* manages an intrusive linear list with head and tail pointers.
Each node must provide `prev` and `next` members.

Example:

  typedef struct job_node {
    struct job_node* prev;
    struct job_node* next;
    i32 priority;
  } job_node;
*/

#define DOUBLY_LIST_EMPTY(head, tail) ((head) == nullptr)

#define DOUBLY_LIST_COUNT(head, tail, count) stmt(                             \
    (void)(tail);                                                              \
    (count) = 0;                                                               \
    safe_for (typeof(head) _node = (head); _node != nullptr; _node = _node->next) { \
      (count)++;                                                               \
    })

#define DOUBLY_LIST_HEAD(head, tail) (head)
#define DOUBLY_LIST_TAIL(head, tail) (tail)

#define DOUBLY_LIST_PUSH_FRONT(head, tail, node) stmt( \
    (node)->prev = nullptr;                            \
    (node)->next = (head);                             \
    if ((head) != nullptr) {                           \
      (head)->prev = (node);                           \
    } else {                                           \
      (tail) = (node);                                 \
    }(head) = (node);)

#define DOUBLY_LIST_PUSH_BACK(head, tail, node) stmt( \
    (node)->next = nullptr;                           \
    (node)->prev = (tail);                            \
    if ((tail) != nullptr) {                          \
      (tail)->next = (node);                          \
    } else {                                          \
      (head) = (node);                                \
    }(tail) = (node);)

#define DOUBLY_LIST_POP_FRONT(head, tail, node) stmt( \
    (node) = (head);                                  \
    if ((head) != nullptr) {                          \
      (head) = (head)->next;                          \
      if ((head) != nullptr) {                        \
        (head)->prev = nullptr;                       \
      } else {                                        \
        (tail) = nullptr;                             \
      }                                               \
      (node)->next = nullptr;                         \
    })

#define DOUBLY_LIST_POP_BACK(head, tail, node) stmt( \
    (node) = (tail);                                 \
    if ((tail) != nullptr) {                         \
      (tail) = (tail)->prev;                         \
      if ((tail) != nullptr) {                       \
        (tail)->next = nullptr;                      \
      } else {                                       \
        (head) = nullptr;                            \
      }                                              \
      (node)->prev = nullptr;                        \
    })

#define DOUBLY_LIST_REMOVE(head, tail, node) stmt( \
    if ((node)->prev != nullptr) {                 \
      (node)->prev->next = (node)->next;           \
    } else {                                       \
      (head) = (node)->next;                       \
    } if ((node)->next != nullptr) {               \
      (node)->next->prev = (node)->prev;           \
    } else {                                       \
      (tail) = (node)->prev;                       \
    }(node)                                        \
        ->next = nullptr;                          \
    (node)->prev = nullptr;)

#define DOUBLY_LIST_INSERT_AFTER(head, tail, after, node) stmt( \
    (void)(head);                                               \
    (node)->prev = (after);                                     \
    (node)->next = (after)->next;                               \
    if ((after)->next != nullptr) {                             \
      (after)->next->prev = (node);                             \
    } else {                                                    \
      (tail) = (node);                                          \
    }(after)                                                    \
        ->next = (node);)

#define DOUBLY_LIST_INSERT_BEFORE(head, tail, before, node) stmt( \
    (void)(tail);                                                 \
    (node)->next = (before);                                      \
    (node)->prev = (before)->prev;                                \
    if ((before)->prev != nullptr) {                              \
      (before)->prev->next = (node);                              \
    } else {                                                      \
      (head) = (node);                                            \
    }(before)                                                     \
        ->prev = (node);)

#define DOUBLY_LIST_FOREACH(head, tail, it) \
  safe_for (typeof((head)) it = (head); (it) != nullptr; (it) = (it)->next)

#define DOUBLY_LIST_FOREACH_REVERSE(head, tail, it) \
  safe_for (typeof((tail)) it = (tail); (it) != nullptr; (it) = (it)->prev)

// =========================================================================
c_end;
// =========================================================================
