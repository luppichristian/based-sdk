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
STACK_LIST_* manages an intrusive LIFO chain rooted at `head`.
Each node must provide a `next` member.

Example:

  typedef struct free_node {
    struct free_node* next;
    void* memory;
  } free_node;
*/

#define STACK_LIST_EMPTY(head) ((head) == nullptr)

#define STACK_LIST_COUNT(head, count) stmt(                                    \
    (count) = 0;                                                               \
    safe_for (typeof(head) _node = (head); _node != nullptr; _node = _node->next) { \
      (count)++;                                                               \
    })

#define STACK_LIST_HEAD(head) (head)
#define STACK_LIST_NEXT(node) ((node)->next)

#define STACK_LIST_PUSH(head, node) stmt( \
    (node)->next = (head);                \
    (head) = (node);)

#define STACK_LIST_POP(head, node) stmt( \
    (node) = (head);                     \
    if ((head) != nullptr) {             \
      (head) = (head)->next;             \
      (node)->next = nullptr;            \
    })

#define STACK_LIST_FOREACH(head, it) \
  safe_for (typeof((head)) it = (head); (it) != nullptr; (it) = (it)->next)

// =========================================================================
c_end;
// =========================================================================
