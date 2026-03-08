// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

struct test_node {
  struct test_node* left;
  struct test_node* right;
  struct test_node* parent;
  i32 value;
};

TEST(containers_binary_tree_test, first_inorder_ptr) {
  struct test_node root = {0};
  struct test_node left = {0};
  struct test_node right = {0};

  root.left = &left;
  root.right = &right;
  left.parent = &root;
  right.parent = &root;

  struct test_node* first = (struct test_node*)binary_tree_first_inorder_ptr(&root);
  EXPECT_EQ(&left, first);
}

TEST(containers_binary_tree_test, next_inorder_ptr) {
  struct test_node root = {0};
  struct test_node left = {0};
  struct test_node right = {0};

  root.left = &left;
  root.right = &right;
  left.parent = &root;
  right.parent = &root;

  struct test_node* first = (struct test_node*)binary_tree_first_inorder_ptr(&root);
  struct test_node* next = (struct test_node*)binary_tree_next_inorder_ptr(&root, first);
  EXPECT_EQ(&root, next);

  next = (struct test_node*)binary_tree_next_inorder_ptr(&root, next);
  EXPECT_EQ(&right, next);

  next = (struct test_node*)binary_tree_next_inorder_ptr(&root, next);
  EXPECT_EQ(nullptr, next);
}

TEST(containers_binary_tree_test, first_postorder_ptr) {
  struct test_node root = {0};
  struct test_node left = {0};
  struct test_node right = {0};

  root.left = &left;
  root.right = &right;
  left.parent = &root;
  right.parent = &root;

  struct test_node* first = (struct test_node*)binary_tree_first_postorder_ptr(&root);
  EXPECT_EQ(&left, first);
}

TEST(containers_binary_tree_test, next_postorder_ptr) {
  struct test_node root = {0};
  struct test_node left = {0};
  struct test_node right = {0};

  root.left = &left;
  root.right = &right;
  left.parent = &root;
  right.parent = &root;

  struct test_node* first = (struct test_node*)binary_tree_first_postorder_ptr(&root);
  struct test_node* next = (struct test_node*)binary_tree_next_postorder_ptr(&root, first);
  EXPECT_EQ(&right, next);

  next = (struct test_node*)binary_tree_next_postorder_ptr(&root, next);
  EXPECT_EQ(&root, next);

  next = (struct test_node*)binary_tree_next_postorder_ptr(&root, next);
  EXPECT_EQ(nullptr, next);
}

TEST(containers_binary_tree_test, next_preorder_ptr) {
  struct test_node root = {0};
  struct test_node left = {0};
  struct test_node right = {0};

  root.left = &left;
  root.right = &right;
  left.parent = &root;
  right.parent = &root;

  struct test_node* next = (struct test_node*)binary_tree_next_preorder_ptr(&root, &root);
  EXPECT_EQ(&left, next);

  next = (struct test_node*)binary_tree_next_preorder_ptr(&root, next);
  EXPECT_EQ(&right, next);

  next = (struct test_node*)binary_tree_next_preorder_ptr(&root, next);
  EXPECT_EQ(nullptr, next);
}

TEST(containers_binary_tree_test, is_root_is_leaf) {
  struct test_node root = {0};
  struct test_node left = {0};

  root.left = &left;
  left.parent = &root;

  EXPECT_NE(0, BINARY_TREE_IS_ROOT(&root));
  EXPECT_EQ(0, BINARY_TREE_IS_ROOT(&left));

  EXPECT_NE(0, BINARY_TREE_IS_LEAF(&left));
  EXPECT_EQ(0, BINARY_TREE_IS_LEAF(&root));
}

TEST(containers_binary_tree_test, insert_left_right) {
  struct test_node root = {0};
  struct test_node left = {0};
  struct test_node right = {0};
  struct test_node* parent = &root;
  struct test_node* node = &left;

  BINARY_TREE_INSERT_LEFT(parent, node);
  EXPECT_EQ(&left, root.left);
  EXPECT_EQ(&root, left.parent);

  node = &right;

  BINARY_TREE_INSERT_RIGHT(parent, node);
  EXPECT_EQ(&right, root.right);
  EXPECT_EQ(&root, right.parent);
}

