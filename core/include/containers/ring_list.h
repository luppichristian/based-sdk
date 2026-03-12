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
RING_LIST_* manages an intrusive circular doubly linked list.
`head->prev` always points at the tail when the list is non-empty.
Each node must provide `prev` and `next` members.

Example:

  typedef struct frame_node {
    struct frame_node* prev;
    struct frame_node* next;
    u64 frame_id;
  } frame_node;
*/

#define RING_LIST_EMPTY(head) ((head) == nullptr)

#define RING_LIST_COUNT(head, count) stmt( \
    (count) = 0;                           \
    typeof(head) _node = (head);           \
    if (_node != nullptr) {                \
      safe_while (true) {                  \
        (count)++;                         \
        _node = _node->next;               \
        if (_node == (head)) {             \
          break;                           \
        }                                  \
      }                                    \
    })

#define RING_LIST_HEAD(head) (head)
#define RING_LIST_TAIL(head) ((head) != nullptr ? (head)->prev : nullptr)

#define RING_LIST_PUSH_FRONT(head, node) stmt( \
    if ((head) == nullptr) {                   \
      (node)->next = (node);                   \
      (node)->prev = (node);                   \
    } else {                                   \
      (node)->next = (head);                   \
      (node)->prev = (head)->prev;             \
      (head)->prev->next = (node);             \
      (head)->prev = (node);                   \
    }(head) = (node);)

#define RING_LIST_PUSH_BACK(head, node) stmt( \
    if ((head) == nullptr) {                  \
      (node)->next = (node);                  \
      (node)->prev = (node);                  \
      (head) = (node);                        \
    } else {                                  \
      (node)->next = (head);                  \
      (node)->prev = (head)->prev;            \
      (head)->prev->next = (node);            \
      (head)->prev = (node);                  \
    })

#define RING_LIST_POP_FRONT(head, node) stmt( \
    (node) = (head);                          \
    if ((head) != nullptr) {                  \
      if ((head)->next == (head)) {           \
        (head) = nullptr;                     \
      } else {                                \
        (node)->next->prev = (node)->prev;    \
        (node)->prev->next = (node)->next;    \
        (head) = (node)->next;                \
      }                                       \
      (node)->next = nullptr;                 \
      (node)->prev = nullptr;                 \
    })

#define RING_LIST_POP_BACK(head, node) stmt(               \
    (node) = ((head) != nullptr ? (head)->prev : nullptr); \
    if ((node) != nullptr) {                               \
      if ((head)->next == (head)) {                        \
        (head) = nullptr;                                  \
      } else {                                             \
        (node)->prev->next = (head);                       \
        (head)->prev = (node)->prev;                       \
      }                                                    \
      (node)->next = nullptr;                              \
      (node)->prev = nullptr;                              \
    })

#define RING_LIST_REMOVE(head, node) stmt( \
    if ((node)->next == (node)) {          \
      (head) = nullptr;                    \
    } else {                               \
      (node)->prev->next = (node)->next;   \
      (node)->next->prev = (node)->prev;   \
      if ((head) == (node)) {              \
        (head) = (node)->next;             \
      }                                    \
    }(node)                                \
        ->next = nullptr;                  \
    (node)->prev = nullptr;)

#define RING_LIST_INSERT_AFTER(head, after, node) stmt( \
    (void)(head);                                       \
    (node)->prev = (after);                             \
    (node)->next = (after)->next;                       \
    (after)->next->prev = (node);                       \
    (after)->next = (node);)

#define RING_LIST_INSERT_BEFORE(head, before, node) stmt( \
    (node)->next = (before);                              \
    (node)->prev = (before)->prev;                        \
    (before)->prev->next = (node);                        \
    (before)->prev = (node);                              \
    if ((head) == (before)) {                             \
      (head) = (node);                                    \
    })

#define RING_LIST_FOREACH(head, it)                                               \
  safe_for(typeof((head)) it = (head), _ring_head_##it = (head); (it) != nullptr; \
           (it) = ((it)->next != _ring_head_##it ? (it)->next : nullptr))

#define RING_LIST_FOREACH_REVERSE(head, it)                                                            \
  safe_for(typeof((head)) it = ((head) != nullptr ? (head)->prev : nullptr), _ring_head_##it = (head); \
           (it) != nullptr;                                                                            \
           (it) = ((it) != _ring_head_##it ? (it)->prev : nullptr))

// =========================================================================
c_end;
// =========================================================================
