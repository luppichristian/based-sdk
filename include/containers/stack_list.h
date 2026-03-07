// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "basic/primitive_types.h"
#include "basic/utility_defines.h"

// A stack_list is an intrusive singly-linked list used as a LIFO stack.
// It is designed for fast, simple push/pop access through a single head pointer.

// The STACK_LIST_EMPTY macro checks if the stack is empty by checking if the head pointer is null.
#define STACK_LIST_EMPTY(head) expr((head) == nullptr)

// The STACK_LIST_COUNT macro counts the number of nodes in the stack by traversing it from the head to the end.
#define STACK_LIST_COUNT(head, count) expr({ \
  (count) = 0;                               \
  auto _node = (head);                       \
  while (_node != nullptr) {                 \
    (count)++;                               \
    _node = _node->next;                     \
  }                                          \
})

// The STACK_LIST_HEAD macro returns the head node of the stack.
#define STACK_LIST_HEAD(head) expr((head))

// The STACK_LIST_NEXT macro returns the next node of a given node.
#define STACK_LIST_NEXT(node) expr((node)->next)

// The STACK_LIST_PUSH macro adds a node to the head of the stack.
#define STACK_LIST_PUSH(head, node) expr({ \
  (node)->next = (head);                   \
  (head) = (node);                         \
})

// The STACK_LIST_POP macro removes a node from the head of the stack and returns it.
#define STACK_LIST_POP(head, node) expr({ \
  (node) = (head);                        \
  if ((head) != nullptr) {                \
    (head) = (head)->next;                \
    (node)->next = nullptr;               \
  }                                       \
})

// STACK_LIST_FOREACH iterates over all nodes from head to the end of the stack.
// 'it' is declared as the loop variable; its type is deduced from 'head'.
#define STACK_LIST_FOREACH(head, it) \
  for (auto it = (head); (it) != nullptr; (it) = (it)->next)
