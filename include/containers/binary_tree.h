// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "basic/primitive_types.h"
#include "basic/utility_defines.h"

// A binary_tree is an intrusive structural binary tree.
// Nodes must embed three pointers of the same node type: left, right, parent.
// No ordering invariant is enforced; that is the caller's responsibility.
// All structural operations are O(1). All traversals are fully iterative via parent
// backtracking — no external stack is required.
// BINARY_TREE_ROTATE_LEFT / _RIGHT serve as building blocks for balanced-tree layers
// (e.g. red-black trees) implemented on top of this header.

// BINARY_TREE_IS_ROOT checks if a node has no parent (i.e. it is the tree root).
#define BINARY_TREE_IS_ROOT(node) expr((node)->parent == nullptr)

// BINARY_TREE_IS_LEAF checks if a node has no children.
#define BINARY_TREE_IS_LEAF(node) \
  expr((node)->left == nullptr && (node)->right == nullptr)

// BINARY_TREE_INSERT_LEFT sets node as the left child of parent.
// The previous left child is orphaned; the caller is responsible for it.
#define BINARY_TREE_INSERT_LEFT(parent, node) expr({ \
  (node)->parent = (parent);                         \
  (parent)->left = (node);                           \
})

// BINARY_TREE_INSERT_RIGHT sets node as the right child of parent.
// The previous right child is orphaned; the caller is responsible for it.
#define BINARY_TREE_INSERT_RIGHT(parent, node) expr({ \
  (node)->parent = (parent);                          \
  (parent)->right = (node);                           \
})

// BINARY_TREE_REMOVE detaches node from its parent. The node's own subtree is unmodified.
// If node is the root, *root_ptr is set to nullptr.
#define BINARY_TREE_REMOVE(root_ptr, node) expr({ \
  if ((node)->parent == nullptr) {                \
    *(root_ptr) = nullptr;                        \
  } else if ((node) == (node)->parent->left) {    \
    (node)->parent->left = nullptr;               \
  } else {                                        \
    (node)->parent->right = nullptr;              \
  }                                               \
  (node)->parent = nullptr;                       \
})

// BINARY_TREE_ROTATE_LEFT performs a left rotation around node.
// *root_ptr is updated if node is the current root.
//
//     node             right
//    /    \           /     \
//   a    right  →  node      c
//        /   \    /    \
//       b     c  a      b
//
#define BINARY_TREE_ROTATE_LEFT(root_ptr, node) expr({        \
  auto _right = (node)->right;                                \
  (node)->right = _right->left;                               \
  if (_right->left != nullptr) _right->left->parent = (node); \
  _right->parent = (node)->parent;                            \
  if ((node)->parent == nullptr)                              \
    *(root_ptr) = _right;                                     \
  else if ((node) == (node)->parent->left)                    \
    (node)->parent->left = _right;                            \
  else                                                        \
    (node)->parent->right = _right;                           \
  _right->left = (node);                                      \
  (node)->parent = _right;                                    \
})

// BINARY_TREE_ROTATE_RIGHT performs a right rotation around node.
// *root_ptr is updated if node is the current root.
//
//      node           left
//     /    \         /    \
//   left    c  →   a      node
//   /  \                  /   \
//  a    b                b     c
//
#define BINARY_TREE_ROTATE_RIGHT(root_ptr, node) expr({       \
  auto _left = (node)->left;                                  \
  (node)->left = _left->right;                                \
  if (_left->right != nullptr) _left->right->parent = (node); \
  _left->parent = (node)->parent;                             \
  if ((node)->parent == nullptr)                              \
    *(root_ptr) = _left;                                      \
  else if ((node) == (node)->parent->right)                   \
    (node)->parent->right = _left;                            \
  else                                                        \
    (node)->parent->left = _left;                             \
  _left->right = (node);                                      \
  (node)->parent = _left;                                     \
})

// =========================================================================
// Preorder traversal (root → left → right)
// =========================================================================

// BINARY_TREE_NEXT_PREORDER_ returns the next node in preorder after 'node' within
// the subtree rooted at 'root'. Returns nullptr when the traversal is complete.
// Internal use only.
#define BINARY_TREE_NEXT_PREORDER_(root, node) ({         \
  auto _cur = (node);                                     \
  auto _nxt = (node);                                     \
  _nxt = nullptr;                                         \
  if (_cur->left != nullptr) {                            \
    _nxt = _cur->left;                                    \
  } else if (_cur->right != nullptr) {                    \
    _nxt = _cur->right;                                   \
  } else {                                                \
    while (_cur != (root)) {                              \
      auto _par = _cur->parent;                           \
      if (_cur == _par->left && _par->right != nullptr) { \
        _nxt = _par->right;                               \
        break;                                            \
      }                                                   \
      _cur = _par;                                        \
    }                                                     \
  }                                                       \
  _nxt;                                                   \
})

