// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "basic/primitive_types.h"
#include "basic/utility_defines.h"

// =========================================================================
c_begin;
// =========================================================================

typedef struct binary_tree_links {
  struct binary_tree_links* left;
  struct binary_tree_links* right;
  struct binary_tree_links* parent;
} binary_tree_links;

// Returns the left-most node in an in-order traversal.
func force_inline void* binary_tree_first_inorder_ptr(void* root_ptr) {
  binary_tree_links* cursor = (binary_tree_links*)root_ptr;
  while (cursor != NULL && cursor->left != NULL) {
    cursor = cursor->left;
  }
  return cursor;
}

// Returns the next node in an in-order traversal.
func force_inline void* binary_tree_next_inorder_ptr(void* root_ptr, void* node_ptr) {
  binary_tree_links* root_node = (binary_tree_links*)root_ptr;
  binary_tree_links* cursor = (binary_tree_links*)node_ptr;
  if (root_node == NULL || cursor == NULL) {
    return NULL;
  }

  if (cursor->right != NULL) {
    cursor = cursor->right;
    while (cursor->left != NULL) {
      cursor = cursor->left;
    }
    return cursor;
  }

  while (cursor != root_node) {
    binary_tree_links* parent_node = cursor->parent;
    if (cursor == parent_node->left) {
      return parent_node;
    }
    cursor = parent_node;
  }

  return NULL;
}

// Returns the first node in a post-order traversal.
func force_inline void* binary_tree_first_postorder_ptr(void* root_ptr) {
  binary_tree_links* cursor = (binary_tree_links*)root_ptr;
  while (cursor != NULL && (cursor->left != NULL || cursor->right != NULL)) {
    cursor = cursor->left != NULL ? cursor->left : cursor->right;
  }
  return cursor;
}

// Returns the next node in a post-order traversal.
func force_inline void* binary_tree_next_postorder_ptr(void* root_ptr, void* node_ptr) {
  binary_tree_links* root_node = (binary_tree_links*)root_ptr;
  binary_tree_links* cursor = (binary_tree_links*)node_ptr;

  if (root_node == NULL || cursor == NULL || cursor == root_node) {
    return NULL;
  }

  binary_tree_links* parent_node = cursor->parent;
  if (cursor == parent_node->right || parent_node->right == NULL) {
    return parent_node;
  }

  cursor = parent_node->right;
  while (cursor->left != NULL || cursor->right != NULL) {
    cursor = cursor->left != NULL ? cursor->left : cursor->right;
  }

  return cursor;
}

// Returns the next node in a pre-order traversal.
func force_inline void* binary_tree_next_preorder_ptr(void* root_ptr, void* node_ptr) {
  binary_tree_links* root_node = (binary_tree_links*)root_ptr;
  binary_tree_links* cursor = (binary_tree_links*)node_ptr;

  if (root_node == NULL || cursor == NULL) {
    return NULL;
  }

  if (cursor->left != NULL) {
    return cursor->left;
  }
  if (cursor->right != NULL) {
    return cursor->right;
  }

  while (cursor != root_node) {
    binary_tree_links* parent_node = cursor->parent;
    if (cursor == parent_node->left && parent_node->right != NULL) {
      return parent_node->right;
    }
    cursor = parent_node;
  }

  return NULL;
}

// Structural predicates.
#define BINARY_TREE_IS_ROOT(node) ((node)->parent == nullptr)
#define BINARY_TREE_IS_LEAF(node) ((node)->left == nullptr && (node)->right == nullptr)

// Mutation helpers.
#define BINARY_TREE_INSERT_LEFT(parent, node) stmt( \
    (node)->parent = (parent);                      \
    (parent)->left = (node);)

#define BINARY_TREE_INSERT_RIGHT(parent, node) stmt( \
    (node)->parent = (parent);                       \
    (parent)->right = (node);)

#define BINARY_TREE_REMOVE(root_ptr, node) stmt( \
    if ((node)->parent == nullptr) {             \
      *(root_ptr) = nullptr;                     \
    } else if ((node) == (node)->parent->left) { \
      (node)->parent->left = nullptr;            \
    } else {                                     \
      (node)->parent->right = nullptr;           \
    }(node)                                      \
        ->parent = nullptr;)

#define BINARY_TREE_ROTATE_LEFT(root_ptr, node) stmt( \
    typeof((node)->right) _right = (node)->right;     \
    (node)->right = _right->left;                     \
    if (_right->left != nullptr) {                    \
      _right->left->parent = (node);                  \
    } _right->parent = (node)->parent;                \
    if ((node)->parent == nullptr) {                  \
      *(root_ptr) = _right;                           \
    } else if ((node) == (node)->parent->left) {      \
      (node)->parent->left = _right;                  \
    } else {                                          \
      (node)->parent->right = _right;                 \
    } _right->left = (node);                          \
    (node)->parent = _right;)

#define BINARY_TREE_ROTATE_RIGHT(root_ptr, node) stmt( \
    typeof((node)->left) _left = (node)->left;         \
    (node)->left = _left->right;                       \
    if (_left->right != nullptr) {                     \
      _left->right->parent = (node);                   \
    } _left->parent = (node)->parent;                  \
    if ((node)->parent == nullptr) {                   \
      *(root_ptr) = _left;                             \
    } else if ((node) == (node)->parent->left) {       \
      (node)->parent->left = _left;                    \
    } else {                                           \
      (node)->parent->right = _left;                   \
    } _left->right = (node);                           \
    (node)->parent = _left;)

// Typed traversal macros.
#define BINARY_TREE_FOREACH_PREORDER(root, it)    \
  for (typeof(root) it = (root); (it) != nullptr; \
       (it) = (typeof(root))binary_tree_next_preorder_ptr((void*)(root), (void*)(it)))

#define BINARY_TREE_FOREACH_INORDER(root, it)                                                         \
  for (typeof(root) it = (typeof(root))binary_tree_first_inorder_ptr((void*)(root)); (it) != nullptr; \
       (it) = (typeof(root))binary_tree_next_inorder_ptr((void*)(root), (void*)(it)))

#define BINARY_TREE_FOREACH_POSTORDER(root, it)                                                         \
  for (typeof(root) it = (typeof(root))binary_tree_first_postorder_ptr((void*)(root)); (it) != nullptr; \
       (it) = (typeof(root))binary_tree_next_postorder_ptr((void*)(root), (void*)(it)))

// =========================================================================
c_end;
// =========================================================================