TEST(containers_binary_tree_test, remove) {
  struct test_node root = {0};
  struct test_node left = {0};

  root.left = &left;
  left.parent = &root;

  struct test_node* root_ptr = &root;
  BINARY_TREE_REMOVE(&root_ptr, &left);
  EXPECT_EQ(nullptr, root.left);
  EXPECT_EQ(nullptr, left.parent);
}

TEST(containers_binary_tree_test, rotate_left) {
  struct test_node root = {0};
  struct test_node* root_ptr = &root;
  struct test_node left_child = {0};
  struct test_node right_child = {0};

  root.right = &left_child;
  left_child.parent = &root;
  left_child.left = &right_child;
  right_child.parent = &left_child;

  BINARY_TREE_ROTATE_LEFT(&root_ptr, &root);
  EXPECT_EQ(&left_child, root_ptr);
  EXPECT_EQ(&root, left_child.left);
  EXPECT_EQ(nullptr, left_child.right);
  EXPECT_EQ(&right_child, root.right);
  EXPECT_EQ(&root, right_child.parent);
}

TEST(containers_binary_tree_test, rotate_right) {
  struct test_node root = {0};
  struct test_node* root_ptr = &root;
  struct test_node left_child = {0};
  struct test_node right_child = {0};

  root.left = &left_child;
  left_child.parent = &root;
  left_child.right = &right_child;
  right_child.parent = &left_child;

  BINARY_TREE_ROTATE_RIGHT(&root_ptr, &root);
  EXPECT_EQ(&left_child, root_ptr);
  EXPECT_EQ(&root, left_child.right);
  EXPECT_EQ(nullptr, left_child.left);
  EXPECT_EQ(&right_child, root.left);
  EXPECT_EQ(&root, right_child.parent);
}

TEST(containers_binary_tree_test, foreach_preorder) {
  struct test_node root = {0};
  struct test_node left = {0};
  struct test_node right = {0};

  root.value = 1;
  left.value = 2;
  right.value = 3;
  root.left = &left;
  root.right = &right;
  left.parent = &root;
  right.parent = &root;

  i32 order[3] = {0};
  i32 idx = 0;
  struct test_node* it = &root;
  for (it = &root; it != nullptr; it = (struct test_node*)binary_tree_next_preorder_ptr(&root, it)) {
    order[idx++] = it->value;
  }

  EXPECT_EQ(1, order[0]);
  EXPECT_EQ(2, order[1]);
  EXPECT_EQ(3, order[2]);
}

TEST(containers_binary_tree_test, foreach_inorder) {
  struct test_node root = {0};
  struct test_node left = {0};
  struct test_node right = {0};

  root.value = 2;
  left.value = 1;
  right.value = 3;
  root.left = &left;
  root.right = &right;
  left.parent = &root;
  right.parent = &root;

  i32 order[3] = {0};
  i32 idx = 0;
  struct test_node* it = (struct test_node*)binary_tree_first_inorder_ptr(&root);
  for (; it != nullptr; it = (struct test_node*)binary_tree_next_inorder_ptr(&root, it)) {
    order[idx++] = it->value;
  }

  EXPECT_EQ(1, order[0]);
  EXPECT_EQ(2, order[1]);
  EXPECT_EQ(3, order[2]);
}

TEST(containers_binary_tree_test, foreach_postorder) {
  struct test_node root = {0};
  struct test_node left = {0};
  struct test_node right = {0};

  root.value = 3;
  left.value = 1;
  right.value = 2;
  root.left = &left;
  root.right = &right;
  left.parent = &root;
  right.parent = &root;

  i32 order[3] = {0};
  i32 idx = 0;
  struct test_node* it = (struct test_node*)binary_tree_first_postorder_ptr(&root);
  for (; it != nullptr; it = (struct test_node*)binary_tree_next_postorder_ptr(&root, it)) {
    order[idx++] = it->value;
  }

  EXPECT_EQ(1, order[0]);
  EXPECT_EQ(2, order[1]);
  EXPECT_EQ(3, order[2]);
}
