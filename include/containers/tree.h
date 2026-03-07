// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "basic/primitive_types.h"
#include "basic/utility_defines.h"

// A tree is an intrusive N-ary tree using the left-child / right-sibling representation.
// Nodes must embed five pointers of the same node type:
//   parent, first_child, last_child, next_sibling, prev_sibling.
// All structural operations are O(1).
// Depth-first preorder traversal is fully iterative via parent backtracking — no stack required.

// TREE_IS_ROOT checks if a node has no parent (i.e. it is a root node).
#define TREE_IS_ROOT(node) expr((node)->parent == nullptr)

// TREE_IS_LEAF checks if a node has no children.
#define TREE_IS_LEAF(node) expr((node)->first_child == nullptr)

// TREE_INSERT_CHILD_FRONT adds node as the first child of parent.
#define TREE_INSERT_CHILD_FRONT(parent, node) expr({ \
  (node)->parent = (parent);                         \
  (node)->prev_sibling = nullptr;                    \
  (node)->next_sibling = (parent)->first_child;      \
  if ((parent)->first_child != nullptr)              \
    (parent)->first_child->prev_sibling = (node);    \
  else                                               \
    (parent)->last_child = (node);                   \
  (parent)->first_child = (node);                    \
})

// TREE_INSERT_CHILD_BACK adds node as the last child of parent.
#define TREE_INSERT_CHILD_BACK(parent, node) expr({ \
  (node)->parent = (parent);                        \
  (node)->next_sibling = nullptr;                   \
  (node)->prev_sibling = (parent)->last_child;      \
  if ((parent)->last_child != nullptr)              \
    (parent)->last_child->next_sibling = (node);    \
  else                                              \
    (parent)->first_child = (node);                 \
  (parent)->last_child = (node);                    \
})

// TREE_INSERT_BEFORE inserts node as a sibling immediately before 'before'.
// 'before' must have a valid parent.
#define TREE_INSERT_BEFORE(before, node) expr({    \
  (node)->parent = (before)->parent;               \
  (node)->next_sibling = (before);                 \
  (node)->prev_sibling = (before)->prev_sibling;   \
  if ((before)->prev_sibling != nullptr)           \
    (before)->prev_sibling->next_sibling = (node); \
  else if ((before)->parent != nullptr)            \
    (before)->parent->first_child = (node);        \
  (before)->prev_sibling = (node);                 \
})

// TREE_INSERT_AFTER inserts node as a sibling immediately after 'after'.
// 'after' must have a valid parent.
#define TREE_INSERT_AFTER(after, node) expr({     \
  (node)->parent = (after)->parent;               \
  (node)->prev_sibling = (after);                 \
  (node)->next_sibling = (after)->next_sibling;   \
  if ((after)->next_sibling != nullptr)           \
    (after)->next_sibling->prev_sibling = (node); \
  else if ((after)->parent != nullptr)            \
    (after)->parent->last_child = (node);         \
  (after)->next_sibling = (node);                 \
})

// TREE_REMOVE detaches node from its parent and siblings.
// The node's own children are not modified; the caller is responsible for them.
#define TREE_REMOVE(node) expr({                               \
  if ((node)->prev_sibling != nullptr)                         \
    (node)->prev_sibling->next_sibling = (node)->next_sibling; \
  else if ((node)->parent != nullptr)                          \
    (node)->parent->first_child = (node)->next_sibling;        \
  if ((node)->next_sibling != nullptr)                         \
    (node)->next_sibling->prev_sibling = (node)->prev_sibling; \
  else if ((node)->parent != nullptr)                          \
    (node)->parent->last_child = (node)->prev_sibling;         \
  (node)->parent = nullptr;                                    \
  (node)->prev_sibling = nullptr;                              \
  (node)->next_sibling = nullptr;                              \
})

// TREE_FOREACH_CHILDREN iterates over direct children of parent from first to last.
// 'it' is declared as the loop variable; its type is deduced from first_child.
#define TREE_FOREACH_CHILDREN(parent, it) \
  for (auto it = (parent)->first_child; (it) != nullptr; (it) = (it)->next_sibling)

// TREE_FOREACH_CHILDREN_REVERSE iterates over direct children of parent from last to first.
// 'it' is declared as the loop variable; its type is deduced from last_child.
#define TREE_FOREACH_CHILDREN_REVERSE(parent, it) \
  for (auto it = (parent)->last_child; (it) != nullptr; (it) = (it)->prev_sibling)

// TREE_NEXT_PREORDER_ advances to the next node in depth-first preorder within the subtree
// rooted at 'root'. Returns nullptr when the traversal is complete. Internal use only.
#define TREE_NEXT_PREORDER_(root, node) ({                  \
  auto _cur = (node);                                       \
  auto _res = (node);                                       \
  _res = nullptr;                                           \
  if (_cur->first_child != nullptr) {                       \
    _res = _cur->first_child;                               \
  } else {                                                  \
    while (_cur != (root) && _cur->next_sibling == nullptr) \
      _cur = _cur->parent;                                  \
    if (_cur != (root)) _res = _cur->next_sibling;          \
  }                                                         \
  _res;                                                     \
})

// TREE_FOREACH_PREORDER performs an iterative depth-first preorder traversal rooted at 'root'.
// Every node in the subtree (root included) is visited exactly once.
// 'it' is declared as the loop variable; its type is deduced from 'root'.
#define TREE_FOREACH_PREORDER(root, it) \
  for (auto it = (root); (it) != nullptr; (it) = TREE_NEXT_PREORDER_(root, it))
