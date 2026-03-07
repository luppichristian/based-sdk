// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "basic/primitive_types.h"
#include "basic/utility_defines.h"

#define DOUBLY_LIST_EMPTY(head, tail) ((head) == nullptr)

#define DOUBLY_LIST_COUNT(head, tail, count) expr_stmt( \
  (void)(tail);                                           \
  (count) = 0;                                            \
  for (typeof(head) _node = (head); _node != nullptr; _node = _node->next) { \
    (count)++;                                            \
  })

#define DOUBLY_LIST_HEAD(head, tail) (head)
#define DOUBLY_LIST_TAIL(head, tail) (tail)

#define DOUBLY_LIST_PUSH_FRONT(head, tail, node) expr_stmt( \
  (node)->prev = nullptr;                                     \
  (node)->next = (head);                                      \
  if ((head) != nullptr) {                                    \
    (head)->prev = (node);                                    \
  } else {                                                    \
    (tail) = (node);                                          \
  }                                                           \
  (head) = (node);)

#define DOUBLY_LIST_PUSH_BACK(head, tail, node) expr_stmt( \
  (node)->next = nullptr;                                    \
  (node)->prev = (tail);                                     \
  if ((tail) != nullptr) {                                   \
    (tail)->next = (node);                                   \
  } else {                                                   \
    (head) = (node);                                         \
  }                                                          \
  (tail) = (node);)

#define DOUBLY_LIST_POP_FRONT(head, tail, node) expr_stmt( \
  (node) = (head);                                           \
  if ((head) != nullptr) {                                   \
    (head) = (head)->next;                                   \
    if ((head) != nullptr) {                                 \
      (head)->prev = nullptr;                                \
    } else {                                                 \
      (tail) = nullptr;                                      \
    }                                                        \
    (node)->next = nullptr;                                  \
  })

#define DOUBLY_LIST_POP_BACK(head, tail, node) expr_stmt( \
  (node) = (tail);                                          \
  if ((tail) != nullptr) {                                  \
    (tail) = (tail)->prev;                                  \
    if ((tail) != nullptr) {                                \
      (tail)->next = nullptr;                               \
    } else {                                                \
      (head) = nullptr;                                     \
    }                                                       \
    (node)->prev = nullptr;                                 \
  })

#define DOUBLY_LIST_REMOVE(head, tail, node) expr_stmt(       \
  if ((node)->prev != nullptr) {                               \
    (node)->prev->next = (node)->next;                         \
  } else {                                                     \
    (head) = (node)->next;                                     \
  }                                                            \
  if ((node)->next != nullptr) {                               \
    (node)->next->prev = (node)->prev;                         \
  } else {                                                     \
    (tail) = (node)->prev;                                     \
  }                                                            \
  (node)->next = nullptr;                                      \
  (node)->prev = nullptr;)

#define DOUBLY_LIST_INSERT_AFTER(head, tail, after, node) expr_stmt( \
  (void)(head);                                                       \
  (node)->prev = (after);                                             \
  (node)->next = (after)->next;                                       \
  if ((after)->next != nullptr) {                                     \
    (after)->next->prev = (node);                                     \
  } else {                                                            \
    (tail) = (node);                                                  \
  }                                                                   \
  (after)->next = (node);)

#define DOUBLY_LIST_INSERT_BEFORE(head, tail, before, node) expr_stmt( \
  (void)(tail);                                                         \
  (node)->next = (before);                                              \
  (node)->prev = (before)->prev;                                        \
  if ((before)->prev != nullptr) {                                      \
    (before)->prev->next = (node);                                      \
  } else {                                                              \
    (head) = (node);                                                    \
  }                                                                     \
  (before)->prev = (node);)

#define DOUBLY_LIST_FOREACH(head, tail, it) \
  for (typeof(head) it = (head); (it) != nullptr; (it) = (it)->next)

#define DOUBLY_LIST_FOREACH_REVERSE(head, tail, it) \
  for (typeof(tail) it = (tail); (it) != nullptr; (it) = (it)->prev)
