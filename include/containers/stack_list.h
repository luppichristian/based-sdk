// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "basic/primitive_types.h"
#include "basic/utility_defines.h"

#define STACK_LIST_EMPTY(head) ((head) == nullptr)

#define STACK_LIST_COUNT(head, count) expr_stmt(                               \
    (count) = 0;                                                               \
    for (typeof(head) _node = (head); _node != nullptr; _node = _node->next) { \
      (count)++;                                                               \
    })

#define STACK_LIST_HEAD(head) (head)
#define STACK_LIST_NEXT(node) ((node)->next)

#define STACK_LIST_PUSH(head, node) expr_stmt( \
    (node)->next = (head);                     \
    (head) = (node);)

#define STACK_LIST_POP(head, node) expr_stmt( \
    (node) = (head);                          \
    if ((head) != nullptr) {                  \
      (head) = (head)->next;                  \
      (node)->next = nullptr;                 \
    })

#define STACK_LIST_FOREACH(head, it) \
  for (typeof(head) it = (head); (it) != nullptr; (it) = (it)->next)