// BINARY_TREE_FOREACH_PREORDER iterates every node in preorder (root → left → right).
// 'it' is declared as the loop variable; its type is deduced from 'root'.
#define BINARY_TREE_FOREACH_PREORDER(root, it) \
  for (auto it = (root); (it) != nullptr; (it) = BINARY_TREE_NEXT_PREORDER_(root, it))

// =========================================================================
// Inorder traversal (left → root → right)
// =========================================================================

// BINARY_TREE_FIRST_INORDER_ returns the first node in inorder (leftmost node).
// Internal use only.
#define BINARY_TREE_FIRST_INORDER_(root) ({                           \
  auto _cur = (root);                                                 \
  while (_cur != nullptr && _cur->left != nullptr) _cur = _cur->left; \
  _cur;                                                               \
})

// BINARY_TREE_NEXT_INORDER_ returns the next node in inorder after 'node' within
// the subtree rooted at 'root'. Returns nullptr when the traversal is complete.
// Internal use only.
#define BINARY_TREE_NEXT_INORDER_(root, node) ({     \
  auto _cur = (node);                                \
  auto _nxt = (node);                                \
  _nxt = nullptr;                                    \
  if (_cur->right != nullptr) {                      \
    _nxt = _cur->right;                              \
    while (_nxt->left != nullptr) _nxt = _nxt->left; \
  } else {                                           \
    while (_cur != (root)) {                         \
      auto _par = _cur->parent;                      \
      if (_cur == _par->left) {                      \
        _nxt = _par;                                 \
        break;                                       \
      }                                              \
      _cur = _par;                                   \
    }                                                \
  }                                                  \
  _nxt;                                              \
})

// BINARY_TREE_FOREACH_INORDER iterates every node in inorder (left → root → right).
// 'it' is declared as the loop variable; its type is deduced from 'root'.
#define BINARY_TREE_FOREACH_INORDER(root, it)                       \
  for (auto it = BINARY_TREE_FIRST_INORDER_(root); (it) != nullptr; \
       (it) = BINARY_TREE_NEXT_INORDER_(root, it))

// =========================================================================
// Postorder traversal (left → right → root)
// =========================================================================

// BINARY_TREE_FIRST_POSTORDER_ returns the first node in postorder (deepest leftmost leaf).
// Internal use only.
#define BINARY_TREE_FIRST_POSTORDER_(root) ({                                    \
  auto _cur = (root);                                                            \
  while (_cur != nullptr && (_cur->left != nullptr || _cur->right != nullptr)) { \
    if (_cur->left != nullptr) _cur = _cur->left;                                \
    else                                                                         \
      _cur = _cur->right;                                                        \
  }                                                                              \
  _cur;                                                                          \
})

// BINARY_TREE_NEXT_POSTORDER_ returns the next node in postorder after 'node' within
// the subtree rooted at 'root'. Returns nullptr when the traversal is complete.
// Internal use only.
#define BINARY_TREE_NEXT_POSTORDER_(root, node) ({              \
  auto _cur = (node);                                           \
  auto _nxt = (node);                                           \
  _nxt = nullptr;                                               \
  if (_cur != (root)) {                                         \
    auto _par = _cur->parent;                                   \
    if (_cur == _par->right || _par->right == nullptr) {        \
      _nxt = _par;                                              \
    } else {                                                    \
      _nxt = _par->right;                                       \
      while (_nxt->left != nullptr || _nxt->right != nullptr) { \
        if (_nxt->left != nullptr) _nxt = _nxt->left;           \
        else                                                    \
          _nxt = _nxt->right;                                   \
      }                                                         \
    }                                                           \
  }                                                             \
  _nxt;                                                         \
})

// BINARY_TREE_FOREACH_POSTORDER iterates every node in postorder (left → right → root).
// 'it' is declared as the loop variable; its type is deduced from 'root'.
#define BINARY_TREE_FOREACH_POSTORDER(root, it)                       \
  for (auto it = BINARY_TREE_FIRST_POSTORDER_(root); (it) != nullptr; \
       (it) = BINARY_TREE_NEXT_POSTORDER_(root, it))
