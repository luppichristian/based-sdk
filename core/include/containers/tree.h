// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "basic/primitive_types.h"
#include "basic/utility_defines.h"

// =========================================================================
c_begin;
// =========================================================================

typedef struct tree_links {
  struct tree_links* parent;
  struct tree_links* first_child;
  struct tree_links* last_child;
  struct tree_links* next_sibling;
  struct tree_links* prev_sibling;
} tree_links;

// Returns the next node in a pre-order traversal.
func force_inline void* tree_next_preorder_ptr(void* root_ptr, void* node_ptr) {
  tree_links* root_node = (tree_links*)root_ptr;
  tree_links* cursor = (tree_links*)node_ptr;

  if (root_node == NULL || cursor == NULL) {
    return NULL;
  }

  if (cursor->first_child != NULL) {
    return cursor->first_child;
  }

  while (cursor != root_node && cursor->next_sibling == NULL) {
    cursor = cursor->parent;
  }

  if (cursor != root_node) {
    return cursor->next_sibling;
  }

  return NULL;
}

// Structural predicates.
#define TREE_IS_ROOT(node) ((node)->parent == nullptr)
#define TREE_IS_LEAF(node) ((node)->first_child == nullptr)

// Mutation helpers.
#define TREE_INSERT_CHILD_FRONT(parent, node) expr_stmt( \
    (node)->parent = (parent);                           \
    (node)->prev_sibling = nullptr;                      \
    (node)->next_sibling = (parent)->first_child;        \
    if ((parent)->first_child != nullptr) {              \
      (parent)->first_child->prev_sibling = (node);      \
    } else {                                             \
      (parent)->last_child = (node);                     \
    }(parent)                                            \
        ->first_child = (node);)

#define TREE_INSERT_CHILD_BACK(parent, node) expr_stmt( \
    (node)->parent = (parent);                          \
    (node)->next_sibling = nullptr;                     \
    (node)->prev_sibling = (parent)->last_child;        \
    if ((parent)->last_child != nullptr) {              \
      (parent)->last_child->next_sibling = (node);      \
    } else {                                            \
      (parent)->first_child = (node);                   \
    }(parent)                                           \
        ->last_child = (node);)

#define TREE_INSERT_BEFORE(before, node) expr_stmt(  \
    (node)->parent = (before)->parent;               \
    (node)->next_sibling = (before);                 \
    (node)->prev_sibling = (before)->prev_sibling;   \
    if ((before)->prev_sibling != nullptr) {         \
      (before)->prev_sibling->next_sibling = (node); \
    } else if ((before)->parent != nullptr) {        \
      (before)->parent->first_child = (node);        \
    }(before)                                        \
        ->prev_sibling = (node);)

#define TREE_INSERT_AFTER(after, node) expr_stmt(   \
    (node)->parent = (after)->parent;               \
    (node)->prev_sibling = (after);                 \
    (node)->next_sibling = (after)->next_sibling;   \
    if ((after)->next_sibling != nullptr) {         \
      (after)->next_sibling->prev_sibling = (node); \
    } else if ((after)->parent != nullptr) {        \
      (after)->parent->last_child = (node);         \
    }(after)                                        \
        ->next_sibling = (node);)

#define TREE_REMOVE(node) expr_stmt(                             \
    if ((node)->prev_sibling != nullptr) {                       \
      (node)->prev_sibling->next_sibling = (node)->next_sibling; \
    } else if ((node)->parent != nullptr) {                      \
      (node)->parent->first_child = (node)->next_sibling;        \
    } if ((node)->next_sibling != nullptr) {                     \
      (node)->next_sibling->prev_sibling = (node)->prev_sibling; \
    } else if ((node)->parent != nullptr) {                      \
      (node)->parent->last_child = (node)->prev_sibling;         \
    }(node)                                                      \
        ->parent = nullptr;                                      \
    (node)->prev_sibling = nullptr;                              \
    (node)->next_sibling = nullptr;)

// Typed traversal macros.
#define TREE_FOREACH_CHILDREN(parent, it) \
  for (typeof((parent)->first_child) it = (parent)->first_child; (it) != nullptr; (it) = (it)->next_sibling)

#define TREE_FOREACH_CHILDREN_REVERSE(parent, it) \
  for (typeof((parent)->last_child) it = (parent)->last_child; (it) != nullptr; (it) = (it)->prev_sibling)

#define TREE_FOREACH_PREORDER(root, it) \
  for (typeof(root) it = (root); (it) != nullptr; (it) = (typeof(root))tree_next_preorder_ptr((void*)(root), (void*)(it)))

// =========================================================================
c_end;
// =========================================================================
