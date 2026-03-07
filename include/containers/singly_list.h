// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "basic/primitive_types.h"
#include "basic/utility_defines.h"

// A singly_list is an intrusive singly-linked list with head and tail pointers.
// Nodes must embed a `next` pointer of the same node type.
// Push to front/back and pop from front are all O(1); pop from back is O(n).

// SINGLY_LIST_EMPTY checks if the list is empty.
#define SINGLY_LIST_EMPTY(head, tail) expr((head) == nullptr)

// SINGLY_LIST_COUNT counts the number of nodes by traversing from head to end.
#define SINGLY_LIST_COUNT(head, tail, count) expr({ \
  (count) = 0;                                      \
  auto _node = (head);                              \
  while (_node != nullptr) {                        \
    (count)++;                                      \
    _node = _node->next;                            \
  }                                                 \
})

// SINGLY_LIST_HEAD returns the head node of the list.
#define SINGLY_LIST_HEAD(head, tail) expr((head))

// SINGLY_LIST_TAIL returns the tail node of the list.
#define SINGLY_LIST_TAIL(head, tail) expr((tail))

// SINGLY_LIST_PUSH_FRONT adds a node to the front of the list.
#define SINGLY_LIST_PUSH_FRONT(head, tail, node) expr({ \
  (node)->next = (head);                                \
  if ((head) == nullptr) (tail) = (node);               \
  (head) = (node);                                      \
})

// SINGLY_LIST_PUSH_BACK adds a node to the back of the list.
#define SINGLY_LIST_PUSH_BACK(head, tail, node) expr({ \
  (node)->next = nullptr;                              \
  if ((tail) != nullptr) (tail)->next = (node);        \
  else                                                 \
    (head) = (node);                                   \
  (tail) = (node);                                     \
})

// SINGLY_LIST_POP_FRONT removes the head node from the list and returns it.
#define SINGLY_LIST_POP_FRONT(head, tail, node) expr({ \
  (node) = (head);                                     \
  if ((head) != nullptr) {                             \
    (head) = (head)->next;                             \
    if ((head) == nullptr) (tail) = nullptr;           \
    (node)->next = nullptr;                            \
  }                                                    \
})

// SINGLY_LIST_FOREACH iterates over all nodes from head to tail.
// 'it' is declared as the loop variable; its type is deduced from 'head'.
#define SINGLY_LIST_FOREACH(head, tail, it) \
  for (auto it = (head); (it) != nullptr; (it) = (it)->next)
