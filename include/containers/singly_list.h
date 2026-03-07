// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "basic/primitive_types.h"
#include "basic/utility_defines.h"

#define SINGLY_LIST_EMPTY(head, tail) ((head) == nullptr)

#define SINGLY_LIST_COUNT(head, tail, count) expr_stmt( \
  (void)(tail);                                           \
  (count) = 0;                                            \
  for (typeof(head) _node = (head); _node != nullptr; _node = _node->next) { \
    (count)++;                                            \
  })

#define SINGLY_LIST_HEAD(head, tail) (head)
#define SINGLY_LIST_TAIL(head, tail) (tail)

#define SINGLY_LIST_PUSH_FRONT(head, tail, node) expr_stmt( \
  (node)->next = (head);                                     \
  if ((head) == nullptr) {                                   \
    (tail) = (node);                                         \
  }                                                          \
  (head) = (node);)

#define SINGLY_LIST_PUSH_BACK(head, tail, node) expr_stmt( \
  (node)->next = nullptr;                                    \
  if ((tail) != nullptr) {                                   \
    (tail)->next = (node);                                   \
  } else {                                                   \
    (head) = (node);                                         \
  }                                                          \
  (tail) = (node);)

#define SINGLY_LIST_POP_FRONT(head, tail, node) expr_stmt( \
  (node) = (head);                                           \
  if ((head) != nullptr) {                                   \
    (head) = (head)->next;                                   \
    if ((head) == nullptr) {                                 \
      (tail) = nullptr;                                      \
    }                                                        \
    (node)->next = nullptr;                                  \
  })

#define SINGLY_LIST_FOREACH(head, tail, it) \
  for (typeof(head) it = (head); (it) != nullptr; (it) = (it)->next)
