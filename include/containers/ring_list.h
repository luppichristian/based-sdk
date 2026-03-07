// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "basic/primitive_types.h"
#include "basic/utility_defines.h"

// A ring_list is an intrusive circular doubly-linked list.
// Nodes must embed both `next` and `prev` pointers of the same node type.
// The list is a closed loop: the last node's `next` points back to `head`,
// and `head->prev` points to the last node (tail).
// An empty list is represented by `head == nullptr`.
// All operations are O(1) except COUNT, which is O(n).

// RING_LIST_EMPTY checks if the list is empty.
#define RING_LIST_EMPTY(head) expr((head) == nullptr)

// RING_LIST_COUNT counts the number of nodes by traversing the full circle.
#define RING_LIST_COUNT(head, count) expr({ \
  (count) = 0;                              \
  auto _node = (head);                      \
  if (_node != nullptr) {                   \
    do {                                    \
      (count)++;                            \
      _node = _node->next;                  \
    } while (_node != (head));              \
  }                                         \
})

// RING_LIST_HEAD returns the head node of the list.
#define RING_LIST_HEAD(head) expr((head))

// RING_LIST_TAIL returns the tail node (head->prev) of the list.
#define RING_LIST_TAIL(head) expr((head) != nullptr ? (head)->prev : nullptr)

// RING_LIST_PUSH_FRONT adds a node at the front; the node becomes the new head.
#define RING_LIST_PUSH_FRONT(head, node) expr({ \
  if ((head) == nullptr) {                      \
    (node)->next = (node);                      \
    (node)->prev = (node);                      \
  } else {                                      \
    (node)->next = (head);                      \
    (node)->prev = (head)->prev;                \
    (head)->prev->next = (node);                \
    (head)->prev = (node);                      \
  }                                             \
  (head) = (node);                              \
})

// RING_LIST_PUSH_BACK adds a node at the back (just before head).
#define RING_LIST_PUSH_BACK(head, node) expr({ \
  if ((head) == nullptr) {                     \
    (node)->next = (node);                     \
    (node)->prev = (node);                     \
    (head) = (node);                           \
  } else {                                     \
    (node)->next = (head);                     \
    (node)->prev = (head)->prev;               \
    (head)->prev->next = (node);               \
    (head)->prev = (node);                     \
  }                                            \
})

// RING_LIST_POP_FRONT removes the head node and returns it; head advances to the next node.
#define RING_LIST_POP_FRONT(head, node) expr({ \
  (node) = (head);                             \
  if ((head) != nullptr) {                     \
    if ((head)->next == (head)) {              \
      (head) = nullptr;                        \
    } else {                                   \
      (node)->next->prev = (node)->prev;       \
      (node)->prev->next = (node)->next;       \
      (head) = (node)->next;                   \
    }                                          \
    (node)->next = nullptr;                    \
    (node)->prev = nullptr;                    \
  }                                            \
})

// RING_LIST_POP_BACK removes the tail node (head->prev) and returns it.
#define RING_LIST_POP_BACK(head, node) expr({            \
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
  }                                                      \
})

// RING_LIST_REMOVE removes an arbitrary node from the list.
#define RING_LIST_REMOVE(head, node) expr({      \
  if ((node)->next == (node)) {                  \
    (head) = nullptr;                            \
  } else {                                       \
    (node)->prev->next = (node)->next;           \
    (node)->next->prev = (node)->prev;           \
    if ((head) == (node)) (head) = (node)->next; \
  }                                              \
  (node)->next = nullptr;                        \
  (node)->prev = nullptr;                        \
})

// RING_LIST_INSERT_AFTER inserts 'node' immediately after 'after'.
#define RING_LIST_INSERT_AFTER(head, after, node) expr({ \
  (node)->prev = (after);                                \
  (node)->next = (after)->next;                          \
  (after)->next->prev = (node);                          \
  (after)->next = (node);                                \
})

// RING_LIST_INSERT_BEFORE inserts 'node' immediately before 'before'.
// If 'before' is the current head, 'node' becomes the new head.
#define RING_LIST_INSERT_BEFORE(head, before, node) expr({ \
  (node)->next = (before);                                 \
  (node)->prev = (before)->prev;                           \
  (before)->prev->next = (node);                           \
  (before)->prev = (node);                                 \
  if ((head) == (before)) (head) = (node);                 \
})

// RING_LIST_FOREACH iterates forward over every node starting from head.
// 'it' is declared as the loop variable; its type is deduced from 'head'.
// The loop terminates after visiting the last node (whose next wraps back to head).
#define RING_LIST_FOREACH(head, it)       \
  for (auto it = (head); (it) != nullptr; \
       (it) = ((it)->next != (head) ? (it)->next : nullptr))

// RING_LIST_FOREACH_REVERSE iterates backward over every node starting from the tail.
// 'it' is declared as the loop variable; its type is deduced from 'head'.
// The loop terminates after visiting head (whose prev wraps back to the tail).
#define RING_LIST_FOREACH_REVERSE(head, it)                                     \
  for (auto it = ((head) != nullptr ? (head)->prev : nullptr); (it) != nullptr; \
       (it) = ((it) != (head) ? (it)->prev : nullptr))
