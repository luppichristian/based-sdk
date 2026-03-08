// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "basic/primitive_types.h"
#include "basic/utility_defines.h"

#define RING_LIST_EMPTY(head) ((head) == nullptr)

#define RING_LIST_COUNT(head, count) expr_stmt( \
    (count) = 0;                                \
    typeof(head) _node = (head);                \
    if (_node != nullptr) {                     \
      do {                                      \
        (count)++;                              \
        _node = _node->next;                    \
      } while (_node != (head));                \
    })

#define RING_LIST_HEAD(head) (head)
#define RING_LIST_TAIL(head) ((head) != nullptr ? (head)->prev : nullptr)

#define RING_LIST_PUSH_FRONT(head, node) expr_stmt( \
    if ((head) == nullptr) {                        \
      (node)->next = (node);                        \
      (node)->prev = (node);                        \
    } else {                                        \
      (node)->next = (head);                        \
      (node)->prev = (head)->prev;                  \
      (head)->prev->next = (node);                  \
      (head)->prev = (node);                        \
    }(head) = (node);)

#define RING_LIST_PUSH_BACK(head, node) expr_stmt( \
    if ((head) == nullptr) {                       \
      (node)->next = (node);                       \
      (node)->prev = (node);                       \
      (head) = (node);                             \
    } else {                                       \
      (node)->next = (head);                       \
      (node)->prev = (head)->prev;                 \
      (head)->prev->next = (node);                 \
      (head)->prev = (node);                       \
    })

#define RING_LIST_POP_FRONT(head, node) expr_stmt( \
    (node) = (head);                               \
    if ((head) != nullptr) {                       \
      if ((head)->next == (head)) {                \
        (head) = nullptr;                          \
      } else {                                     \
        (node)->next->prev = (node)->prev;         \
        (node)->prev->next = (node)->next;         \
        (head) = (node)->next;                     \
      }                                            \
      (node)->next = nullptr;                      \
      (node)->prev = nullptr;                      \
    })

#define RING_LIST_POP_BACK(head, node) expr_stmt(          \
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

#define RING_LIST_REMOVE(head, node) expr_stmt( \
    if ((node)->next == (node)) {               \
      (head) = nullptr;                         \
    } else {                                    \
      (node)->prev->next = (node)->next;        \
      (node)->next->prev = (node)->prev;        \
      if ((head) == (node)) {                   \
        (head) = (node)->next;                  \
      }                                         \
    }(node)                                     \
        ->next = nullptr;                       \
    (node)->prev = nullptr;)

#define RING_LIST_INSERT_AFTER(head, after, node) expr_stmt( \
    (void)(head);                                            \
    (node)->prev = (after);                                  \
    (node)->next = (after)->next;                            \
    (after)->next->prev = (node);                            \
    (after)->next = (node);)

#define RING_LIST_INSERT_BEFORE(head, before, node) expr_stmt( \
    (node)->next = (before);                                   \
    (node)->prev = (before)->prev;                             \
    (before)->prev->next = (node);                             \
    (before)->prev = (node);                                   \
    if ((head) == (before)) {                                  \
      (head) = (node);                                         \
    })

#define RING_LIST_FOREACH(head, it)                                         \
  for (typeof(head) it = (head), _ring_head_##it = (head); (it) != nullptr; \
       (it) = ((it)->next != _ring_head_##it ? (it)->next : nullptr))

#define RING_LIST_FOREACH_REVERSE(head, it)                                                      \
  for (typeof(head) it = ((head) != nullptr ? (head)->prev : nullptr), _ring_head_##it = (head); \
       (it) != nullptr;                                                                          \
       (it) = ((it) != _ring_head_##it ? (it)->prev : nullptr))
